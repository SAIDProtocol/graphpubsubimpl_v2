//
// Created by ubuntu on 10/4/18.
//

#include "HRC_EtherAnnotator.hh"
#include "HRC_PacketAnno.hh"
#include "HRC_Helper.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ether.h>

CLICK_DECLS

HRC_EtherAnnotator::HRC_EtherAnnotator() = default;

HRC_EtherAnnotator::~HRC_EtherAnnotator() = default;

Packet *HRC_EtherAnnotator::simple_action(Packet *p) {
    auto etherHeader = p->ether_header();
    copyEther(HRC_ANNO_PREV_HOP_ETHER(p), etherHeader->ether_shost);
    *HRC_ANNO_PREV_HOP_PORT(p) = _myPort;
    return p;
}

int HRC_EtherAnnotator::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh)
                .read_mp("PORT", _myPort)
                .complete() < 0) {
        return -1;
    }
    errh->debug("port=0x%02x", _myPort);
    return 0;
}


CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_EtherAnnotator)