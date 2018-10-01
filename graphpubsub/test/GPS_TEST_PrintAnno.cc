//
// Created by jiachen on 9/20/18.
//

#include "GPS_TEST_PrintAnno.hh"
#include "../elements/GPS_NA.hh"
#include "../elements/GPS_PacketAnno.h"
#include <click/args.hh>
#include <click/etheraddress.hh>
#include <click/ipaddress.hh>

CLICK_DECLS

GPS_TEST_PrintAnno::GPS_TEST_PrintAnno() = default;

GPS_TEST_PrintAnno::~GPS_TEST_PrintAnno() = default;

int GPS_TEST_PrintAnno::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh)
                .read_p("LABEL", _label)
                .complete() < 0) {
        return -1;
    }
    return 0;
}

Packet *GPS_TEST_PrintAnno::simple_action(Packet *p_in) {
    click_chatter(
            "%s: srcIp=%s | srcEther=%s | srcPort=%d |  nextHopNa=%s | nextHopIp=%s | nextHopEther=%s | nextHopPort=%d | prio=%d",
            _label.c_str(),
            IPAddress(GPS_ANNO_SRC_IP(p_in)).unparse().c_str(),
            EtherAddress(GPS_ANNO_SRC_ETHER(p_in)).unparse_colon().c_str(),
            *GPS_ANNO_SRC_PORT(p_in),
            gps_na_unparse(GPS_ANNO_NEXT_HOP_NA(p_in)).c_str(),
            IPAddress(GPS_ANNO_NEXT_HOP_IP(p_in)).unparse().c_str(),
            EtherAddress(GPS_ANNO_NEXT_HOP_ETHER(p_in)).unparse_colon().c_str(),
            *GPS_ANNO_NEXT_HOP_PORT(p_in),
            *GPS_ANNO_PRIO(p_in));
    return p_in;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_TEST_PrintAnno)
