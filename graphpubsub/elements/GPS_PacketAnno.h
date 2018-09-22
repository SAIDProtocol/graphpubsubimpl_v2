//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_GPS_PACKETANNO_H
#define CLICK_GPS_PACKETANNO_H

#include <click/cxxprotect.h>
#include <net/ethernet.h>

CLICK_CXX_PROTECT

#define SRC_IP_ANNO_OFFSET  (0)
#define SRC_IP_ANNO_SIZE  4
#define SRC_IP_ANNO(p)  ((struct in_addr *)((p)->anno_u8() + SRC_IP_ANNO_OFFSET))

#define SRC_ETHER_ANNO_OFFSET  (4)
#define SRC_ETHER_ANNO_SIZE  (6)
#define SRC_ETHER_ANNO(p)  ((struct ether_addr *)((p)->anno_u8() + SRC_ETHER_ANNO_OFFSET))

#define NEXT_HOP_ETHER_ANNO_OFFSET  (10)
#define NEXT_HOP_ETHER_ANNO_SIZE  (6)
#define NEXT_HOP_ETHER_ANNO(p)  ((struct ether_addr *)((p)->anno_u8() + NEXT_HOP_ETHER_ANNO_OFFSET))

#define NEXT_HOP_IP_ANNO_OFFSET  (16)
#define NEXT_HOP_IP_ANNO_SIZE  (4)
#define NEXT_HOP_IP_ANNO(p)  ((struct ether_addr *)((p)->anno_u8() + NEXT_HOP_IP_ANNO_OFFSET))

#define NEXT_HOP_NA_ANNO_OFFSET  (20)
#define NEXT_HOP_NA_ANNO_SIZE  (4)
#define NEXT_HOP_NA_ANNO(p)  ((gps_na_t *)((p)->anno_u8() + NEXT_HOP_NA_ANNO_OFFSET))

#define SRC_PORT_ANNO_OFFSET  (24)
#define SRC_PORT_ANNO_SIZE  1
#define SRC_PORT_ANNO(p)  ((p)->anno_u8() + SRC_PORT_ANNO_OFFSET)

#define NEXT_HOP_PORT_ANNO_OFFSET (25)
#define NEXT_HOP_PORT_ANNO_SIZE  (1)
#define NEXT_HOP_PORT_ANNO(p)  ((p)->anno_u8() + NEXT_HOP_PORT_ANNO_OFFSET)

#define PRIO_ANNO_OFFSET  (26)
#define PRIO_ANNO_SIZE  1
#define PRIO_ANNO(p)  ((p)->anno_u8() + PRIO_ANNO_OFFSET)


CLICK_CXX_UNPROTECT

#include <click/cxxunprotect.h>

#endif //CLICK_GPS_PACKETANNO_H
