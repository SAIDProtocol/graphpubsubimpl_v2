//
// Created by ubuntu on 9/21/18.
//

#include "GPS_GNRSCache.hh"
#include "GPS_PacketAnno.h"
#include "GPS_PacketHeader.hh"
#include <click/args.hh>


CLICK_DECLS

GPS_GNRSCache::GPS_GNRSCache() = default;

GPS_GNRSCache::~GPS_GNRSCache() = default;

int GPS_GNRSCache::configure(Vector<String> &conf, ErrorHandler *errh) {
    uint32_t numGnrsNa, numNa;
    if (Args(conf, this, errh)
                .read_mp("GNRS_NA", numGnrsNa)
                .read_mp("NA", numNa)
                .complete() < 0) {
        return -1;
    }

    gps_na_set_val(&_myNa, numNa);
    gps_na_set_val(&_gnrsNa, numGnrsNa);
    return 0;
}

void GPS_GNRSCache::push(int port, Packet *p) {
    switch (port) {
        case IN_PORT_DATA:
            handleData(p);
            break;
        case IN_PORT_GNRS_REQ:
            handleGNRSRequest(p);
            break;
        case IN_PORT_GNRS_RESP:
            handleGNRSResponse(p);
            break;
        default:
            // from other ports?? should not reach here!
            click_chatter("[GPS_GNRSCache::push] Got packet from unexpected port: %d, discarding...", port);
            p->kill();
            break;
    }
}

void GPS_GNRSCache::emit(Packet *packet) {
    auto dstNa = gps_packet_application_get_dstNa(packet->data());
    if (*dstNa == _myNa) {
        checked_output_push(OUT_PORT_TO_ENDHOST, packet);
    } else {
        checked_output_push(OUT_PORT_TO_ROUTER, packet);
    }
}

void GPS_GNRSCache::handleData(Packet *packet) {
    auto header = packet->data();

    if (unlikely(!gps_packet_is_application(header))) {
        click_chatter("[GPS_GNRSCache::handleData] Error packet type: 0x%x, should match mask: 0x%x",
                      gps_packet_get_type(header), GPS_PACKET_TYPE_APPLICATION_MASK);
        packet->kill();
    } else {
        const gps_na_t *dstNa = gps_packet_application_get_dstNa(header);
        if (gps_na_is_empty(dstNa)) {
            const gps_guid_t *dstGuid = gps_packet_application_get_dstGuid(header);
            Packet *toEmit = nullptr;
            _cacheLock.acquire_read();
            auto it = _guidNas.find(*dstGuid);
            if (it == _guidNas.end()) {
                // find in pending packets, if there is already one, put and discard
                // else create a new entry, send a request packet
                _pendingPacketsLock.acquire_write();
                auto &pendingPackets = _pendingPackets[*dstGuid];
                if (pendingPackets.empty()) {
                    auto request = Packet::make(GPS_DEFAULT_HEAD_ROOM, nullptr, sizeof(gps_packet_gnrsRequest_t), 0);
                    *PRIO_ANNO(request) = GPS_PACKET_PRIO_GNRS_REQ;
                    auto requestHeader = request->data();
                    // fill req
                    gps_packet_gnrsRequest_init(requestHeader, dstGuid, &_myNa, &_gnrsNa);
                    toEmit = request;
                }
                pendingPackets.push_back(packet);
                _pendingPacketsLock.release_write();
            } else {
                auto p = packet->uniqueify();
                gps_packet_application_set_dstNa(p->data(), it->second.getNa());
                toEmit = p;
            }
            _cacheLock.release_read();
            if (toEmit) emit(toEmit);
        } else {
            emit(packet);
        }
    }
}

