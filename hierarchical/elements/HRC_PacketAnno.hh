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

#define HRC_ANNO_OFFSET_PREV_HOP_NA  (16)
#define HRC_ANNO_SIZE_PREV_HOP_NA  (4)
#define HRC_ANNO_PREV_HOP_NA(p)  ((hrc_na_t *)((p)->anno_u8() + HRC_ANNO_OFFSET_PREV_HOP_NA))

#define HRC_ANNO_OFFSET_PREV_HOP_IP  (20)
#define HRC_ANNO_SIZE_PREV_HOP_IP  4
#define HRC_ANNO_PREV_HOP_IP(p)  ((unsigned char *)((p)->anno_u8() + HRC_ANNO_OFFSET_PREV_HOP_IP))

#define HRC_ANNO_OFFSET_PREV_HOP_ETHER  (24)
#define HRC_ANNO_SIZE_PREV_HOP_ETHER  (6)
#define HRC_ANNO_PREV_HOP_ETHER(p)  ((unsigned char *)((p)->anno_u8() + HRC_ANNO_OFFSET_PREV_HOP_ETHER))

#define HRC_ANNO_OFFSET_NEXT_HOP_ETHER  (30)
#define HRC_ANNO_SIZE_NEXT_HOP_ETHER  (6)
#define HRC_ANNO_NEXT_HOP_ETHER(p)  ((unsigned char *)((p)->anno_u8() + HRC_ANNO_OFFSET_NEXT_HOP_ETHER))

#define HRC_ANNO_OFFSET_NEXT_HOP_IP  (36)
#define HRC_ANNO_SIZE_NEXT_HOP_IP  (4)
#define HRC_ANNO_NEXT_HOP_IP(p)  ((unsigned char *)((p)->anno_u8() + HRC_ANNO_OFFSET_NEXT_HOP_IP))

#define HRC_ANNO_OFFSET_NEXT_HOP_NA  (40)
#define HRC_ANNO_SIZE_NEXT_HOP_NA  (4)
#define HRC_ANNO_NEXT_HOP_NA(p)  ((hrc_na_t *)((p)->anno_u8() + HRC_ANNO_OFFSET_NEXT_HOP_NA))

#define HRC_ANNO_OFFSET_PREV_HOP_PORT  (44)
#define HRC_ANNO_SIZE_PREV_HOP_PORT  1
#define HRC_ANNO_PREV_HOP_PORT(p)  ((p)->anno_u8() + HRC_ANNO_OFFSET_PREV_HOP_PORT)

#define HRC_ANNO_OFFSET_NEXT_HOP_PORT (45)
#define HRC_ANNO_SIZE_NEXT_HOP_PORT  (1)
#define HRC_ANNO_NEXT_HOP_PORT(p)  ((p)->anno_u8() + HRC_ANNO_OFFSET_NEXT_HOP_PORT)

#define HRC_ANNO_OFFSET_PRIO  (46)
#define HRC_ANNO_SIZE_PRIO  1
#define HRC_ANNO_PRIO(p)  ((p)->anno_u8() + HRC_ANNO_OFFSET_PRIO)

CLICK_ENDDECLS


#endif //CLICK_HRC_PACKETANNO_HH
