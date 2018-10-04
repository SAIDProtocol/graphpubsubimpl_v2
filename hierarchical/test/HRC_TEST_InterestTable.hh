//
// Created by jiachen on 10/1/18.
//

#ifndef CLICK_HRC_TEST_INTERESTTABLE_HH
#define CLICK_HRC_TEST_INTERESTTABLE_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/error.hh>
#include <click/glue.hh>
#include "../elements/HRC_InterestTable.hh"

class HRC_TEST_InterestTable : public Element {
public:
    HRC_TEST_InterestTable() CLICK_COLD;

    ~HRC_TEST_InterestTable() override CLICK_COLD;

    const char *class_name() const override { return "HRC_TEST_InterestTable"; };

    const char *port_count() const override { return PORTS_0_0; };

    const char *processing() const override { return AGNOSTIC; };

    void add_handlers() override CLICK_COLD;

    int initialize(ErrorHandler *errh) override CLICK_COLD;


    void longestPrefixMatch(const String &param);

    void exactMatch(const String &param);

    void set(const String &param);

    void remove(const String &param);

    void print(ErrorHandler *errh);

    void print();

private:
    HRC_InterestTable<int> _table;

    enum {
        h_lpm, h_exact, h_print, h_set, h_remove
    };

    String _longestPrefixMatch, _exactMatch, _set, _remove;


    static int change_param(const String &, Element *, void *, ErrorHandler *);

};


#endif //CLICK_HRC_TEST_INTERESTTABLE_HH
