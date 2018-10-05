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

    HRC_NAAddress(uint32_t port, const EtherAddress &ether, const IPAddress &ip) :
            _port(port), _ether(ether), _ip(ip), _hash(getHash(port, ether, ip)) {}

    HRC_NAAddress() : HRC_NAAddress(0, EtherAddress(), IPAddress()) {}

    ~HRC_NAAddress() = default;

    const EtherAddress &getEther() const { return _ether; }

    const IPAddress &getIp() const { return _ip; }

    const uint32_t &getPort() const { return _port; }

    size_t getHash() const { return _hash; }

    bool operator==(const HRC_NAAddress &rhs) const {
        return _port == rhs._port &&
               _ether == rhs._ether &&
               _ip == rhs._ip;
    }

    bool operator!=(const HRC_NAAddress &rhs) const {
        return !(rhs == *this);
    }

private:
    static size_t getHash(uint32_t port, const EtherAddress &ether, const IPAddress &ip) {
        return port ^ ether.hashcode() ^ ip.hashcode();
    }

    uint32_t _port;
    EtherAddress _ether;
    IPAddress _ip;
    size_t _hash;
};

namespace std {
    template<>
    struct hash<HRC_NAAddress> {
        size_t operator()(const HRC_NAAddress &addr) const { return hash<size_t>()(addr.getHash()); }
    };
}

class HRC_ARPTable : public Element {
public:
    static const int IN_PORT_OUT_DATA = 0;
    static const int IN_PORT_IN_PACKET = 1;
    static const int OUT_PORT_OUT_DATA = 0;
    static const int OUT_PORT_IN_PACKET = 1;
    static const int OUT_PORT_DISCARD = 2;

    static int
    parseArgFile(const String &fileName, ErrorHandler *errh,
                 std::unordered_map<hrc_na_t, HRC_NAAddress> &naAddresses,
                 std::unordered_map<HRC_NAAddress, hrc_na_t> &inverseNaAddresses);

    HRC_ARPTable() CLICK_COLD;

    ~HRC_ARPTable() override

    CLICK_COLD;

    const char *class_name() const override { return "HRC_ARPTable"; };

    const char *port_count() const override { return "-2/-3"; };

    const char *processing() const override { return PUSH; };

    int configure(Vector<String> &conf, ErrorHandler *errh) override

    CLICK_COLD;

    void push(int port, Packet *p) override;


private:
    static void addNAAddress(const hrc_na_t &na, const HRC_NAAddress &addr,
                             std::unordered_map<hrc_na_t, HRC_NAAddress> &naAddresses,
                             std::unordered_map<HRC_NAAddress, hrc_na_t> &inverseNaAddresses);

    std::unordered_map<hrc_na_t, HRC_NAAddress> _naAddresses;
    std::unordered_map<HRC_NAAddress, hrc_na_t> _inverseNaAddresses;
    HRC_ReadWriteLock _lock;

    void handleOutData(Packet *p);

    void handleInPacket(Packet *p);
};

CLICK_ENDDECLS

#endif //CLICK_HRC_ARPTABLE_HH

