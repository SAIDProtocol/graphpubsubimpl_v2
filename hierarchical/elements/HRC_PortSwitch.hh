//
// Created by ubuntu on 10/4/18.
//

#ifndef CLICK_HRC_PORTSWITCH_HH
#define CLICK_HRC_PORTSWITCH_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class HRC_PortSwitch : public Element {
public:
    HRC_PortSwitch() CLICK_COLD;

    ~HRC_PortSwitch() override CLICK_COLD;

    const char *class_name() const override { return "HRC_PortSwitch"; };

    const char *port_count() const override { return "-/-"; };

    const char *processing() const override { return PUSH; };

    void push(int port, Packet *p) override;


};

CLICK_ENDDECLS

#endif //CLICK_HRC_PORTSWITCH_HH
