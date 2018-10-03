//
// Created by ubuntu on 9/23/18.
//

#include "GPS_PrioAnnotator.hh"
#include "GPS_PacketAnno.h"
#include "GPS_PacketHeader.hh"

CLICK_DECLS

GPS_PrioAnnotator::GPS_PrioAnnotator() = default;

GPS_PrioAnnotator::~GPS_PrioAnnotator() = default;

Packet *GPS_PrioAnnotator::simple_action(Packet *p) {
    auto prio = GPS_ANNO_PRIO(p);
    auto type = gps_packet_get_type(p->data());

    switch (type) {
        case GPS_PACKET_TYPE_LSA:
            *prio = GPS_PACKET_PRIO_LSA;
            break;
        case GPS_PACKET_TYPE_PUBLICATION:
            *prio = GPS_PACKET_PRIO_PUBLICATION;
            break;
        case GPS_PACKET_TYPE_SUBSCRIPTION:
            *prio = GPS_PACKET_PRIO_SUBSCRIPTION;
            break;
        case GPS_PACKET_TYPE_GNRS_REQ:
            *prio = GPS_PACKET_PRIO_GNRS_REQ;
            break;
        case GPS_PACKET_TYPE_GNRS_RESP:
            *prio = GPS_PACKET_PRIO_GNRS_RESP;
            break;
        case GPS_PACKET_TYPE_GNRS_ASSO:
            *prio = GPS_PACKET_PRIO_GNRS_ASSO;
            break;
        default:
            *prio = GPS_PACKET_PRIO_UNDEFINED;
            break;
    }
    return p;
}


CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_PrioAnnotator)