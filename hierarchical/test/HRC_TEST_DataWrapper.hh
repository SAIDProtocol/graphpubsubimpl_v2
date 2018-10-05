//
// Created by jiachen on 10/3/18.
//

#ifndef CLICK_HRC_TEST_DATAWRAPPER_HH
#define CLICK_HRC_TEST_DATAWRAPPER_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class HRC_TEST_DataWrapper : public Element {
public:
    HRC_TEST_DataWrapper() CLICK_COLD;

    ~HRC_TEST_DataWrapper() override CLICK_COLD;

    const char *class_name() const override { return "HRC_TEST_DataWrapper"; }

    const char *port_count() const override { return PORTS_1_1; };

    const char *processing() const override { return AGNOSTIC; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void add_handlers() override CLICK_COLD;

    Packet *simple_action(Packet *p) override;

    void setName(const String &name, ErrorHandler *errh);

    void setType(const String &type, ErrorHandler *errh);

private:
    enum {
        h_name, h_type
    };
    String _name, _strType;
    uint8_t _type;
    size_t _headerSize;

    static int change_param(const String &, Element *, void *, ErrorHandler *);
};


CLICK_ENDDECLS
#endif //CLICK_HRC_TEST_DATAWRAPPER_HH
