//
// Created by ubuntu on 10/4/18.
//

#ifndef CLICK_HRC_ETHERANNOTATOR_HH
#define CLICK_HRC_ETHERANNOTATOR_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class HRC_EtherAnnotator : public Element {
public:
    HRC_EtherAnnotator() CLICK_COLD;

    ~HRC_EtherAnnotator() override  CLICK_COLD;

    const char *class_name() const override { return "HRC_EtherAnnotator"; };

    const char *port_count() const override { return "-1/-1"; };

    const char *processing() const override { return AGNOSTIC; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override  CLICK_COLD;

    Packet *simple_action(Packet *p) override;

private:
    uint8_t _myPort;
};

CLICK_ENDDECLS

#endif //CLICK_HRC_ETHERANNOTATOR_HH
