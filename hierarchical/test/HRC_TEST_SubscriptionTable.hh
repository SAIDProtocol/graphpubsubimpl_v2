//
// Created by jiachen on 10/3/18.
//

#ifndef CLICK_HRC_TEST_SUBSCRIPTIONTABLE_HH
#define CLICK_HRC_TEST_SUBSCRIPTIONTABLE_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include "../elements/HRC_SubscriptionTable.hh"


class HRC_TEST_SubscriptionTable : public Element {
public:
    HRC_TEST_SubscriptionTable() CLICK_COLD;

    ~HRC_TEST_SubscriptionTable() override CLICK_COLD;

    const char *class_name() const override { return "HRC_TEST_SubscriptionTable"; };

    const char *port_count() const override { return PORTS_0_0; };

    const char *processing() const override { return AGNOSTIC; };

    int initialize(ErrorHandler *errh) override CLICK_COLD;

    void add_handlers() override CLICK_COLD;

    void insert(const String &param, ErrorHandler *errh);

    void remove(const String &param, ErrorHandler *errh);

    void lookup(const String &param, ErrorHandler *errh);

    void exactMatch(const String &param, ErrorHandler *errh);

    void print(ErrorHandler *errh);

private:
    HRC_SubscriptionTable<int> _table;
    enum {
        h_lookup, h_exact, h_print, h_insert, h_remove
    };
    String _lookup, _exactMatch, _insert, _remove;

    void insert(const char *name, int val, ErrorHandler *errh, bool printTable);

    void remove(const char *name, ErrorHandler *errh, bool printTable);

    void remove(const char *name, int val, ErrorHandler *errh, bool printTable);


    static int change_param(const String &, Element *, void *, ErrorHandler *);
};


#endif //CLICK_HRC_TEST_SUBSCRIPTIONTABLE_HH
