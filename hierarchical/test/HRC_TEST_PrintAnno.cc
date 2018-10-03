//
// Created by jiachen on 10/3/18.
//

#include "HRC_TEST_PrintAnno.hh"
#include "../elements/HRC_NA.hh"
#include "../elements/HRC_PacketAnno.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/etheraddress.hh>
#include <click/ipaddress.hh>

CLICK_DECLS

HRC_TEST_PrintAnno::HRC_TEST_PrintAnno() = default;

HRC_TEST_PrintAnno::~HRC_TEST_PrintAnno() = default;

Packet *HRC_TEST_PrintAnno::simple_action(Packet *p) {
    auto name = *HRC_ANNO_DST_NAME(p);

    click_chatter(
            "%s: srcIp=%s | srcEther=%s | srcPort=%d |  nextHopNa=%s | nextHopIp=%s | nextHopEther=%s | nextHopPort=%d | prio=%d | dstName=(%p)%s",
            _label.c_str(),
            IPAddress(HRC_ANNO_SRC_IP(p)).unparse().c_str(),
            EtherAddress(HRC_ANNO_SRC_ETHER(p)).unparse_colon().c_str(),
            *HRC_ANNO_SRC_PORT(p),
            hrc_na_unparse(HRC_ANNO_NEXT_HOP_NA(p)).c_str(),
            IPAddress(HRC_ANNO_NEXT_HOP_IP(p)).unparse().c_str(),
            EtherAddress(HRC_ANNO_NEXT_HOP_ETHER(p)).unparse_colon().c_str(),
            *HRC_ANNO_NEXT_HOP_PORT(p),
            *HRC_ANNO_PRIO(p),
            name,
            name ? name : "(nil)");
    return p;
}

int HRC_TEST_PrintAnno::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh)
                .read_mp("LABEL", _label)
                .complete() < 0)
        return -1;
    return 0;
}


CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_TEST_PrintAnno)