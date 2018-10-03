//
// Created by jiachen on 10/3/18.
//

#include "HRC_PrioAnnotator.hh"
#include "HRC_PacketAnno.hh"
#include "HRC_PacketHeader.hh"

CLICK_DECLS

HRC_PrioAnnotator::HRC_PrioAnnotator() =default;

HRC_PrioAnnotator::~HRC_PrioAnnotator() =default;

Packet *HRC_PrioAnnotator::simple_action(Packet *p) {
    auto prio = HRC_ANNO_PRIO(p);
    auto type = hrc_packet_get_type(p->data());

    switch(type) {
        case HRC_PACKET_TYPE_INTEREST:
            *prio = HRC_PACKET_PRIO_INTEREST;
            break;
        case HRC_PACKET_TYPE_SUBSCRIPTION:
            *prio = HRC_PACKET_PRIO_SUBSCRIPTION;
            break;
        case HRC_PACKET_TYPE_PUBLICATION:
            *prio = HRC_PACKET_PRIO_PUBLICATION;
            break;
        default:
            *prio = HRC_PACKET_PRIO_UNDEFINED;
            break;
    }
    //clear dest na
    *HRC_ANNO_DST_NAME(p) = nullptr;

    return p;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_PrioAnnotator)