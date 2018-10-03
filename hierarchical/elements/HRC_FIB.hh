//
// Created by jiachen on 9/26/18.
//

#ifndef CLICK_HRC_FIB_HH
#define CLICK_HRC_FIB_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/error.hh>
#include <click/etheraddress.hh>
#include <click/ipaddress.hh>
#include <click/glue.hh>
#include <unordered_map>
#include "HRC_InterestTable.hh"
#include "HRC_NA.hh"
#include "HRC_ReadWriteLock.hh"

CLICK_DECLS

class HRC_FIB : public Element {
public:
    static const int IN_PORT_DATA = 0;
    static const int IN_PORT_SUBSCRIPTION = 1;
    static const int IN_PORT_ANNOUNCEMENT = 2;
    static const int OUT_PORT_DATA = 0;
    static const int OUT_PORT_SUBSCRIPTION = 1;
    static const int OUT_PORT_ANNOUNCEMENT = 2;
    static const int OUT_PORT_DISCARD = 3;
    static int parseArgFile(String &fileName, ErrorHandler *errh, HRC_InterestTable<hrc_na_t> &interestTable);

    HRC_FIB() CLICK_COLD;

    ~HRC_FIB() override CLICK_COLD;

    const char *class_name() const override { return "HRC_FIB"; };

    const char *port_count() const override { return "-3/-4"; };

    const char *processing() const override { return PUSH; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void push(int port, Packet *p) override;

private:
    HRC_InterestTable<hrc_na_t> _interestTable;
    HRC_ReadWriteLock _lock;


    void handleInterest(Packet *packet);
    void handleSubscription(Packet *packet);
    void handleAnnouncement(Packet *packet);

};


CLICK_ENDDECLS

#endif //CLICK_HRC_FIB_HH
