//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_GPS_PACKETANNO_HH
#define CLICK_GPS_PACKETANNO_HH

#include <net/ethernet.h>
#include <click/config.h>
#include <click/glue.hh>

CLICK_DECLS

#define GPS_ANNO_OFFSET_SRC_IP  (16)
#define GPS_ANNO_SIZE_SRC_IP  4
#define GPS_ANNO_SRC_IP(p)  ((unsigned char *)((p)->anno_u8() + GPS_ANNO_OFFSET_SRC_IP))

#define GPS_ANNO_OFFSET_SRC_ETHER  (20)
#define GPS_ANNO_SIZE_SRC_ETHER  (6)
#define GPS_ANNO_SRC_ETHER(p)  ((unsigned char *)((p)->anno_u8() + GPS_ANNO_OFFSET_SRC_ETHER))

#define GPS_ANNO_OFFSET_NEXT_HOP_ETHER  (26)
#define GPS_ANNO_SIZE_NEXT_HOP_ETHER  (6)
#define GPS_ANNO_NEXT_HOP_ETHER(p)  ((unsigned char *)((p)->anno_u8() + GPS_ANNO_OFFSET_NEXT_HOP_ETHER))

#define GPS_ANNO_OFFSET_NEXT_HOP_IP  (32)
#define GPS_ANNO_SIZE_NEXT_HOP_IP  (4)
#define GPS_ANNO_NEXT_HOP_IP(p)  ((unsigned char *)((p)->anno_u8() + GPS_ANNO_OFFSET_NEXT_HOP_IP))

#define GPS_ANNO_OFFSET_NEXT_HOP_NA  (36)
#define GPS_ANNO_SIZE_NEXT_HOP_NA  (4)
#define GPS_ANNO_NEXT_HOP_NA(p)  ((gps_na_t *)((p)->anno_u8() + GPS_ANNO_OFFSET_NEXT_HOP_NA))

#define GPS_ANNO_OFFSET_SRC_PORT  (40)
#define GPS_ANNO_SIZE_SRC_PORT  1
#define GPS_ANNO_SRC_PORT(p)  ((p)->anno_u8() + GPS_ANNO_OFFSET_SRC_PORT)

#define GPS_ANNO_OFFSET_NEXT_HOP_PORT (41)
#define GPS_ANNO_SIZE_NEXT_HOP_PORT  (1)
#define GPS_ANNO_NEXT_HOP_PORT(p)  ((p)->anno_u8() + GPS_ANNO_OFFSET_NEXT_HOP_PORT)

#define GPS_ANNO_OFFSET_PRIO  (42)
#define GPS_ANNO_SIZE_PRIO  1
#define GPS_ANNO_PRIO(p)  ((p)->anno_u8() + GPS_ANNO_OFFSET_PRIO)


CLICK_ENDDECLS

#endif //CLICK_GPS_PACKETANNO_HH
