//
// Created by jiachen on 9/20/18.
//

#include "GPS_TEST_PubWrapper.hh"
#include "../elements/GPS_RoutingTable.hh"
#include "../elements/GPS_PacketHeader.hh"
#include "../elements/GPS_PacketAnno.h"
#include <vector>
#include <random>
#include <click/args.hh>
#include <click/error.hh>
#include <click/straccum.hh>
#include <cstdlib>

CLICK_DECLS

class GPS_TEST_StaticPubWrapper : public GPS_TEST_AbstractPubWrapper {
public:

    GPS_TEST_StaticPubWrapper(const gps_guid_t &srcGuid, const gps_guid_t &dstGuid, const gps_na_t &srcNa,
                              const gps_na_t &dstNa,
                              ErrorHandler *errh)
            : _srcGuid(srcGuid), _dstGuid(dstGuid), _srcNa(srcNa), _dstNa(dstNa) {
        errh->message("GPS_TEST_StaticPubWrapper srcGuid=%s, dstGuid=%s, srcNa=%s, dstNa=%s",
                      gps_guid_unparse(&_srcGuid).c_str(),
                      gps_guid_unparse(&_dstGuid).c_str(),
                      gps_na_unparse(&_srcNa).c_str(),
                      gps_na_unparse(&_dstNa).c_str());
    }

    ~GPS_TEST_StaticPubWrapper() override = default;

    Packet *wrapPacket(Packet *p) override {
        auto payloadSize = p->length();

        auto newPkt = p->push(sizeof(gps_packet_publication_t));

        gps_packet_publication_init(newPkt->data(), &_srcGuid, &_dstGuid, &_srcNa, &_dstNa, payloadSize);
        *PRIO_ANNO(p) = GPS_PACKET_PRIO_PUBLICATION;

        return newPkt;
    }

private:
    gps_guid_t _srcGuid, _dstGuid;
    gps_na_t _srcNa, _dstNa;
};

class GPS_TEST_RandomPubWrapper : public GPS_TEST_AbstractPubWrapper {
public:

    GPS_TEST_RandomPubWrapper(const gps_guid_t &srcGuid, const gps_guid_t &dstGuid, const gps_na_t &srcNa,
                              const std::unordered_map<gps_na_t, GPS_RoutingTableEntry> &dstNas, ErrorHandler *errh)
            : _srcGuid(srcGuid), _dstGuid(dstGuid), _srcNa(srcNa), _rand(0) {
        for (auto &it : dstNas) _dstNas.push_back(it.first);

        StringAccum sa;
        for (auto &it : _dstNas) {
            sa.append(" ");
            sa.append(gps_na_unparse(&it).c_str());
        }

        errh->message("GPS_TEST_RandomPubWrapper srcGuid=%s, dstGuid=%s, srcNa=%s dstNas(%d)=%s",
                      gps_guid_unparse(&_srcGuid).c_str(),
                      gps_guid_unparse(&_dstGuid).c_str(),
                      gps_na_unparse(&_srcNa).c_str(),
                      _dstNas.size(),
                      sa.c_str());
    }

    ~GPS_TEST_RandomPubWrapper() override = default;

    Packet *wrapPacket(Packet *p) override {
        auto payloadSize = p->length();

        auto newPkt = p->push(sizeof(gps_packet_publication_t));

        gps_na_t *dstNa = &_dstNas[_rand() % _dstNas.size()];
        gps_packet_publication_init(newPkt->data(), &_srcGuid, &_dstGuid, &_srcNa, dstNa, payloadSize);
        *PRIO_ANNO(p) = GPS_PACKET_PRIO_PUBLICATION;

        return newPkt;
    }

private:
    gps_guid_t _srcGuid, _dstGuid;
    gps_na_t _srcNa;
    std::vector<gps_na_t> _dstNas;

    std::default_random_engine _rand;
};

GPS_TEST_PubWrapper::GPS_TEST_PubWrapper() = default;

GPS_TEST_PubWrapper::~GPS_TEST_PubWrapper() {
    delete _wrapper;
};

int GPS_TEST_PubWrapper::configure(Vector<String> &conf, ErrorHandler *errh) {
    uint32_t numSrcGuid = 0, numDstGuid, numSrcNa = 0, numDstNa = 0;
    String dstNaFile;
    bool hasDstNaFile;
    gps_guid_t srcGuid, dstGuid;
    gps_na_t srcNa, dstNa;
    if (Args(conf, this, errh)
                .read_p("SRC_GUID", numSrcGuid)
                .read_mp("DST_GUID", numDstGuid)
                .read_p("SRC_NA", numSrcNa)
                .read_p("DST_NA", numDstNa)
                .read_p("DST_NA_FILE", dstNaFile)
                .read_status(hasDstNaFile)
                .complete() < 0) {
        return -1;
    }
    gps_guid_set_val(&srcGuid, numSrcGuid);
    gps_guid_set_val(&dstGuid, numDstGuid);

    gps_na_set_val(&srcNa, numSrcNa);

    if (hasDstNaFile) {
        std::unordered_map<gps_na_t, GPS_RoutingTableEntry> naNextHops;
        if (GPS_RoutingTable::parseArgFile(dstNaFile, errh, naNextHops) < 0) return -1;
        _wrapper = new GPS_TEST_RandomPubWrapper(srcGuid, dstGuid, srcNa, naNextHops, errh);
    } else {
        gps_na_set_val(&dstNa, numDstNa);
        _wrapper = new GPS_TEST_StaticPubWrapper(srcGuid, dstGuid, srcNa, dstNa, errh);
    }
    return 0;
}

Packet *GPS_TEST_PubWrapper::simple_action(Packet *p) {
    return _wrapper->wrapPacket(p);
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_TEST_PubWrapper)
