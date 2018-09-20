//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_GPS_SUBSCRIPTIONTABLE_HH
#define CLICK_GPS_SUBSCRIPTIONTABLE_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <unordered_map>
#include <unordered_set>
#include "GPS_ReadWriteLock.hh"
#include "GPS_GUID.hh"
#include "GPS_NA.hh"

CLICK_DECLS

class GPS_SubscriptionTable : public Element {
public:
    static const int IN_PORT_PUBLICATION = 0;
    static const int IN_PORT_SUBSCRIPTION = 1;
    static const int OUT_PORT_PUBLICATION = 0;
    static const int OUT_PORT_SUBSCRIPTION = 1;
    static const int OUT_PORT_PUBLICATION_DISCARD = 2;

    GPS_SubscriptionTable() CLICK_COLD;

    ~GPS_SubscriptionTable() override CLICK_COLD;

    const char *class_name() const override { return "GPS_SubscriptionTable"; }

    const char *port_count() const override { return "-2/-3"; }

    const char *processing() const override { return PUSH; };


    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void push(int port, Packet *p) override;

private:
    std::unordered_map<gps_guid_t, std::unordered_set<gps_na_t>> _subscriptions;

    GPS_ReadWriteLock _lock;

    int parseArgFile(const String &fileName, ErrorHandler *errh);

    void handlePublication(Packet *packet);

    void handleSubscription(Packet *packet);
};

CLICK_ENDDECLS

#endif //CLICK_GPS_SUBSCRIPTIONTABLE_HH
