//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_HRC_ARPTABLE_HH
#define CLICK_HRC_ARPTABLE_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <click/etheraddress.hh>
#include <click/ipaddress.hh>
#include <unordered_map>
#include "HRC_NA.hh"
#include "HRC_ReadWriteLock.hh"

CLICK_DECLS

class HRC_NAAddress {
public:
    HRC_NAAddress() : _port(0) {}

    HRC_NAAddress(uint32_t port, EtherAddress ether, IPAddress ip) : _port(port), _ether(ether), _ip(ip) {}

    ~HRC_NAAddress() = default;

    const EtherAddress &getEther() const { return _ether; }

    const IPAddress &getIp() const { return _ip; }

    const uint32_t &getPort() const { return _port; }

private:
    uint32_t _port;
    EtherAddress _ether;
    IPAddress _ip;
};

class HRC_ARPTable : public Element {
public:
    static const int IN_PORT_DATA = 0;
    static const int IN_PORT_LSA = 1;
    static const int OUT_PORT_DATA = 0;
    static const int OUT_PORT_DISCARD = 1;

    static int
    parseArgFile(const String &fileName, ErrorHandler *errh, std::unordered_map<hrc_na_t, HRC_NAAddress> &naAddresses);

    HRC_ARPTable() CLICK_COLD;

    ~HRC_ARPTable() override

    CLICK_COLD;

    const char *class_name() const override { return "HRC_ARPTable"; };

    const char *port_count() const override { return "-2/-2"; };

    const char *processing() const override { return PUSH; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override

    CLICK_COLD;

    void push(int port, Packet *p) override;


private:
    std::unordered_map<hrc_na_t, HRC_NAAddress> _naAddresses;
    HRC_ReadWriteLock _lock;

    void handleData(Packet *p);

    void handleLSA(Packet *p);
};

CLICK_ENDDECLS

#endif //CLICK_HRC_ARPTABLE_HH

