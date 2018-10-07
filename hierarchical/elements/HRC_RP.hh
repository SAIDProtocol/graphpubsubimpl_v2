//
// Created by jiachen on 10/7/18.
//

#ifndef CLICK_HRC_RP_HH
#define CLICK_HRC_RP_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/error.hh>
#include <click/glue.hh>

CLICK_DECLS

class HRC_RP : public Element {
public:
    HRC_RP();

    ~HRC_RP() override;

    const char *class_name() const override { return "HRC_RP"; }

    const char *port_count() const override { return PORTS_1_1; }

    const char *processing() const override { return PUSH; }

    void push(int port, Packet *p) override ;

};

CLICK_ENDDECLS

#endif //CLICK_HRC_RP_HH
