//
// Created by ubuntu on 9/22/18.
//

#ifndef CLICK_GPS_ETHERANNOTATOR_HH
#define CLICK_GPS_ETHERANNOTATOR_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <ostream>

CLICK_DECLS

class GPS_EtherAnnotator : public Element {
public:
    GPS_EtherAnnotator() CLICK_COLD;

    ~GPS_EtherAnnotator() override CLICK_COLD;

    const char *class_name() const override { return "GPS_EtherAnnotator"; };

    const char *port_count() const override { return "-1/-1"; };

    const char *processing() const override { return AGNOSTIC; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    Packet *simple_action(Packet *p) override;
private:
    uint8_t _myPort;
};

CLICK_ENDDECLS

#endif //CLICK_GPS_ETHERANNOTATOR_HH
