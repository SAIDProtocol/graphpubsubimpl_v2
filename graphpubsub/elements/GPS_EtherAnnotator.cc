//
// Created by ubuntu on 9/22/18.
//

#include "GPS_EtherAnnotator.hh"
#include "GPS_PacketAnno.h"
#include "GPS_Helper.hh"
#include <clicknet/ether.h>
#include <click/packet_anno.hh>

CLICK_DECLS

GPS_EtherAnnotator::GPS_EtherAnnotator() = default;


GPS_EtherAnnotator::~GPS_EtherAnnotator() = default;

Packet *GPS_EtherAnnotator::simple_action(Packet *p) {
    auto etherHeader = p->ether_header();
    copyEther(SRC_ETHER_ANNO(p), etherHeader->ether_shost);
    return p;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_EtherAnnotator)