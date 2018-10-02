//
// Created by jiachen on 9/25/18.
//

#ifndef CLICK_HRC_TEST_CONTENTNAME_HH
#define CLICK_HRC_TEST_CONTENTNAME_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class HRC_TEST_ContentName : public Element {
public:
    HRC_TEST_ContentName() CLICK_COLD;

    ~HRC_TEST_ContentName() override CLICK_COLD;

    const char *class_name() const override { return "HRC_TEST_ContentName"; };

    const char *port_count() const override { return PORTS_0_0; };

    const char *processing() const override { return AGNOSTIC; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    Packet *simple_action(Packet *p) override;

};

CLICK_ENDDECLS

#endif //CLICK_HRC_TEST_CONTENTNAME_HH
