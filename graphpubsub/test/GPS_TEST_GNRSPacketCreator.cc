//
// Created by ubuntu on 9/21/18.
//

#include "GPS_TEST_GNRSPacketCreator.hh"
#include "../elements/GPS_PacketHeader.hh"
#include "../elements/GPS_PacketAnno.h"
#include <click/args.hh>
#include <click/error.hh>

CLICK_DECLS

class GPS_TEST_GNRSRequestPacketCreator : public GPS_TEST_AbstractGNRSPacketCreator {
public:

    GPS_TEST_GNRSRequestPacketCreator(const gps_guid_t &_dstGuid, const gps_na_t &_srcNa, const gps_na_t &_dstNa,
                                      ErrorHandler *errh)
            : _dstGuid(_dstGuid), _srcNa(_srcNa), _dstNa(_dstNa) {
        errh->message("GPS_TEST_GNRSRequestPacketCreator GUID=%s, srcNa=%s, dstNa=%s",
                      gps_guid_unparse(&_dstGuid).c_str(),
                      gps_na_unparse(&_srcNa).c_str(),
                      gps_na_unparse(&_dstNa).c_str());
    }

    ~GPS_TEST_GNRSRequestPacketCreator() override = default;

    Packet *getPacket() override {
        auto pkt = Packet::make(GPS_DEFAULT_HEAD_ROOM, nullptr, sizeof(gps_packet_gnrsRequest_t), 0);
        *PRIO_ANNO(pkt) = GPS_PACKET_PRIO_GNRS_REQ;
        gps_packet_gnrsRequest_init(pkt->data(), &_dstGuid, &_srcNa, &_dstNa);
        return pkt;
    }

private:
    gps_guid_t _dstGuid;
    gps_na_t _srcNa, _dstNa;

};

class GPS_TEST_GNRSResponsePacketCreator : public GPS_TEST_AbstractGNRSPacketCreator {
public:
    GPS_TEST_GNRSResponsePacketCreator(const gps_guid_t &_srcGuid, const gps_na &_srcNa, const gps_na &_dstNa,
                                       const gps_na &_requestedGuidNa, uint32_t _version, uint32_t _nonce,
                                       ErrorHandler *errh)
            : _srcGuid(_srcGuid), _srcNa(_srcNa), _dstNa(_dstNa), _requestedGuidNa(_requestedGuidNa),
              _version(_version),
              _nonce(_nonce) {
        errh->message(
                "GPS_TEST_GNRSResponsePacketCreator GUID=%s, srcNa=%s, dstNa=%s, requestedGuidNa=%s, version=%d, nonce=0x%08x",
                gps_guid_unparse(&_srcGuid).c_str(),
                gps_na_unparse(&_srcNa).c_str(),
                gps_na_unparse(&_dstNa).c_str(),
                gps_na_unparse(&_requestedGuidNa).c_str(),
                _version, _nonce);

    }

    ~GPS_TEST_GNRSResponsePacketCreator() override = default;

    Packet *getPacket() override {
        auto pkt = Packet::make(GPS_DEFAULT_HEAD_ROOM, nullptr, sizeof(gps_packet_gnrsResponse_t), 0);
        *PRIO_ANNO(pkt) = GPS_PACKET_PRIO_GNRS_RESP;
        gps_packet_gnrsResponse_init(pkt->data(), &_srcGuid, &_srcNa, &_dstNa, &_requestedGuidNa, _version, _nonce);
        return pkt;
    }

private:
    gps_guid_t _srcGuid;
    gps_na _srcNa, _dstNa, _requestedGuidNa;
    uint32_t _version, _nonce;
};


GPS_TEST_GNRSPacketCreator::GPS_TEST_GNRSPacketCreator() : _creator(nullptr) {
}

GPS_TEST_GNRSPacketCreator::~GPS_TEST_GNRSPacketCreator() {
    delete _creator;
}

int GPS_TEST_GNRSPacketCreator::configure(Vector<String> &conf, ErrorHandler *errh) {
    uint8_t type;
    uint32_t version = 0, nonce = 0;
    uint32_t numGuid, numSrcNa = 0, numDstNa = 0, numRequestedGuidNa = 0;
    if (Args(conf, this, errh)
                .read_mp("TYPE", type)
                .read_mp("GUID", numGuid)
                .read_p("SRC_NA", numSrcNa)
                .read_p("DST_NA", numDstNa)
                .read_p("REQUESTED_GUID_NA", numRequestedGuidNa)
                .read_p("VERSION", version)
                .read_p("NONCE", nonce)
                .complete() < 0) {
        return -1;
    }
    gps_guid_t guid;
    gps_na_t srcNa, dstNa, requestedGuidNa;
    gps_guid_set_val(&guid, numGuid);
    gps_na_set_val(&srcNa, numSrcNa);
    gps_na_set_val(&dstNa, numDstNa);
    gps_na_set_val(&requestedGuidNa, numRequestedGuidNa);

    switch (type) {
        case GPS_PACKET_TYPE_GNRS_REQ:
            _creator = new GPS_TEST_GNRSRequestPacketCreator(guid, srcNa, dstNa, errh);
            break;
        case GPS_PACKET_TYPE_GNRS_RESP:
            _creator = new GPS_TEST_GNRSResponsePacketCreator(guid, srcNa, dstNa, requestedGuidNa, version, nonce,
                                                              errh);
            break;
        default:
            errh->error("Illegal type (0x%02x), should be GNRSReq(0x%02x) or GNRSResp(0x%02x)",
                        type, GPS_PACKET_TYPE_GNRS_REQ, GPS_PACKET_TYPE_GNRS_RESP);
            return -1;
    }
    return 0;
}

Packet *GPS_TEST_GNRSPacketCreator::simple_action(Packet *p) {
    p->kill();
    return _creator->getPacket();
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_TEST_GNRSPacketCreator)
