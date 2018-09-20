//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_GPS_ARPTABLE_HH
#define CLICK_GPS_ARPTABLE_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <click/etheraddress.hh>
#include <click/ipaddress.hh>
#include <unordered_map>
#include "GPS_NA.hh"
#include "GPS_ReadWriteLock.hh"

CLICK_DECLS

class GPS_NAAddress {
public:
    GPS_NAAddress() : _port(0) {}

    GPS_NAAddress(uint32_t port, EtherAddress ether, IPAddress ip) : _port(port), _ether(ether), _ip(ip) {}

    ~GPS_NAAddress() = default;

    const EtherAddress &getEther() const { return _ether; }

    const IPAddress &getIp() const { return _ip; }

    const uint32_t &getPort() const { return _port; }

private:
    uint32_t _port;
    EtherAddress _ether;
    IPAddress _ip;
};

class GPS_ARPTable : public Element {
public:
    static const int IN_PORT_DATA = 0;
    static const int IN_PORT_LSA = 1;
    static const int OUT_PORT_DATA = 0;
    static const int OUT_PORT_DISCARD = 1;

    GPS_ARPTable() CLICK_COLD;

    ~GPS_ARPTable() override CLICK_COLD;

    const char *class_name() const override { return "GPS_ARPTable"; };

    const char *port_count() const override { return "-2/-2"; };

    const char *processing() const override { return PUSH; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    void push(int port, Packet *p) override;


private:
    std::unordered_map<gps_na_t, GPS_NAAddress> _naAddresses;
    GPS_ReadWriteLock _lock;

    int parseArgFile(const String &fileName, ErrorHandler *errh);

    void handleData(Packet *p);

    void handleLSA(Packet *p);
};

CLICK_ENDDECLS

#endif //CLICK_GPS_ARPTABLE_HH
