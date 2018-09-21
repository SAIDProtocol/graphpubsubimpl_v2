//
// Created by ubuntu on 9/20/18.
//

#ifndef CLICK_V2_GPS_ROUTINGTABLE_HH
#define CLICK_V2_GPS_ROUTINGTABLE_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <unordered_map>

#include "GPS_NA.hh"

CLICK_DECLS

class GPS_RoutingTableEntry {
public:

    inline GPS_RoutingTableEntry() : _minNextHop(), _minDistance(UINT32_MAX) {

    }

    inline ~GPS_RoutingTableEntry() = default;

    uint32_t getMinDistance() const { return _minDistance; }

    const gps_na_t *getMinNextHop() const { return &_minNextHop; }

    const std::unordered_map<gps_na_t, uint32_t> *getCandidates() const { return &_candidates; }

    inline void insertValue(const gps_na_t *na, uint32_t distance) {
        // simplified implementation.
        // Should: update na/distance pair first, and then find min
        // Because it can be an update, instead of insert
        _candidates[*na] = distance;
    }

    inline void updateMinimal() {
        gps_na_clear(&_minNextHop);
        _minDistance = UINT32_MAX;
        for (auto &it : _candidates) {
            if (it.second < _minDistance) {
                gps_na_set_val(&_minNextHop, &it.first);
                _minDistance = it.second;
            }
        }
    }

private:
    std::unordered_map<gps_na_t, uint32_t> _candidates;
    gps_na_t _minNextHop;
    uint32_t _minDistance;
};


class GPS_RoutingTable : public Element {
public:
    static const int IN_PORT_DATA = 0;
    static const int OUT_PORT_DATA = 0;
    static const int OUT_PORT_DISCARD = 1;

    GPS_RoutingTable() CLICK_COLD;

    ~GPS_RoutingTable() override CLICK_COLD;

    const char *class_name() const override { return "GPS_RoutingTable"; }

    const char *port_count() const override { return "-1/-2"; }

    const char *processing() const override { return PUSH; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void push(int port, Packet *p) override;

private:
    std::unordered_map<gps_na_t, GPS_RoutingTableEntry> _naNextHops;

    int parseArgFile(const String &fileName, ErrorHandler *errh);

    void handleData(Packet *p);

};

CLICK_ENDDECLS


#endif //CLICK_V2_GPS_ROUTINGTABLE_HH
