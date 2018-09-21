//
// Created by ubuntu on 9/21/18.
//

#ifndef CLICK_GPS_GNRSCACHE_HH
#define CLICK_GPS_GNRSCACHE_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "GPS_GUID.hh"
#include "GPS_NA.hh"
#include "GPS_ReadWriteLock.hh"

CLICK_DECLS

class GPS_GUIDInfo {
public:
    GPS_GUIDInfo() : _na(), _version(0) {}

//    GPS_GUIDInfo(gps_na_t *na, uint32_t version) : _version(version) { gps_na_set_val(&_na, na); }

    const gps_na_t *getNa() const { return &_na; }

    uint32_t getVersion() const { return _version; }

    bool set(const gps_na_t *na, uint32_t version) {
//        click_chatter(">>>>> version: %d, _version: %d", version, _version);
        if (version <= _version) return false;
        gps_na_set_val(&_na, na);
        _version = version;
        return true;
    }

private:
    gps_na_t _na;
    uint32_t _version;
};


class GPS_GNRSCache : public Element {
public:
    static const int NONCE_CACHE_SIZE = 3000;

    static const int IN_PORT_DATA = 0;
    static const int IN_PORT_GNRS_REQ = 1;
    static const int IN_PORT_GNRS_RESP = 2;
    static const int OUT_PORT_TO_ROUTER = 0;
    static const int OUT_PORT_TO_ENDHOST = 1;
//    static const int OUT_PORT_DISCARD = 2;

    GPS_GNRSCache() CLICK_COLD;

    ~GPS_GNRSCache() override CLICK_COLD;

    const char *class_name() const override { return "GPS_GNRSCache"; }

    const char *port_count() const override { return "-3/-2"; }

    const char *processing() const override { return PUSH; }

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void push(int port, Packet *p) override;

private:
    std::unordered_map<gps_guid_t, GPS_GUIDInfo> _guidNas;
    std::unordered_map<gps_guid_t, std::vector<Packet *>> _pendingPackets;

    GPS_ReadWriteLock _cacheLock, _pendingPacketsLock;
    gps_na_t _myNa, _gnrsNa;

    void emit(Packet *packet);

    void handleData(Packet *packet);

    void handleGNRSRequest(Packet *packet);

    void handleGNRSResponse(Packet *packet);

    std::deque<uint32_t> _nonceCacheQueue;
    std::unordered_set<uint32_t> _nonceCacheTable;

    // returns true if nonce is added, false if nonce already in cache
    bool addNonce(uint32_t nonce);

};


CLICK_ENDDECLS

#endif //CLICK_GPS_GNRSCACHE_HH
