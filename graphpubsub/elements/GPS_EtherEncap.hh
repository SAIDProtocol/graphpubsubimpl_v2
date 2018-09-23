//
// Created by ubuntu on 9/23/18.
//

#ifndef CLICK_GPS_ETHERENCAP_HH
#define CLICK_GPS_ETHERENCAP_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/etheraddress.hh>
#include <click/glue.hh>

CLICK_DECLS

class GPS_EtherEncap : public Element {
public:
    static inline uint16_t GPS_DEFAULT_ETHER_TYPE() {
        return htons(0x27c0);
    };

    GPS_EtherEncap() CLICK_COLD;

    ~GPS_EtherEncap() override CLICK_COLD;

    const char *class_name() const override { return "GPS_EtherEncap"; };

    const char *port_count() const override { return PORTS_1_1; };

    const char *processing() const override { return AGNOSTIC; }

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    Packet *simple_action(Packet *p) override;

private:
    EtherAddress _myEther;
};

CLICK_ENDDECLS

#endif //CLICK_GPS_ETHERENCAP_HH
