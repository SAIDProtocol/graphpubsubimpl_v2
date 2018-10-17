//
// Created by jiachen on 10/17/18.
//

#include "GPS_TEST_PubGenerator.hh"
#include "../elements/GPS_PacketHeader.hh"
#include "../elements/GPS_RoutingTable.hh"
#include "../elements/GPS_SubscriptionTable.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <ctime>

CLICK_DECLS


GPS_TEST_PubGenerator::GPS_TEST_PubGenerator() :
        _rand(0),
        _packet(nullptr),
        _pktCnt(0),
        _generatePacket(&GPS_TEST_PubGenerator::generateGuidPacket) {
}

GPS_TEST_PubGenerator::~GPS_TEST_PubGenerator() = default;

int GPS_TEST_PubGenerator::configure(Vector<String> &conf, ErrorHandler *errh) {
    uint32_t numSrcNa, numSrcGuid;
    uint32_t payloadSize;
    String dstNaFile, dstGuidFile;
    bool hasDstNaFile, hasDstGuidFile;
    if (Args(conf, this, errh)
                .read_mp("SRC_NA", numSrcNa)
                .read_mp("SRC_GUID", numSrcGuid)
                .read_mp("PAYLOAD_SIZE", payloadSize)
                .read("DST_NA_FILE", FilenameArg(), dstNaFile)
                .read_status(hasDstNaFile)
                .read("DST_GUID_FILE", FilenameArg(), dstGuidFile)
                .read_status(hasDstGuidFile)
                .complete() < 0)
        return -1;

    auto wp = Packet::make(sizeof(gps_packet_publication_t) + payloadSize);
    auto header = wp->data();
    gps_packet_set_type(header, GPS_PACKET_TYPE_PUBLICATION);
    gps_guid_set_val(gps_packet_publication_get_srcGuid(header), numSrcGuid);
    gps_na_set_val(gps_packet_publication_get_srcNa(header), numSrcNa);
    gps_guid_set_val(gps_packet_publication_get_dstGuid(header), 0U);
    gps_na_set_val(gps_packet_publication_get_dstNa(header), 0U);
    gps_packet_publication_set_size(header, payloadSize);
    _packet = wp;

    if (hasDstNaFile) {
        if (hasDstGuidFile) {
            errh->error("Cannot set DST_NA_FILE and DST_GUID_FILE at the same time.");
            return -1;
        }
        std::unordered_map<gps_na_t, GPS_RoutingTableEntry> naNextHops;
        if (GPS_RoutingTable::parseArgFile(dstNaFile, errh, naNextHops) < 0) return -1;
        for (auto &it : naNextHops) _dstNas.push_back(it.first);
        errh->debug("NA table size: %d", _dstNas.size());
        _generatePacket = &GPS_TEST_PubGenerator::generateNaPacket;
    } else if (hasDstGuidFile) {
        std::unordered_map<gps_guid_t, std::unordered_set<gps_na_t>> subscriptions;
        if (GPS_SubscriptionTable::parseArgFile(dstGuidFile, errh, subscriptions) < 0) return -1;
        for (auto &it : subscriptions) _dstGuids.push_back(it.first);
        errh->debug("GUID table size: %d", _dstGuids.size());
        _generatePacket = &GPS_TEST_PubGenerator::generateGuidPacket;
    } else {
        errh->error("Have to set DST_NA_FILE or DST_GUID_FILE.");
        return -1;
    }

    return 0;
}

Packet *GPS_TEST_PubGenerator::pull(int) {
    return (this->*_generatePacket)();
}


Packet *GPS_TEST_PubGenerator::generateNaPacket() {
    auto wp = _packet->clone()->uniqueify();
    gps_na_set_val(gps_packet_publication_get_dstNa(wp->data()), &_dstNas[_rand() % _dstNas.size()]);
    return setCount(wp);
}

Packet *GPS_TEST_PubGenerator::generateGuidPacket() {
    auto wp = _packet->clone()->uniqueify();
    gps_guid_set_val(gps_packet_publication_get_dstGuid(wp->data()), &_dstGuids[_rand() % _dstGuids.size()]);
    return setCount(wp);
}

WritablePacket *GPS_TEST_PubGenerator::setCount(WritablePacket *wp) {
    *(uint32_t *) (wp->data() + sizeof(gps_packet_publication_t)) = htonl(++_pktCnt);
    wp->timestamp_anno().assign_now();
//    ErrorHandler::default_handler()->debug("now: %s", wp->timestamp_anno().unparse().c_str());
    return wp;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_TEST_PubGenerator)