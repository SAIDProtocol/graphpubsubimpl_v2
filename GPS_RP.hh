//
// Created by ubuntu on 9/21/18.
//

#ifndef CLICK_GPS_RP_HH
#define CLICK_GPS_RP_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <unordered_map>
#include <unordered_set>
#include "GPS_GUID.hh"
#include "GPS_NA.hh"
#include "GPS_ReadWriteLock.hh"

CLICK_DECLS

class GPS_RP : public Element {
public:
    static const int IN_PORT_DATA = 0;
    static const int OUT_PORT_TO_SUBSCRIBERS = 0;
    static const int OUT_PORT_TO_OTHER_RP = 1;
//    static const int OUT_PORT_DISCARD = 2;

    GPS_RP() CLICK_COLD;

    ~GPS_RP() override CLICK_COLD;


    const char *class_name() const override { return "GPS_RP"; };

    const char *port_count() const override { return "-1/-2"; };

    const char *processing() const override { return PUSH; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void push(int port, Packet *p) override;

private:
    void handleData(Packet *packet);
    int parseArgFile(const String &fileName, ErrorHandler *errh);

    gps_na_t _myNa;
    std::unordered_map<gps_guid_t, std::unordered_set<gps_guid_t>> _mappingTable;
    GPS_ReadWriteLock _lock;
};


CLICK_ENDDECLS

#endif //CLICK_GPS_RP_HH
