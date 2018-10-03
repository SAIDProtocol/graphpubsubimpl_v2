//
// Created by jiachen on 10/1/18.
//

#ifndef CLICK_HRC_PACKETANNO_HH
#define CLICK_HRC_PACKETANNO_HH

#include <net/ethernet.h>
#include <click/config.h>
#include <click/glue.hh>

CLICK_DECLS

#define HRC_ANNO_OFFSET_DST_NAME (0)
#define HRC_ANNO_SIZE_DST_NAME (sizeof(const char *))
#define HRC_ANNO_DST_NAME(p) ((char **)((p)->anno_u8() + HRC_ANNO_OFFSET_DST_NAME))

#define HRC_ANNO_OFFSET_SRC_IP  (16)
#define HRC_ANNO_SIZE_SRC_IP  4
#define HRC_ANNO_SRC_IP(p)  ((unsigned char *)((p)->anno_u8() + HRC_ANNO_OFFSET_SRC_IP))

#define HRC_ANNO_OFFSET_SRC_ETHER  (20)
#define HRC_ANNO_SIZE_SRC_ETHER  (6)
#define HRC_ANNO_SRC_ETHER(p)  ((unsigned char *)((p)->anno_u8() + HRC_ANNO_OFFSET_SRC_ETHER))

#define HRC_ANNO_OFFSET_NEXT_HOP_ETHER  (26)
#define HRC_ANNO_SIZE_NEXT_HOP_ETHER  (6)
#define HRC_ANNO_NEXT_HOP_ETHER(p)  ((unsigned char *)((p)->anno_u8() + HRC_ANNO_OFFSET_NEXT_HOP_ETHER))

#define HRC_ANNO_OFFSET_NEXT_HOP_IP  (32)
#define HRC_ANNO_SIZE_NEXT_HOP_IP  (4)
#define HRC_ANNO_NEXT_HOP_IP(p)  ((unsigned char *)((p)->anno_u8() + HRC_ANNO_OFFSET_NEXT_HOP_IP))

#define HRC_ANNO_OFFSET_NEXT_HOP_NA  (36)
#define HRC_ANNO_SIZE_NEXT_HOP_NA  (4)
#define HRC_ANNO_NEXT_HOP_NA(p)  ((hrc_na_t *)((p)->anno_u8() + HRC_ANNO_OFFSET_NEXT_HOP_NA))

#define HRC_ANNO_OFFSET_SRC_PORT  (40)
#define HRC_ANNO_SIZE_SRC_PORT  1
#define HRC_ANNO_SRC_PORT(p)  ((p)->anno_u8() + HRC_ANNO_OFFSET_SRC_PORT)

#define HRC_ANNO_OFFSET_NEXT_HOP_PORT (41)
#define HRC_ANNO_SIZE_NEXT_HOP_PORT  (1)
#define HRC_ANNO_NEXT_HOP_PORT(p)  ((p)->anno_u8() + HRC_ANNO_OFFSET_NEXT_HOP_PORT)

#define HRC_ANNO_OFFSET_PRIO  (42)
#define HRC_ANNO_SIZE_PRIO  1
#define HRC_ANNO_PRIO(p)  ((p)->anno_u8() + HRC_ANNO_OFFSET_PRIO)

CLICK_ENDDECLS


#endif //CLICK_HRC_PACKETANNO_HH
