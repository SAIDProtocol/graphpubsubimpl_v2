//
// Created by jiachen on 10/6/18.
//

#ifndef CLICK_HRC_PUBLICATIONENCAPSULATOR_HH
#define CLICK_HRC_PUBLICATIONENCAPSULATOR_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include "HRC_ContentName.hh"
#include "HRC_InterestTable.hh"
#include "HRC_ReadWriteLock.hh"

CLICK_DECLS

class HRC_PublicationEncapsulator : public Element {
public:
//    static const int IN_PORT_PUBLICATION = 0;
    static const int OUT_PORT_UNICAST = 0;
    static const int OUT_PORT_DISCARD = 1;

    HRC_PublicationEncapsulator() CLICK_COLD;

    ~HRC_PublicationEncapsulator() override CLICK_COLD;

    const char *class_name() const override { return "HRC_PublicationEncapsulator"; }

    const char *port_count() const override { return "-1/-2"; }

    const char *processing() const override { return PUSH; }

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void push(int port, Packet *p) override;

private:
    HRC_InterestTable<const char *> _table;
    HRC_ReadWriteLock _lock;

};

CLICK_ENDDECLS

#endif //CLICK_HRC_PUBLICATIONENCAPSULATOR_HH
