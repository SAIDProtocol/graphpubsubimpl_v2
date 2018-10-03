//
// Created by jiachen on 10/3/18.
//

#ifndef CLICK_HRC_PRIOANNOTATOR_HH
#define CLICK_HRC_PRIOANNOTATOR_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class HRC_PrioAnnotator : public Element {
public:
    HRC_PrioAnnotator() CLICK_COLD;

    ~HRC_PrioAnnotator() override CLICK_COLD;

    const char *class_name() const override { return "HRC_PrioAnnotator"; };

    const char *port_count() const override { return PORTS_1_1; };

    const char *processing() const override { return AGNOSTIC; };

    Packet *simple_action(Packet *p) override;

};

CLICK_ENDDECLS
#endif //CLICK_HRC_PRIOANNOTATOR_HH
