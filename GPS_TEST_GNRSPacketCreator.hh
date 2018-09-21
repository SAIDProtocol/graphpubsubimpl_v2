//
// Created by ubuntu on 9/21/18.
//

#ifndef CLICK_GPS_TEST_GNRSPACKETCREATOR_HH
#define CLICK_GPS_TEST_GNRSPACKETCREATOR_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

class GPS_TEST_AbstractGNRSPacketCreator {
public:
    virtual ~GPS_TEST_AbstractGNRSPacketCreator() = default;

    virtual Packet *getPacket() = 0;
};


class GPS_TEST_GNRSPacketCreator : public Element {
public:
    GPS_TEST_GNRSPacketCreator() CLICK_COLD;

    ~GPS_TEST_GNRSPacketCreator() override CLICK_COLD;


    const char *class_name() const override { return "GPS_TEST_GNRSPacketCreator"; }

    const char *port_count() const override { return PORTS_1_1; };

    const char *processing() const override { return AGNOSTIC; };


    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    Packet *simple_action(Packet *p) override;


private:
    GPS_TEST_AbstractGNRSPacketCreator *_creator;

};


#endif //CLICK_GPS_TEST_GNRSPACKETCREATOR_HH
