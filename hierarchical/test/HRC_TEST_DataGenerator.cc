//
// Created by jiachen on 10/18/18.
//

#include "HRC_TEST_DataGenerator.hh"
#include "HRC_TEST_DataWrapper.hh"
#include "../elements/HRC_PacketHeader.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/router.hh>

CLICK_DECLS

HRC_TEST_DataGenerator::HRC_TEST_DataGenerator() :
        _pktCnt(0), _size(0), _type(HRC_PACKET_TYPE_INTEREST), _rand(0), _timer(this),
        _generatePacket(&HRC_TEST_DataGenerator::generateInterest) {

}

HRC_TEST_DataGenerator::~HRC_TEST_DataGenerator() = default;

Packet *HRC_TEST_DataGenerator::pull(int) {
    return (this->*_generatePacket)();
}

void HRC_TEST_DataGenerator::run_timer(Timer *) {
    router()->please_stop_driver();
}

int HRC_TEST_DataGenerator::configure(Vector<String> &conf, ErrorHandler *errh) {
    String type, fileName;

    if (Args(conf, this, errh)
                .read_mp("TYPE", type)
                .read_mp("NAME_FILE", FilenameArg(), fileName)
                .read_mp("SIZE", _size)
                .read_p("DURATION", _duration)
                .complete() < 0) {
        return -1;
    }
    String tmpType = type.upper();
    click_chatter("%s", tmpType.c_str());
    std::vector<std::string> names;
    if (HRC_TEST_DataWrapper::parseFile(fileName, names, errh) < 0) return -1;


    if (tmpType == "INTEREST" || tmpType == "0X81" || tmpType == "129") {
        _type = HRC_PACKET_TYPE_INTEREST;
        _generatePacket = &HRC_TEST_DataGenerator::generateInterest;
        for (auto &tmp : names) {
            auto size = hrc_packet_interest_calculate_header_size(tmp.length()) + sizeof(uint32_t);
            if (size > _size)
                errh->debug(R"( Name too long for size (%d>%d): "%s")", size, _size, tmp.c_str());
            else
                _names.push_back(tmp);
        }
        errh->debug("Setting as INTEREST (0x81), names: %d", _names.size());
    } else if (tmpType == "PUBLICATION" || tmpType == "0X83" || tmpType == "131") {
        _type = HRC_PACKET_TYPE_PUBLICATION;
        _generatePacket = &HRC_TEST_DataGenerator::generatePublication;
        for (auto &tmp : names) {
            auto size = hrc_packet_publication_calculate_header_size(tmp.length()) + sizeof(uint32_t);
            if (size > _size)
                errh->debug(R"( Name too long for size (%d>%d): "%s")", size, _size, tmp.c_str());
            else
                _names.push_back(tmp);
        }
        errh->debug("Setting as PUBLICATION (0x83), names: %d", _names.size());
    } else if (tmpType == "SUBSCRIPTION" || tmpType == "0X84" || tmpType == "132") {
        _type = HRC_PACKET_TYPE_SUBSCRIPTION;
        _generatePacket = &HRC_TEST_DataGenerator::generateSubscription;
        for (auto &tmp : names) {
            auto size = hrc_packet_subscription_calculate_header_size(tmp.length()) + sizeof(uint32_t);
            if (size > _size)
                errh->debug(R"( Name too long for size (%d>%d): "%s")", size, _size, tmp.c_str());
            else
                _names.push_back(tmp);
        }
        errh->debug("Setting as SUBSCRIPTION (0x84), names: %d", _names.size());
    } else {
        errh->error("Incorrect packet type %s", type.c_str());
        return -1;
    }
    if (_names.empty()) {
        errh->error("Cannot find name!");
        return -1;
    }
    return 0;
}

int HRC_TEST_DataGenerator::initialize(ErrorHandler *errh) {
    _timer.initialize(this);
    if (_duration) {
        errh->debug("Duration: %s", _duration.unparse().c_str());
        _timer.schedule_after(_duration);
    }
    return 0;
}

Packet *HRC_TEST_DataGenerator::generateInterest() {
    auto &name = _names[_rand() % _names.size()];
    auto headerSize = hrc_packet_interest_calculate_header_size(name.size()),
            bodySize = _size - headerSize;
    auto wp = Packet::make(_size);
    hrc_packet_interest_init(wp->data(), name.c_str(), static_cast<uint32_t>(bodySize));
    return setCount(wp, headerSize);
}

Packet *HRC_TEST_DataGenerator::generatePublication() {
    auto &name = _names[_rand() % _names.size()];
    auto headerSize = hrc_packet_publication_calculate_header_size(name.size()),
            bodySize = _size - headerSize;
    auto wp = Packet::make(_size);
    hrc_packet_publication_init(wp->data(), name.c_str(), static_cast<uint32_t>(bodySize));
    return setCount(wp, headerSize);
}

Packet *HRC_TEST_DataGenerator::generateSubscription() {
    auto &name = _names[_rand() % _names.size()];
    auto headerSize = hrc_packet_subscription_calculate_header_size(name.size());
    auto wp = Packet::make(static_cast<uint32_t>(headerSize));
    hrc_packet_subscription_init(wp->data(), name.c_str(), true);
    return wp;
}

WritablePacket *HRC_TEST_DataGenerator::setCount(WritablePacket *wp, size_t start) {
    *(uint32_t *) (wp->data() + start) = htonl(++_pktCnt);
    wp->timestamp_anno().assign_now();
//    ErrorHandler::default_handler()->debug("now: %s", wp->timestamp_anno().unparse().c_str());
    return wp;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_TEST_DataGenerator)