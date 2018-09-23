//
// Created by ubuntu on 9/23/18.
//

#ifndef CLICK_GPS_PRIOANNOTATOR_HH
#define CLICK_GPS_PRIOANNOTATOR_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class GPS_PrioAnnotator : public Element {
public:
    GPS_PrioAnnotator() CLICK_COLD;

    ~GPS_PrioAnnotator() override CLICK_COLD;

    const char *class_name() const override { return "GPS_PrioAnnotator"; };

    const char *port_count() const override { return PORTS_1_1; };

    const char *processing() const override { return AGNOSTIC; };

    Packet *simple_action(Packet *p) override;

};

CLICK_ENDDECLS

#endif //CLICK_GPS_PRIOANNOTATOR_HH
