//
// Created by ubuntu on 9/23/18.
//

#ifndef CLICK_GPS_PORTSWITCH_HH
#define CLICK_GPS_PORTSWITCH_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class GPS_PortSwitch : public Element {
public:
    GPS_PortSwitch() CLICK_COLD;

    ~GPS_PortSwitch() override CLICK_COLD;

    const char *class_name() const override { return "GPS_PortSwitch"; };

    const char *port_count() const override { return "-/-"; };

    const char *processing() const override { return PUSH; };

    void push(int port, Packet *p) override;
};

CLICK_ENDDECLS

#endif //CLICK_GPS_PORTSWITCH_HH
