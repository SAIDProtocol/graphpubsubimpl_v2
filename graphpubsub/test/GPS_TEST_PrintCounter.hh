//
// Created by root on 10/23/18.
//

#ifndef CLICK_GPS_TEST_PRINTCOUNTER_HH
#define CLICK_GPS_TEST_PRINTCOUNTER_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class GPS_TEST_PrintCounter : public Element {
public:
    GPS_TEST_PrintCounter() CLICK_COLD;

    ~GPS_TEST_PrintCounter() override CLICK_COLD;

    const char *class_name() const override { return "GPS_TEST_PrintCounter"; }

    const char *port_count() const override { return PORTS_0_0; }

    const char *processing() const override { return AGNOSTIC; }

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void cleanup(CleanupStage stage) override CLICK_COLD;

    int initialize(ErrorHandler *errh) override;

private:
    Vector<Element *> _elements;
    Timestamp _start;

};

CLICK_ENDDECLS

#endif //CLICK_GPS_TEST_PRINTCOUNTER_HH
