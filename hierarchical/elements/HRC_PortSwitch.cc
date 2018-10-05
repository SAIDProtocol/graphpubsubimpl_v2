//
// Created by ubuntu on 10/4/18.
//

#include "HRC_PortSwitch.hh"
#include "HRC_PacketAnno.hh"

CLICK_DECLS

HRC_PortSwitch::HRC_PortSwitch() = default;

HRC_PortSwitch::~HRC_PortSwitch() = default;

void HRC_PortSwitch::push(int, Packet *p) {
    auto nextHopPort = HRC_ANNO_NEXT_HOP_PORT(p);
    checked_output_push(*nextHopPort, p);
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_PortSwitch)