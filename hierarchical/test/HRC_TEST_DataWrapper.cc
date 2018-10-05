//
// Created by jiachen on 10/3/18.
//

#include "HRC_TEST_DataWrapper.hh"
#include "../elements/HRC_PacketHeader.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <cstring>

CLICK_DECLS
static size_t getPacketHeaderSize(uint8_t type, const char *name) {
    switch (type) {
        case HRC_PACKET_TYPE_INTEREST:
            return hrc_packet_interest_calculate_header_size(name);
        case HRC_PACKET_TYPE_PUBLICATION:
            return hrc_packet_publication_calculate_header_size(name);
        default:
            return 0;
    }
}

HRC_TEST_DataWrapper::HRC_TEST_DataWrapper()
        : _type(HRC_PACKET_TYPE_INTEREST), _headerSize(getPacketHeaderSize(HRC_PACKET_TYPE_INTEREST, "")) {};

HRC_TEST_DataWrapper::~HRC_TEST_DataWrapper() = default;

Packet *HRC_TEST_DataWrapper::simple_action(Packet *p) {
    auto payloadSize = p->length();

    auto wp = p->push(static_cast<uint32_t >(_headerSize));
    auto header = wp->data();
    switch (_type) {
        case HRC_PACKET_TYPE_INTEREST:
            hrc_packet_interest_init(header, _name.c_str(), payloadSize);
            break;
        case HRC_PACKET_TYPE_PUBLICATION:
            hrc_packet_publication_init(header, _name.c_str(), payloadSize);
            break;
        default:
            ErrorHandler::default_handler()->fatal("Should not reach here! Unknown packet type 0x%02x", _type);
            break;
    }
    return wp;
}

int HRC_TEST_DataWrapper::configure(Vector<String> &conf, ErrorHandler *errh) {
    String type, name, test;
    if (Args(conf, this, errh)
                .read_mp("TYPE", type)
                .read_mp("NAME", name)
                .complete() < 0) {
        return -1;
    }
    click_chatter(test.c_str());
    click_chatter(name.c_str());
    setType(type, errh);
    setName(name, errh);
    return 0;
}

void HRC_TEST_DataWrapper::setName(const String &name, ErrorHandler *errh) {
    _name = name;
    _headerSize = getPacketHeaderSize(_type,_name.c_str());
    errh->debug(R"(name="%s", headerSize=%d)", _name.c_str(), _headerSize);
}

void HRC_TEST_DataWrapper::setType(const String &type, ErrorHandler *errh) {
    _strType = type;
    _strType.upper();
    if (_strType == "INTEREST" || _strType == "0X81" || _strType == "129") {
        _type = HRC_PACKET_TYPE_INTEREST;
        errh->debug("Setting as INTEREST (0x81)");
    } else if (_strType == "PUBLICATION" || _strType == "0X83" || _strType == "131") {
        _type = HRC_PACKET_TYPE_PUBLICATION;
        errh->debug("Setting as PUBLICATION (0x83)");
    } else {
        _type = HRC_PACKET_TYPE_INTEREST;
        errh->debug("Incorrect packet type %s, setting as INTEREST (0x81)", _strType.c_str());
    }
    _headerSize = getPacketHeaderSize(_type,_name.c_str());
    _strType = type;
}

int HRC_TEST_DataWrapper::change_param(const String &str, Element *e, void *vparam, ErrorHandler *errh) {
    (void) errh;
    auto elem = dynamic_cast<HRC_TEST_DataWrapper *>(e);
    switch (reinterpret_cast<intptr_t >(vparam)) {
        case h_name:
            elem->setName(str, ErrorHandler::default_handler());
            break;
        case h_type:
            elem->setType(str, ErrorHandler::default_handler());
            break;
        default:
            break;
    }
    return 0;
}

void HRC_TEST_DataWrapper::add_handlers() {
    add_data_handlers("content_name", Handler::f_read | Handler::f_write, &_name);
    add_write_handler("content_name", change_param, h_name, Handler::f_raw);
    add_data_handlers("packet_type", Handler::f_read | Handler::f_write, &_strType);
    add_write_handler("packet_type", change_param, h_type, Handler::f_raw);
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_TEST_DataWrapper)