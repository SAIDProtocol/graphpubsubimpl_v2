//
// Created by jiachen on 10/3/18.
//

#ifndef CLICK_HRC_PACKETHEADER_HH
#define CLICK_HRC_PACKETHEADER_HH

#include <click/config.h>
#include <click/glue.hh>
#include <click/string.hh>
#include <cstring>

CLICK_DECLS

#define HRC_PACKET_TYPE_LSA (0x01U)
#define HRC_PACKET_TYPE_INTEREST (0x81U)
#define HRC_PACKET_TYPE_PUBLICATION (0x83U)
#define HRC_PACKET_TYPE_SUBSCRIPTION (0x84U)

#define HRC_PACKET_PRIO_INTEREST (3)
#define HRC_PACKET_PRIO_PUBLICATION (3)
#define HRC_PACKET_PRIO_SUBSCRIPTION (2)

#define HRC_PACKET_PRIO_UNDEFINED (8)

#define HRC_DEFAULT_HEAD_ROOM (40) // Ether header + IP header


union hrc_packet {
    uint8_t type;
    uint32_t spaceholder;
} CLICK_SIZE_PACKED_ATTRIBUTE;
typedef union hrc_packet hrc_packet_t;

static inline uint8_t hrc_packet_get_type(const void *pkt) {
    return reinterpret_cast<const hrc_packet_t *>(pkt)->type;
}

static inline void hrc_packet_set_type(void *pkt, uint8_t type) {
    reinterpret_cast<hrc_packet_t *>(pkt)->type = type;
}

struct hrc_packet_interest {
    hrc_packet_t premable;
    uint32_t size;
    char name[];
} CLICK_SIZE_PACKED_ATTRIBUTE;
typedef struct hrc_packet_interest hrc_packet_interest_t;

static inline uint32_t hrc_packet_interest_get_size(const void *pkt) {
    return ntohl(reinterpret_cast<const hrc_packet_interest_t *>(pkt)->size);
}

static inline const char *hrc_packet_interest_get_name(const void *pkt) {
    return reinterpret_cast<const hrc_packet_interest_t *>(pkt)->name;
}

static inline void hrc_packet_interest_set_size(void *pkt, uint32_t size) {
    reinterpret_cast<hrc_packet_interest_t *>(pkt)->size = htonl(size);
}

static inline size_t hrc_packet_interest_calculate_header_size(const char *name) {
    return sizeof(hrc_packet_interest_t) + strlen(name) + 1;
}

static inline size_t hrc_packet_interest_get_header_size(const void *pkt) {
    return sizeof(hrc_packet_interest_t) + strlen(hrc_packet_interest_get_name(pkt)) + 1;
}

static inline void hrc_packet_interest_init(void *pkt, const char *name, uint32_t size) {
    hrc_packet_set_type(pkt, HRC_PACKET_TYPE_INTEREST);
    hrc_packet_interest_set_size(pkt, size);
    strcpy(reinterpret_cast<hrc_packet_interest_t *>(pkt)->name, name);
}

static inline void
hrc_packet_interest_print(const void *pkt, const String &label, uint32_t packetSize, uint32_t printLimit,
                          uint32_t headRoom, uint32_t tailRoom) {
    auto tmpPayloadBuf = new char[2 * printLimit + 1];
    auto name = hrc_packet_interest_get_name(pkt);
    auto size = hrc_packet_interest_get_size(pkt);
    auto headerSize = hrc_packet_interest_calculate_header_size(name);
    auto payloadSize = packetSize - headerSize;
    auto payload = reinterpret_cast<const unsigned char *>(pkt) + headerSize;
    for (decltype(size) i = 0; i < payloadSize && i < size && i < printLimit; ++i)
        snprintf(tmpPayloadBuf + i * 2, 3, "%02x", payload[i]);

    click_chatter("%s: INTEREST name=%s | size=%d | payload=%d | content(lim %d)=%s | [h:%d,t:%d]", label.c_str(), name,
                  size, payloadSize, printLimit, tmpPayloadBuf, headRoom, tailRoom);
    delete[] tmpPayloadBuf;
}

struct hrc_packet_publication {
    hrc_packet_t premable;
    uint32_t size;
    char name[];
} CLICK_SIZE_PACKED_ATTRIBUTE;

typedef struct hrc_packet_publication hrc_packet_publication_t;

static inline uint32_t hrc_packet_publication_get_size(const void *pkt) {
    return ntohl(reinterpret_cast<const hrc_packet_publication_t *>(pkt)->size);
}

