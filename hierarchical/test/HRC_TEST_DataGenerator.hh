//
// Created by jiachen on 10/18/18.
//

#ifndef CLICK_HRC_TEST_DATAGENERATOR_HH
#define CLICK_HRC_TEST_DATAGENERATOR_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <click/timer.hh>
#include <random>
#include <vector>

CLICK_DECLS

class HRC_TEST_DataGenerator : public Element {
public:
    HRC_TEST_DataGenerator() CLICK_COLD;

    ~HRC_TEST_DataGenerator() override CLICK_COLD;

    const char *class_name() const override { return "HRC_TEST_DataGenerator"; }

    const char *port_count() const override { return "0/1"; }

    const char *processing() const override { return PULL; }

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    int initialize(ErrorHandler *errh) override CLICK_COLD;

    Packet *pull(int port) override;

    void run_timer(Timer *timer) override;
private:
    uint32_t _pktCnt, _size;
    uint8_t _type;
    std::vector<std::string> _names;
    std::default_random_engine _rand;
    Timestamp _duration;
    Timer _timer;

    Packet *(HRC_TEST_DataGenerator::*_generatePacket)();

    Packet *generateInterest();

    Packet *generatePublication();

    Packet *generateSubscription();

    WritablePacket *setCount(WritablePacket *p, size_t start);
};

CLICK_ENDDECLS

#endif //CLICK_HRC_TEST_DATAGENERATOR_HH
