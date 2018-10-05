//
// Created by jiachen on 10/5/18.
//

#ifndef CLICK_HRC_SUBSCRIPTIONTABLE_HH
#define CLICK_HRC_SUBSCRIPTIONTABLE_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include "HRC_MulticastTable.hh"
#include "HRC_NA.hh"
#include "HRC_ReadWriteLock.hh"

CLICK_DECLS

class HRC_SubscriptionTable : public Element {
public:
    static const int IN_PORT_PUBLICATION = 0;
    static const int IN_PORT_SUBSCRIPTION = 1;
    static const int OUT_PORT_PUBLICATION = 0;
    static const int OUT_PORT_SUBSCRIPTION = 1;
    static const int OUT_PORT_DISCARD = 2;

    static int parseArgFile(const String &fileName, ErrorHandler *errh, HRC_MulticastTable<hrc_na_t> &table);

    HRC_SubscriptionTable() CLICK_COLD;

    ~HRC_SubscriptionTable() override CLICK_COLD;

    const char *class_name() const override { return "HRC_SubscriptionTable"; };

    const char *port_count() const override { return "-2/-3"; };

    const char *processing() const override { return PUSH; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void push(int port, Packet *p) override;

private:
    HRC_MulticastTable<hrc_na_t> _table;
    HRC_ReadWriteLock _lock;

    void handlePublication(Packet *packet);

    void handleSubscription(Packet *packet);
};

CLICK_ENDDECLS

#endif //CLICK_HRC_SUBSCRIPTIONTABLE_HH
