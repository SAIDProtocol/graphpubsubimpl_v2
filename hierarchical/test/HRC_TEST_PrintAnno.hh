//
// Created by jiachen on 10/3/18.
//

#ifndef CLICK_HRC_TEST_PRINTANNO_HH
#define CLICK_HRC_TEST_PRINTANNO_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <click/string.hh>

CLICK_DECLS

class HRC_TEST_PrintAnno : public Element {
public:
    HRC_TEST_PrintAnno() CLICK_COLD;

    ~HRC_TEST_PrintAnno() override CLICK_COLD;

    const char *class_name() const override { return "HRC_TEST_PrintAnno"; };

    const char *port_count() const override { return PORTS_1_1; };

    const char *processing() const override { return AGNOSTIC; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    Packet *simple_action(Packet *p) override;
private:
    String _label;
};

CLICK_ENDDECLS

#endif //CLICK_HRC_TEST_PRINTANNO_HH
