//
// Created by jiachen on 10/3/18.
//

#include "HRC_TEST_InterestWrapper.hh"
#include "../elements/HRC_PacketHeader.hh"
#include <click/args.hh>
#include <click/error.hh>

CLICK_DECLS

HRC_TEST_InterestWrapper::HRC_TEST_InterestWrapper() : _headerSize(hrc_packet_interest_get_header_size("")) {
};

HRC_TEST_InterestWrapper::~HRC_TEST_InterestWrapper() = default;

Packet *HRC_TEST_InterestWrapper::simple_action(Packet *p) {
    auto payloadSize = p->length();
    auto wp = p->push(_headerSize);
    auto header = wp->data();
    hrc_packet_interest_init(header, _name.c_str(), payloadSize);
    return wp;
}

int HRC_TEST_InterestWrapper::configure(Vector<String> &conf, ErrorHandler *errh) {
    String name;
    if (Args(conf, this, errh)
                .read_mp("NAME", name)
                .complete() < 0) {
        return -1;
    }
    setName(name, errh);
    return 0;
}

void HRC_TEST_InterestWrapper::setName(const String &name, ErrorHandler *errh = nullptr) {
    _name = name;
    _headerSize = hrc_packet_interest_get_header_size(_name.c_str());
    if (errh)errh->debug(R"(name="%s", headerSize=%d)", _name.c_str(), _headerSize);
    else click_chatter(R"(name="%s", headerSize=%d)", _name.c_str(), _headerSize);
}

int HRC_TEST_InterestWrapper::change_param(const String &str, Element *e, void *vparam, ErrorHandler *errh) {
    (void) errh;
    auto elem = dynamic_cast<HRC_TEST_InterestWrapper *>(e);
    switch (reinterpret_cast<intptr_t >(vparam)) {
        case h_name:
            elem->setName(str, nullptr);
            break;
        default:
            break;
    }
    return 0;
}

void HRC_TEST_InterestWrapper::add_handlers() {
    add_data_handlers("content_name", Handler::f_read | Handler::f_write, &_name);
    add_write_handler("content_name", change_param, h_name, Handler::f_raw);
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_TEST_InterestWrapper)