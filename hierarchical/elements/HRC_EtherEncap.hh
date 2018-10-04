//
// Created by ubuntu on 10/4/18.
//

#ifndef CLICK_HRC_ETHERENCAP_HH
#define CLICK_HRC_ETHERENCAP_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/etheraddress.hh>
#include <click/glue.hh>

CLICK_DECLS

class HRC_EtherEncap : public Element {
public:
    static inline uint16_t HRC_DEFAULT_ETHER_TYPE() {
        return htons(0x27c1);
    };

    HRC_EtherEncap() CLICK_COLD;

    ~HRC_EtherEncap() override CLICK_COLD;

    const char *class_name() const override { return "HRC_EtherEncap"; }

    const char *port_count() const override { return "-1/-1"; }

    const char *processing() const override { return AGNOSTIC; }

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    Packet *simple_action(Packet *p) override;

private:
    EtherAddress _myEther;
};

CLICK_ENDDECLS


#endif //CLICK_HRC_ETHERENCAP_HH
