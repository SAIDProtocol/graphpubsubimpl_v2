//
// Created by jiachen on 10/3/18.
//

#include "HRC_TEST_DataWrapper.hh"
#include "../elements/HRC_PacketHeader.hh"
#include "../elements/HRC_Helper.hh"
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
        case HRC_PACKET_TYPE_SUBSCRIPTION:
            return hrc_packet_subscription_calculate_header_size(name);
        default:
            return 0;
    }
}

HRC_TEST_DataWrapper::HRC_TEST_DataWrapper()
        : _isFile(false),
          _type(HRC_PACKET_TYPE_INTEREST),
          _headerSize(getPacketHeaderSize(HRC_PACKET_TYPE_INTEREST, "")),
          _rand(0) {};

HRC_TEST_DataWrapper::~HRC_TEST_DataWrapper() = default;

Packet *HRC_TEST_DataWrapper::simple_action(Packet *p) {
    auto payloadSize = p->length();

    const char *name;
    size_t headerSize;
    if (_isFile) {
        name = _names[_rand() % _names.size()].c_str();
        headerSize = getPacketHeaderSize(_type, name);
    } else {
        name = _name.c_str();
        headerSize = _headerSize;
    }
    ErrorHandler::default_handler()->debug(R"(>>> "%s")", name);
    auto wp = p->push(static_cast<uint32_t >(headerSize));
    auto header = wp->data();
    switch (_type) {
        case HRC_PACKET_TYPE_INTEREST:
            hrc_packet_interest_init(header, name, payloadSize);
            break;
        case HRC_PACKET_TYPE_PUBLICATION:
            hrc_packet_publication_init(header, name, payloadSize);
            break;
        case HRC_PACKET_TYPE_SUBSCRIPTION:
            wp->take(payloadSize);
            hrc_packet_subscription_init(header, name, payloadSize >= 2);
            break;
        default:
            ErrorHandler::default_handler()->fatal("Should not reach here! Unknown packet type 0x%02x", _type);
            break;
    }
    return wp;
}

int HRC_TEST_DataWrapper::configure(Vector<String> &conf, ErrorHandler *errh) {
    String type, name;
    bool isFile;
    if (Args(conf, this, errh)
                .read_mp("TYPE", type)
                .read_mp("NAME", name)
                .read_p("ISFILE", isFile)
                .complete() < 0) {
        return -1;
    }
    setType(type, errh);
    if (setName(name, isFile, errh) < 0) return -1;
    errh->debug("type=%02x | name=%s | headerSize=%d", _type, _name.c_str(), _headerSize);
    return 0;
}

int HRC_TEST_DataWrapper::setName(const String &name, bool isFile, ErrorHandler *errh) {
    if (isFile) {
        if (parseFile(name, _names, errh) < 0) return -1;
        _name = name;
        _isFile = isFile;
        for (auto &tmp : _names) errh->debug(R"(  "%s")", tmp.c_str());
    } else {
        _name = name;
        _isFile = isFile;
        _headerSize = getPacketHeaderSize(_type, _name.c_str());
        errh->debug(R"(name="%s", headerSize=%d)", _name.c_str(), _headerSize);
    }
    return 0;
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
    } else if (_strType == "SUBSCRIPTION" || _strType == "0X84" || _strType == "132") {
        _type = HRC_PACKET_TYPE_SUBSCRIPTION;
        errh->debug("Setting as SUBSCRIPTION (0x84)");
    } else {
        _type = HRC_PACKET_TYPE_INTEREST;
        errh->debug("Incorrect packet type %s, setting as INTEREST (0x81)", _strType.c_str());
    }
    _headerSize = getPacketHeaderSize(_type, _name.c_str());
    _strType = type;
}

int HRC_TEST_DataWrapper::parseFile(const String &fileName, std::vector<std::string> &names, ErrorHandler *errh) {
    auto fp = fopen(fileName.c_str(), "r");
    if (fp == nullptr) {
        errh->error("Cannot read file %s", fileName.c_str());
        return -1;
    }
    char *line = nullptr, *tmp;
    size_t len = 0, lineNum = 0;
    ssize_t pos;
    const char *name;
    names.clear();

    while (getline(&line, &len, fp) != -1) {
        lineNum++;
        tmp = line;
        pos = skipEmpty(tmp);

        //empty line
        if (pos == -1) continue;
        tmp += pos;
        // comment skip
        if (*tmp == '#') continue;

        // name
        pos = getPart(tmp);
        name = tmp;
        names.emplace_back(name);
        (void) pos;
    }
    return 0;
}

int HRC_TEST_DataWrapper::change_param(const String &str, Element *e, void *vparam, ErrorHandler *errh) {
    (void) errh;
    auto elem = dynamic_cast<HRC_TEST_DataWrapper *>(e);
    switch (reinterpret_cast<intptr_t >(vparam)) {
        case h_name:
            if (elem->setName(str, elem->_isFile, ErrorHandler::default_handler()) < 0) return -1;
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