//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_GPS_TEST_PRINTPACKET_HH
#define CLICK_GPS_TEST_PRINTPACKET_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class GPS_TEST_PrintPacket : public Element {
public:
    GPS_TEST_PrintPacket() CLICK_COLD;

    ~GPS_TEST_PrintPacket() override CLICK_COLD;

    const char *class_name() const override { return "GPS_TEST_PrintPacket"; }

    const char *port_count() const override { return PORTS_1_1; }

    const char *processing() const override { return AGNOSTIC; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    Packet *simple_action(Packet *p) override;

private:
    String _label;

};

CLICK_ENDDECLS

#endif //CLICK_GPS_TEST_PRINTPACKET_HH
