//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_GPS_TEST_PUBWRAPPER_HH
#define CLICK_GPS_TEST_PUBWRAPPER_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

#include "GPS_GUID.hh"
#include "GPS_NA.hh"

CLICK_DECLS

class GPS_TEST_PubWrapper : public Element {
public:
    GPS_TEST_PubWrapper() CLICK_COLD;

    ~GPS_TEST_PubWrapper() override CLICK_COLD;

    const char *class_name() const override { return "GPS_TEST_PubWrapper"; }

    const char *port_count() const override { return "1/1"; }

    const char *processing() const override { return AGNOSTIC; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    Packet *simple_action(Packet *p) override ;

private:
    gps_guid_t _srcGuid, _dstGuid;
    gps_na_t _srcNa, _dstNa;
};

CLICK_ENDDECLS

#endif //CLICK_GPS_TEST_PUBWRAPPER_HH
