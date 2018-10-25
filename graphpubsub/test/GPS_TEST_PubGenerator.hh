//
// Created by jiachen on 10/17/18.
//

#ifndef CLICK_GPS_TEST_PUBGENERATOR_HH
#define CLICK_GPS_TEST_PUBGENERATOR_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <click/timer.hh>
#include <vector>
#include <random>
#include "../elements/GPS_GUID.hh"
#include "../elements/GPS_NA.hh"

CLICK_DECLS

class GPS_TEST_PubGenerator : public Element {
public:
    GPS_TEST_PubGenerator() CLICK_COLD;

    ~GPS_TEST_PubGenerator() override CLICK_COLD;

    const char *class_name() const override { return "GPS_TEST_PubGenerator"; }

    const char *port_count() const override { return "0/1"; }

    const char *processing() const override { return PULL; }

    int initialize(ErrorHandler *errh) override CLICK_COLD;

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void add_handlers() override CLICK_COLD;

    void run_timer(Timer *timer) override;

    Packet *pull(int port) override;

private:
    std::vector<gps_guid_t> _dstGuids;
    std::vector<gps_na_t> _dstNas;
    std::default_random_engine _rand;
    Packet *_packet;
    uint32_t _pktCnt;
    Timer _timer;
    Timestamp _duration;

    Packet *(GPS_TEST_PubGenerator::*_generatePacket)();

    Packet *generateNaPacket();

    Packet *generateGuidPacket();

    WritablePacket *setCount(WritablePacket *p);

    static int getPacketCountHandler(const String &s, Element *e, void *vparam, ErrorHandler *);
};

CLICK_ENDDECLS

#endif //CLICK_GPS_TEST_PUBGENERATOR_HH
