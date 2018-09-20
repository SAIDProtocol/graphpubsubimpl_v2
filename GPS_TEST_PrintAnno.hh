//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_GPS_TEST_PRINTANNO_HH
#define CLICK_GPS_TEST_PRINTANNO_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class GPS_TEST_PrintAnno : public Element {
public:
    GPS_TEST_PrintAnno() CLICK_COLD;

    ~GPS_TEST_PrintAnno() override CLICK_COLD;

    const char *class_name() const override { return "GPS_TEST_PrintAnno"; }

    const char *port_count() const override { return "1/1"; }

    const char *processing() const override { return AGNOSTIC; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    Packet *simple_action(Packet *p_in) override;


private:
    String _label;

};

CLICK_ENDDECLS

#endif //CLICK_GPS_TEST_PRINTANNO_HH
