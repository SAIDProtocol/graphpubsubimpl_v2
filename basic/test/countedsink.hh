//
// Created by root on 1/17/19.
//

#ifndef CLICK_BASIC_COUNTEDSINK_HH
#define CLICK_BASIC_COUNTEDSINK_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <ctime>

CLICK_DECLS

#ifndef LISTEN_DURATION
#define LISTEN_DURATION (65536)
#endif

class CountedSink : public Element {
public:
    CountedSink() CLICK_COLD;

    ~CountedSink() override CLICK_COLD;

    const char *class_name() const override { return "BASIC_CountedSink"; }

    const char *port_count() const override { return PORTS_1_0; }

    const char *processing() const override { return PUSH; };

    void push(int port, Packet *p) override;

    void cleanup(CleanupStage stage) override;

private:
    uint32_t _counts[LISTEN_DURATION];
    struct timeval _start;
};

CLICK_ENDDECLS
#endif //CLICK_BASIC_COUNTEDSINK_HH
