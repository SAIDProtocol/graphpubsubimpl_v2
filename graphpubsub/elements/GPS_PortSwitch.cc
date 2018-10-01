//
// Created by ubuntu on 9/23/18.
//

#include "GPS_PortSwitch.hh"
#include "GPS_PacketAnno.h"

CLICK_DECLS

GPS_PortSwitch::GPS_PortSwitch() = default;

GPS_PortSwitch::~GPS_PortSwitch() = default;


void GPS_PortSwitch::push(int port, Packet *p) {
    (void)port;
    uint8_t *nextHopPort = GPS_ANNO_NEXT_HOP_PORT(p);
    checked_output_push(*nextHopPort, p);
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_PortSwitch)