static inline const char *hrc_packet_publication_get_name(const void *pkt) {
    return reinterpret_cast<const hrc_packet_publication_t *>(pkt)->name;
}

static inline void hrc_packet_publication_set_size(void *pkt, uint32_t size) {
    reinterpret_cast<hrc_packet_publication_t *>(pkt)->size = htonl(size);
}

static inline size_t hrc_packet_publication_calculate_header_size(const char *name) {
    return sizeof(hrc_packet_publication_t) + strlen(name) + 1;
}

static inline size_t hrc_packet_publication_get_header_size(const void *pkt) {
    return sizeof(hrc_packet_publication_t) + strlen(hrc_packet_publication_get_name(pkt)) + 1;
}

static inline void hrc_packet_publication_init(void *pkt, const char *name, uint32_t size) {
    hrc_packet_set_type(pkt, HRC_PACKET_TYPE_PUBLICATION);
    hrc_packet_publication_set_size(pkt, size);
    strcpy(reinterpret_cast<hrc_packet_publication_t *>(pkt)->name, name);
}

static inline void
hrc_packet_publication_print(const void *pkt, const String &label, uint32_t packetSize, uint32_t printLimit,
                             uint32_t headRoom, uint32_t tailRoom) {
    auto tmpPayloadBuf = new char[2 * printLimit + 1];
    auto name = hrc_packet_publication_get_name(pkt);
    auto size = hrc_packet_publication_get_size(pkt);
    auto headerSize = hrc_packet_publication_calculate_header_size(name);
    auto payloadSize = packetSize - headerSize;
    auto payload = reinterpret_cast<const unsigned char *>(pkt) + headerSize;
    for (decltype(size) i = 0; i < payloadSize && i < size && i < printLimit; ++i)
        snprintf(tmpPayloadBuf + i * 2, 3, "%02x", payload[i]);

    click_chatter("%s: PUBLICATION name=%s | size=%d | payload=%d | content(lim %d)=%s | [h:%d,t:%d]", label.c_str(),
                  name, size,
                  payloadSize, printLimit, tmpPayloadBuf, headRoom, tailRoom);
    delete[] tmpPayloadBuf;
}

struct hrc_packet_subscription {
    hrc_packet_t premable;
    unsigned char sub;
    unsigned char unused[3];
    char name[];
} CLICK_SIZE_PACKED_ATTRIBUTE;

typedef struct hrc_packet_subscription hrc_packet_subscription_t;

static inline const char *hrc_packet_subscription_get_name(const void *pkt) {
    return reinterpret_cast<const hrc_packet_subscription_t *>(pkt)->name;
}

static inline bool hrc_packet_subscription_is_sub(const void *pkt) {
    return static_cast<bool>(reinterpret_cast<const hrc_packet_subscription_t *>(pkt)->sub);
}

static inline size_t hrc_packet_subscription_calculate_header_size(const char *name) {
    return sizeof(hrc_packet_subscription_t) + strlen(name) + 1;
}

static inline size_t hrc_packet_subscription_get_header_size(const void *pkt) {
    return sizeof(hrc_packet_subscription_t) + strlen(hrc_packet_subscription_get_name(pkt)) + 1;
}

static inline void hrc_packet_subscription_set_sub(void *pkt, bool sub) {
    reinterpret_cast<hrc_packet_subscription_t *>(pkt)->sub = sub;
}

static inline void hrc_packet_subscription_init(void *pkt, const char *name, bool sub) {
    hrc_packet_set_type(pkt, HRC_PACKET_TYPE_SUBSCRIPTION);
    hrc_packet_subscription_set_sub(pkt, sub);
    strcpy(reinterpret_cast<hrc_packet_subscription_t *>(pkt)->name, name);
}

static inline void
hrc_packet_subscription_print(const void *pkt, const String &label, uint32_t packetSize, uint32_t headRoom,
                              uint32_t tailRoom) {
    auto name = hrc_packet_subscription_get_name(pkt);
    auto sub = hrc_packet_subscription_is_sub(pkt);

    click_chatter("%s: SUBSCRIPTION name=%s | sub=%d | size=%d(%d) [h:%d,t:%d]", label.c_str(), name, sub,
                  hrc_packet_subscription_get_header_size(pkt), packetSize, headRoom, tailRoom);
}


CLICK_ENDDECLS

#endif //CLICK_HRC_PACKETHEADER_HH
