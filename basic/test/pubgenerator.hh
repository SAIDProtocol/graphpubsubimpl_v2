//
// Created by root on 1/9/19.
//

#ifndef CLICK_BASIC_PUBGENERATOR_HH
#define CLICK_BASIC_PUBGENERATOR_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class PubGenerator : public Element {
public:
    PubGenerator() CLICK_COLD;

    ~PubGenerator() override CLICK_COLD;

    const char *class_name() const override { return "BASIC_PubGenerator"; }

    const char *port_count() const override { return "0/-1"; }

    const char *processing() const override { return PULL; }

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void add_handlers() override CLICK_COLD;

    Packet *pull(int port) override;

private:
    Packet *_pkt;
    uint64_t _pktCount;
};

CLICK_ENDDECLS

#endif //CLICK_BASIC_PUBGENERATOR_HH