void GPS_GNRSCache::handleGNRSRequest(Packet *packet) {
    auto header = packet->data();
    auto type = gps_packet_get_type(header);

    if (unlikely(type != GPS_PACKET_TYPE_GNRS_REQ)) {
        click_chatter("[GPS_GNRSCache::handleGNRSRequest] Error packet type: 0x%x, should be GNRS_Request: 0x%x", type,
                      GPS_PACKET_TYPE_GNRS_REQ);
        packet->kill();
    } else {
        const gps_guid_t *guid = gps_packet_gnrsRequest_get_dstGuid(header);
        _cacheLock.acquire_read();
        auto it = _guidNas.find(*guid);
        if (it == _guidNas.end()) { // cannot find
            _cacheLock.release_read();
            if (unlikely(gps_na_is_empty(gps_packet_gnrsRequest_get_dstNa(header)))) {
                auto writablePacket = packet->uniqueify();
                auto writableHeader = writablePacket->data();
                gps_packet_gnrsRequest_set_dstNa(writableHeader, &_gnrsNa);
                emit(writablePacket);
            } else {
                emit(packet);
            }
        } else {
            auto response = Packet::make(GPS_DEFAULT_HEAD_ROOM, nullptr, sizeof(gps_packet_gnrsResponse_t), 0);
            *PRIO_ANNO(response) = GPS_PACKET_PRIO_GNRS_RESP;
            auto responseHeader = response->data();
            auto nonce = click_random(0, UINT32_MAX);
            gps_packet_gnrsResponse_init(responseHeader, guid, &_myNa, gps_packet_gnrsRequest_get_srcNa(header),
                                         it->second.getNa(), it->second.getVersion(), nonce);
            _cacheLock.release_read();
            packet->kill();
            emit(response);
        }
    }
}

void GPS_GNRSCache::handleGNRSResponse(Packet *packet) {
    auto header = packet->data();
    auto type = gps_packet_get_type(header);
    if (unlikely(type != GPS_PACKET_TYPE_GNRS_RESP)) {
        click_chatter("[GPS_GNRSCache::handleGNRSResponse] Error packet type: 0x%x, should be GNRS_Response: 0x%x",
                      type, GPS_PACKET_TYPE_GNRS_RESP);
        packet->kill();
    } else {
        auto dstNa = gps_packet_gnrsResponse_get_dstNa(header);
        if (unlikely(gps_na_is_broadcast(dstNa) && !addNonce(gps_packet_gnrsResponse_get_nonce(header)))) {
            packet->kill();
        } else {
            bool toMyself = (*dstNa) == _myNa;
            auto guid = gps_packet_gnrsResponse_get_srcGuid(header);
            auto requestedGuidNa = gps_packet_gnrsResponse_get_requestedGuidNa(header);

            _cacheLock.acquire_write();

            auto it = _pendingPackets.find(*guid);
            bool hasPendingPackets = it != _pendingPackets.end();

            bool cacheUpdated;
            if (hasPendingPackets || toMyself) {
                // packets pending on this GUID, or destination is myself, update cache mandate
                cacheUpdated = _guidNas[*guid].set(requestedGuidNa, gps_packet_gnrsResponse_get_version(header));
            } else {
                // update cache selective
                auto it2 = _guidNas.find(*guid);
                cacheUpdated = (it2 == _guidNas.end()) ?
                               false :
                               it2->second.set(requestedGuidNa, gps_packet_gnrsResponse_get_version(header));
            }

//            click_chatter(">>>> toMyself: %d, cache updated: %d, has pending packets: %d, version: %d", toMyself, cacheUpdated, hasPendingPackets, ntohl(resp->version));
            if (cacheUpdated && hasPendingPackets) {
                for (auto &it3 : it->second) {
                    auto p = it3->uniqueify();
                    gps_packet_application_set_dstNa(p->data(), requestedGuidNa);
                    emit(p);
                }
                _pendingPackets.erase(it);
            }
            _cacheLock.release_write();
            if (toMyself) packet->kill();
            else emit(packet);
        }
    }
}

bool GPS_GNRSCache::addNonce(uint32_t nonce) {
    if (!_nonceCacheTable.insert(nonce).second)
        return false;

    _nonceCacheQueue.push_back(nonce);
    while (_nonceCacheQueue.size() > NONCE_CACHE_SIZE) {
        uint32_t toRemove = _nonceCacheQueue.front();
        _nonceCacheQueue.pop_front();
        _nonceCacheTable.erase(toRemove);
    }
    return true;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_GNRSCache)
