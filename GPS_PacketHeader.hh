//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_GPS_PACKETHEADER_HH
#define CLICK_GPS_PACKETHEADER_HH

#include <click/config.h>
#include "GPS_NA.hh"
#include "GPS_GUID.hh"

CLICK_DECLS

#define GPS_PACKET_TYPE_APPLICATION_MASK (0x80U) // 1000 0000
#define GPS_PACKET_TYPE_GNRS_MASK (0xC0U)  // 1100 0000

#define GPS_PACKET_TYPE_LSA (0x01U)
#define GPS_PACKET_TYPE_PUBLICATION (0x81U) // 1000 0001
#define GPS_PACKET_TYPE_SUBSCRIPTION (0x82U) // 1000 0010
#define GPS_PACKET_TYPE_GNRS_REQ (0xC1U) // 1100 0001
#define GPS_PACKET_TYPE_GNRS_RESP (0xC2U) // 1100 0010
#define GPS_PACKET_TYPE_GNRS_ASSO (0xC3U) // 1000 0011

#define GPS_PACKET_PRIO_LSA (0)
#define GPS_PACKET_PRIO_PUBLICATION (3)
#define GPS_PACKET_PRIO_SUBSCRIPTION (2)
#define GPS_PACKET_PRIO_GNRS_REQ (1)
#define GPS_PACKET_PRIO_GNRS_RESP (1)
#define GPS_PACKET_PRIO_GNRS_ASSO (1)

#define GPS_DEFAULT_HEAD_ROOM (40) // Ether header + IP header

union gps_packet {
    uint8_t type;
    uint32_t spaceholder;
} CLICK_SIZE_PACKED_ATTRIBUTE;
typedef union gps_packet gps_packet_t;

static inline uint8_t gps_packet_get_type(const void *pkt) {
    return reinterpret_cast<const gps_packet_t *>(pkt)->type;
}

static inline void gps_packet_set_type(void *pkt, uint8_t type) {
    reinterpret_cast<gps_packet_t *>(pkt)->type = type;
}

static inline bool gps_packet_is_application(const void *pkt) {
    return static_cast<bool>(gps_packet_get_type(pkt) & GPS_PACKET_TYPE_APPLICATION_MASK);
}

static inline bool gps_packet_is_gnrs(const void *pkt) {
    return static_cast<bool>(gps_packet_get_type(pkt) & GPS_PACKET_TYPE_GNRS_MASK);
}

struct gps_packet_lsa {
    gps_packet_t premable;
    gps_na_t srcNa;
    gps_na_t intermediateNa;
    uint32_t nonce;
} CLICK_SIZE_PACKED_ATTRIBUTE;

typedef struct gps_packet_lsa gps_packet_lsa_t;

static inline const gps_na_t *gps_packet_lsa_get_srcNa(const void *pkt) {
    return &reinterpret_cast<const gps_packet_lsa *>(pkt)->srcNa;
}

static inline const gps_na_t *gps_packet_lsa_get_intermediateNa(const void *pkt) {
    return &reinterpret_cast<const gps_packet_lsa *>(pkt)->intermediateNa;
}

static inline uint32_t gps_packet_lsa_get_nonce(const void *pkt) {
    return ntohl(reinterpret_cast<const gps_packet_lsa *>(pkt)->nonce);
}

static inline void gps_packet_lsa_set_srcNa(void *pkt, const gps_na_t *srcNa) {
    gps_na_set_val(&reinterpret_cast<gps_packet_lsa *>(pkt)->srcNa, srcNa);
}

static inline void gps_packet_lsa_set_intermediateNa(void *pkt, const gps_na_t *intermediateNa) {
    gps_na_set_val(&reinterpret_cast<gps_packet_lsa *>(pkt)->intermediateNa, intermediateNa);
}

static inline void gps_packet_lsa_set_nonce(void *pkt, uint32_t nonce) {
    reinterpret_cast<gps_packet_lsa *>(pkt)->nonce = htonl(nonce);
}

static inline void
gps_packet_lsa_init(void *pkt, const gps_na_t *srcNa, const gps_na_t *intermediateNa, uint32_t nonce) {
    gps_packet_set_type(pkt, GPS_PACKET_TYPE_LSA);
    gps_packet_lsa_set_srcNa(pkt, srcNa);
    gps_packet_lsa_set_intermediateNa(pkt, intermediateNa);
    gps_packet_lsa_set_nonce(pkt, nonce);
}


static inline void gps_packet_lsa_print(const void *pkt, const String &label) {
    const gps_na_t *srcNa = gps_packet_lsa_get_srcNa(pkt),
            *intermediateNa = gps_packet_lsa_get_intermediateNa(pkt);
    uint32_t nonce = gps_packet_lsa_get_nonce(pkt);
    click_chatter("%s: LSA srcNa=%s | intermediateNa=%s | nonce=0x%08x",
                  label.c_str(),
                  gps_na_unparse(srcNa).c_str(),
                  gps_na_unparse(intermediateNa).c_str(),
                  nonce);
}

struct gps_packet_application {
    gps_packet_t premable;
    gps_guid_t srcGuid;
    gps_guid_t dstGuid;
    gps_na_t srcNa;
    gps_na_t dstNa;
} CLICK_SIZE_PACKED_ATTRIBUTE;

typedef struct gps_packet_application gps_packet_application_t;

static inline const gps_guid_t *gps_packet_application_get_srcGuid(const void *pkt) {
    return &reinterpret_cast<const gps_packet_application_t *>(pkt)->srcGuid;
}

static inline const gps_guid_t *gps_packet_application_get_dstGuid(const void *pkt) {
    return &reinterpret_cast<const gps_packet_application_t *>(pkt)->dstGuid;
}

static inline const gps_na_t *gps_packet_application_get_srcNa(const void *pkt) {
    return &reinterpret_cast<const gps_packet_application_t *>(pkt)->srcNa;
}

static inline const gps_na_t *gps_packet_application_get_dstNa(const void *pkt) {
    return &reinterpret_cast<const gps_packet_application_t *>(pkt)->dstNa;
}

static inline gps_guid_t *gps_packet_application_get_srcGuid(void *pkt) {
    return &reinterpret_cast<gps_packet_application_t *>(pkt)->srcGuid;
}

static inline gps_guid_t *gps_packet_application_get_dstGuid(void *pkt) {
    return &reinterpret_cast<gps_packet_application_t *>(pkt)->dstGuid;
}

static inline gps_na_t *gps_packet_application_get_srcNa(void *pkt) {
    return &reinterpret_cast<gps_packet_application_t *>(pkt)->srcNa;
}

static inline gps_na_t *gps_packet_application_get_dstNa(void *pkt) {
    return &reinterpret_cast<gps_packet_application_t *>(pkt)->dstNa;
}

static inline void gps_packet_application_set_srcGuid(void *pkt, const gps_guid_t *srcGuid) {
    gps_guid_set_val(&reinterpret_cast<gps_packet_application_t *>(pkt)->srcGuid, srcGuid);
}

static inline void gps_packet_application_set_dstGuid(void *pkt, const gps_guid_t *dstGuid) {
    gps_guid_set_val(&reinterpret_cast<gps_packet_application_t *>(pkt)->dstGuid, dstGuid);
}

static inline void gps_packet_application_set_srcNa(void *pkt, const gps_na_t *srcNa) {
    gps_na_set_val(&reinterpret_cast<gps_packet_application_t *>(pkt)->srcNa, srcNa);
}

static inline void gps_packet_application_set_dstNa(void *pkt, const gps_na_t *dstNa) {
    gps_na_set_val(&reinterpret_cast<gps_packet_application_t *>(pkt)->dstNa, dstNa);
}


struct gps_packet_publication {
    gps_packet_application_t premable;
    uint32_t size;
    char payload[];
} CLICK_SIZE_PACKED_ATTRIBUTE;
typedef struct gps_packet_publication gps_packet_publication_t;


static inline const gps_guid_t *gps_packet_publication_get_srcGuid(const void *pkt) {
    return gps_packet_application_get_srcGuid(&reinterpret_cast<const gps_packet_publication_t *>(pkt)->premable);
}

static inline const gps_guid_t *gps_packet_publication_get_dstGuid(const void *pkt) {
    return gps_packet_application_get_dstGuid(&reinterpret_cast<const gps_packet_publication_t *>(pkt)->premable);
}

static inline const gps_na_t *gps_packet_publication_get_srcNa(const void *pkt) {
    return gps_packet_application_get_srcNa(&reinterpret_cast<const gps_packet_publication_t *>(pkt)->premable);
}

static inline const gps_na_t *gps_packet_publication_get_dstNa(const void *pkt) {
    return gps_packet_application_get_dstNa(&reinterpret_cast<const gps_packet_publication_t *>(pkt)->premable);
}

static inline gps_guid_t *gps_packet_publication_get_srcGuid(void *pkt) {
    return gps_packet_application_get_srcGuid(&reinterpret_cast<gps_packet_publication_t *>(pkt)->premable);
}

static inline gps_guid_t *gps_packet_publication_get_dstGuid(void *pkt) {
    return gps_packet_application_get_dstGuid(&reinterpret_cast<gps_packet_publication_t *>(pkt)->premable);
}

static inline gps_na_t *gps_packet_publication_get_srcNa(void *pkt) {
    return gps_packet_application_get_srcNa(&reinterpret_cast<gps_packet_publication_t *>(pkt)->premable);
}

static inline gps_na_t *gps_packet_publication_get_dstNa(void *pkt) {
    return gps_packet_application_get_dstNa(&reinterpret_cast<gps_packet_publication_t *>(pkt)->premable);
}


static inline uint32_t gps_packet_publication_get_size(const void *pkt) {
    return ntohl(reinterpret_cast<const gps_packet_publication_t *>(pkt)->size);
}

static inline const char *gps_packet_publication_get_payload(const void *pkt) {
    return reinterpret_cast<const gps_packet_publication_t *>(pkt)->payload;
}

static inline char *gps_packet_publication_get_payload(void *pkt) {
    return reinterpret_cast<gps_packet_publication_t *>(pkt)->payload;
}

static inline void gps_packet_publication_set_srcGuid(void *pkt, const gps_guid_t *srcGuid) {
    gps_packet_application_set_srcGuid(&reinterpret_cast<gps_packet_publication_t *>(pkt)->premable, srcGuid);
}

static inline void gps_packet_publication_set_dstGuid(void *pkt, const gps_guid_t *dstGuid) {
    gps_packet_application_set_dstGuid(&reinterpret_cast<gps_packet_publication_t *>(pkt)->premable, dstGuid);
}

static inline void gps_packet_publication_set_srcNa(void *pkt, const gps_na_t *srcNa) {
    gps_packet_application_set_srcNa(&reinterpret_cast<gps_packet_publication_t *>(pkt)->premable, srcNa);
}

static inline void gps_packet_publication_set_dstNa(void *pkt, const gps_na_t *dstNa) {
    gps_packet_application_set_dstNa(&reinterpret_cast<gps_packet_publication_t *>(pkt)->premable, dstNa);
}

static inline void gps_packet_publication_set_size(void *pkt, uint32_t size) {
    reinterpret_cast<gps_packet_publication_t *>(pkt)->size = htonl(size);
}

// TODO: implement set size and set payload resize the packet

static inline void
gps_packet_publication_init(void *pkt, const gps_guid_t *srcGuid, const gps_guid_t *dstGuid, const gps_na_t *srcNa,
                            const gps_na_t *dstNa, uint32_t size) {
    gps_packet_set_type(pkt, GPS_PACKET_TYPE_PUBLICATION);
    gps_packet_publication_set_srcGuid(pkt, srcGuid);
    gps_packet_publication_set_dstGuid(pkt, dstGuid);
    gps_packet_publication_set_srcNa(pkt, srcNa);
    gps_packet_publication_set_dstNa(pkt, dstNa);
    gps_packet_publication_set_size(pkt, size);
}

static inline void gps_packet_publication_print(const void *pkt, const String &label, uint32_t packetSize) {
    const gps_guid_t *srcGuid = gps_packet_publication_get_srcGuid(pkt),
            *dstGuid = gps_packet_publication_get_dstGuid(pkt);
    const gps_na_t *srcNa = gps_packet_publication_get_srcNa(pkt),
            *dstNa = gps_packet_publication_get_dstNa(pkt);
    uint32_t size = gps_packet_publication_get_size(pkt),
            payloadSize = packetSize - sizeof(gps_packet_publication_t);
    click_chatter(
            "%s: PUBLICATION srcGuid=%s | dstGuid=%s | srcNa=%s | dstNa=%s | size=%d | payload=%d",
            label.c_str(),
            gps_guid_unparse(srcGuid).c_str(),
            gps_guid_unparse(dstGuid).c_str(),
            gps_na_unparse(srcNa).c_str(),
            gps_na_unparse(dstNa).c_str(),
            size,
            payloadSize);
}


struct gps_packet_subscription {
    gps_packet_application_t premable;
    uint8_t subscribe;
} CLICK_SIZE_PACKED_ATTRIBUTE;

typedef struct gps_packet_subscription gps_packet_subscription_t;

static inline const gps_guid_t *gps_packet_subscription_get_srcGuid(const void *pkt) {
    return gps_packet_application_get_srcGuid(&reinterpret_cast<const gps_packet_subscription_t *>(pkt)->premable);
}

static inline const gps_guid_t *gps_packet_subscription_get_dstGuid(const void *pkt) {
    return gps_packet_application_get_dstGuid(&reinterpret_cast<const gps_packet_subscription_t *>(pkt)->premable);
}

static inline const gps_na_t *gps_packet_subscription_get_srcNa(const void *pkt) {
    return gps_packet_application_get_srcNa(&reinterpret_cast<const gps_packet_subscription_t *>(pkt)->premable);
}

static inline const gps_na_t *gps_packet_subscription_get_dstNa(const void *pkt) {
    return gps_packet_application_get_dstNa(&reinterpret_cast<const gps_packet_subscription_t *>(pkt)->premable);
}

static inline gps_guid_t *gps_packet_subscription_get_srcGuid(void *pkt) {
    return gps_packet_application_get_srcGuid(&reinterpret_cast<gps_packet_subscription_t *>(pkt)->premable);
}

static inline gps_guid_t *gps_packet_subscription_get_dstGuid(void *pkt) {
    return gps_packet_application_get_dstGuid(&reinterpret_cast<gps_packet_subscription_t *>(pkt)->premable);
}

static inline gps_na_t *gps_packet_subscription_get_srcNa(void *pkt) {
    return gps_packet_application_get_srcNa(&reinterpret_cast<gps_packet_subscription_t *>(pkt)->premable);
}

static inline gps_na_t *gps_packet_subscription_get_dstNa(void *pkt) {
    return gps_packet_application_get_dstNa(&reinterpret_cast<gps_packet_subscription_t *>(pkt)->premable);
}

static inline bool gps_packet_subscription_is_subscribe(const void *pkt) {
    return static_cast<bool>(reinterpret_cast<const gps_packet_subscription_t *>(pkt)->subscribe);
}

static inline void gps_packet_subscription_set_srcGuid(void *pkt, const gps_guid_t *srcGuid) {
    gps_packet_application_set_srcGuid(&reinterpret_cast<gps_packet_subscription_t *>(pkt)->premable, srcGuid);
}

static inline void gps_packet_subscription_set_dstGuid(void *pkt, const gps_guid_t *dstGuid) {
    gps_packet_application_set_dstGuid(&reinterpret_cast<gps_packet_subscription_t *>(pkt)->premable, dstGuid);
}

static inline void gps_packet_subscription_set_srcNa(void *pkt, const gps_na_t *srcNa) {
    gps_packet_application_set_srcNa(&reinterpret_cast<gps_packet_subscription_t *>(pkt)->premable, srcNa);
}

static inline void gps_packet_subscription_set_dstNa(void *pkt, const gps_na_t *dstNa) {
    gps_packet_application_set_dstNa(&reinterpret_cast<gps_packet_subscription_t *>(pkt)->premable, dstNa);
}

static inline void gps_packet_subscription_set_subscribe(void *pkt, bool subscribe) {
    reinterpret_cast<gps_packet_subscription_t *>(pkt)->subscribe = subscribe;
}


static inline void
gps_packet_subscription_init(void *pkt, const gps_guid_t *dstGuid, const gps_na_t *srcNa,
                             const gps_na_t *dstNa, bool subscribe) {
    gps_packet_set_type(pkt, GPS_PACKET_TYPE_PUBLICATION);
    gps_guid_clear(gps_packet_subscription_get_srcGuid(pkt));
    gps_packet_subscription_set_dstGuid(pkt, dstGuid);
    gps_packet_subscription_set_srcNa(pkt, srcNa);
    gps_packet_subscription_set_dstNa(pkt, dstNa);
    gps_packet_subscription_set_subscribe(pkt, subscribe);
}

static inline void gps_packet_subscription_print(const void *pkt, const String &label) {
    const gps_guid_t *srcGuid = gps_packet_subscription_get_srcGuid(pkt),
            *dstGuid = gps_packet_subscription_get_dstGuid(pkt);
    const gps_na_t *srcNa = gps_packet_subscription_get_srcNa(pkt),
            *dstNa = gps_packet_subscription_get_dstNa(pkt);
    bool subscribe = gps_packet_subscription_is_subscribe(pkt);
    click_chatter(
            "%s: SUBSCRIPTION srcGuid=%s | dstGuid=%s | srcNa=%s | dstNa=%s | subscribe=%s",
            label.c_str(),
            gps_guid_unparse(srcGuid).c_str(),
            gps_guid_unparse(dstGuid).c_str(),
            gps_na_unparse(srcNa).c_str(),
            gps_na_unparse(dstNa).c_str(),
            subscribe ? "TRUE" : "FALSE");
}


struct gps_packet_gnrsRequest {
    // premable.srcGuid -> empty
    // premable.dstGuid -> requestedGuid
    // premable.srcNa -> srcNa
    // premable.dstNa -> gnrsNa (dst)
    gps_packet_application_t premable;
} CLICK_SIZE_PACKED_ATTRIBUTE;

typedef struct gps_packet_gnrsRequest gps_packet_gnrsRequest_t;

static inline const gps_guid_t *gps_packet_gnrsRequest_get_srcGuid(const void *pkt) {
    return gps_packet_application_get_srcGuid(&reinterpret_cast<const gps_packet_gnrsRequest_t *>(pkt)->premable);
}

static inline const gps_guid_t *gps_packet_gnrsRequest_get_dstGuid(const void *pkt) {
    return gps_packet_application_get_dstGuid(&reinterpret_cast<const gps_packet_gnrsRequest_t *>(pkt)->premable);
}

static inline const gps_na_t *gps_packet_gnrsRequest_get_srcNa(const void *pkt) {
    return gps_packet_application_get_srcNa(&reinterpret_cast<const gps_packet_gnrsRequest_t *>(pkt)->premable);
}

static inline const gps_na_t *gps_packet_gnrsRequest_get_dstNa(const void *pkt) {
    return gps_packet_application_get_dstNa(&reinterpret_cast<const gps_packet_gnrsRequest_t *>(pkt)->premable);
}

static inline gps_guid_t *gps_packet_gnrsRequest_get_srcGuid(void *pkt) {
    return gps_packet_application_get_srcGuid(&reinterpret_cast<gps_packet_gnrsRequest_t *>(pkt)->premable);
}

static inline gps_guid_t *gps_packet_gnrsRequest_get_dstGuid(void *pkt) {
    return gps_packet_application_get_dstGuid(&reinterpret_cast<gps_packet_gnrsRequest_t *>(pkt)->premable);
}

static inline gps_na_t *gps_packet_gnrsRequest_get_srcNa(void *pkt) {
    return gps_packet_application_get_srcNa(&reinterpret_cast<gps_packet_gnrsRequest_t *>(pkt)->premable);
}

static inline gps_na_t *gps_packet_gnrsRequest_get_dstNa(void *pkt) {
    return gps_packet_application_get_dstNa(&reinterpret_cast<gps_packet_gnrsRequest_t *>(pkt)->premable);
}

static inline void gps_packet_gnrsRequest_set_srcGuid(void *pkt, const gps_guid_t *srcGuid) {
    gps_packet_application_set_srcGuid(&reinterpret_cast<gps_packet_gnrsRequest_t *>(pkt)->premable, srcGuid);
}

static inline void gps_packet_gnrsRequest_set_dstGuid(void *pkt, const gps_guid_t *dstGuid) {
    gps_packet_application_set_dstGuid(&reinterpret_cast<gps_packet_gnrsRequest_t *>(pkt)->premable, dstGuid);
}

static inline void gps_packet_gnrsRequest_set_srcNa(void *pkt, const gps_na_t *srcNa) {
    gps_packet_application_set_srcNa(&reinterpret_cast<gps_packet_gnrsRequest_t *>(pkt)->premable, srcNa);
}

static inline void gps_packet_gnrsRequest_set_dstNa(void *pkt, const gps_na_t *dstNa) {
    gps_packet_application_set_dstNa(&reinterpret_cast<gps_packet_gnrsRequest_t *>(pkt)->premable, dstNa);
}


static inline void
gps_packet_gnrsRequest_init(void *pkt, const gps_guid_t *requestGuid, const gps_na_t *srcNa, const gps_na_t *dstNa) {
    gps_packet_set_type(pkt, GPS_PACKET_TYPE_GNRS_REQ);
    gps_guid_clear(gps_packet_gnrsRequest_get_srcGuid(pkt));
    gps_packet_gnrsRequest_set_dstGuid(pkt, requestGuid);
    gps_packet_gnrsRequest_set_srcNa(pkt, srcNa);
    gps_packet_gnrsRequest_set_dstNa(pkt, dstNa);
}

static inline void gps_packet_gnrsRequest_print(const void *pkt, const String &label) {
    const gps_guid_t *srcGuid = gps_packet_gnrsRequest_get_srcGuid(pkt),
            *dstGuid = gps_packet_gnrsRequest_get_dstGuid(pkt);
    const gps_na_t *srcNa = gps_packet_gnrsRequest_get_srcNa(pkt),
            *dstNa = gps_packet_gnrsRequest_get_dstNa(pkt);
    click_chatter(
            "%s: GNRS_REQUEST srcGuid(empty)=%s | dstGuid(request)=%s | srcNa=%s | dstNa=%s",
            label.c_str(),
            gps_guid_unparse(srcGuid).c_str(),
            gps_guid_unparse(dstGuid).c_str(),
            gps_na_unparse(srcNa).c_str(),
            gps_na_unparse(dstNa).c_str());
}


struct gps_packet_gnrsResponse {
    // premable.srcGuid -> requestedGuid
    // premable.dstGuid -> empty
    // premable.srcNa -> gnrsNa
    // premable.dstNa -> requesterGuid (dst)
    gps_packet_application_t premable;
    gps_na_t requestedGuidNa;
    uint32_t version;
    uint32_t nonce;
} CLICK_SIZE_PACKED_ATTRIBUTE;

typedef struct gps_packet_gnrsResponse gps_packet_gnrsResponse_t;

static inline const gps_guid_t *gps_packet_gnrsResponse_get_srcGuid(const void *pkt) {
    return gps_packet_application_get_srcGuid(&reinterpret_cast<const gps_packet_gnrsResponse_t *>(pkt)->premable);
}

static inline const gps_guid_t *gps_packet_gnrsResponse_get_dstGuid(const void *pkt) {
    return gps_packet_application_get_dstGuid(&reinterpret_cast<const gps_packet_gnrsResponse_t *>(pkt)->premable);
}

static inline const gps_na_t *gps_packet_gnrsResponse_get_srcNa(const void *pkt) {
    return gps_packet_application_get_srcNa(&reinterpret_cast<const gps_packet_gnrsResponse_t *>(pkt)->premable);
}

static inline const gps_na_t *gps_packet_gnrsResponse_get_dstNa(const void *pkt) {
    return gps_packet_application_get_dstNa(&reinterpret_cast<const gps_packet_gnrsResponse_t *>(pkt)->premable);
}

static inline const gps_na_t *gps_packet_gnrsResponse_get_requestedGuidNa(const void *pkt) {
    return &reinterpret_cast<const gps_packet_gnrsResponse_t *>(pkt)->requestedGuidNa;
}

static inline gps_guid_t *gps_packet_gnrsResponse_get_srcGuid(void *pkt) {
    return gps_packet_application_get_srcGuid(&reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->premable);
}

static inline gps_guid_t *gps_packet_gnrsResponse_get_dstGuid(void *pkt) {
    return gps_packet_application_get_dstGuid(&reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->premable);
}

static inline gps_na_t *gps_packet_gnrsResponse_get_srcNa(void *pkt) {
    return gps_packet_application_get_srcNa(&reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->premable);
}

static inline gps_na_t *gps_packet_gnrsResponse_get_dstNa(void *pkt) {
    return gps_packet_application_get_dstNa(&reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->premable);
}

static inline gps_na_t *gps_packet_gnrsResponse_get_requestedGuidNa(void *pkt) {
    return &reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->requestedGuidNa;
}

static inline uint32_t gps_packet_gnrsResponse_get_version(const void *pkt) {
    return ntohl(reinterpret_cast<const gps_packet_gnrsResponse_t *>(pkt)->version);
}

static inline uint32_t gps_packet_gnrsResponse_get_nonce(const void *pkt) {
    return ntohl(reinterpret_cast<const gps_packet_gnrsResponse_t *>(pkt)->nonce);
}

static inline void gps_packet_gnrsResponse_set_srcGuid(void *pkt, const gps_guid_t *srcGuid) {
    gps_packet_application_set_srcGuid(&reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->premable, srcGuid);
}

static inline void gps_packet_gnrsResponse_set_dstGuid(void *pkt, const gps_guid_t *dstGuid) {
    gps_packet_application_set_dstGuid(&reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->premable, dstGuid);
}

static inline void gps_packet_gnrsResponse_set_srcNa(void *pkt, const gps_na_t *srcNa) {
    gps_packet_application_set_srcNa(&reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->premable, srcNa);
}

static inline void gps_packet_gnrsResponse_set_dstNa(void *pkt, const gps_na_t *dstNa) {
    gps_packet_application_set_dstNa(&reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->premable, dstNa);
}

static inline void gps_packet_gnrsResponse_set_requestedGuidNa(void *pkt, const gps_na_t *requestedGuidNa) {
    gps_na_set_val(&reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->requestedGuidNa, requestedGuidNa);
}

static inline void gps_packet_gnrsResponse_set_version(void *pkt, uint32_t version) {
    reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->version = htonl(version);
}

static inline void gps_packet_gnrsResponse_set_nonce(void *pkt, uint32_t nonce) {
    reinterpret_cast<gps_packet_gnrsResponse_t *>(pkt)->nonce = htonl(nonce);
}

static inline void
gps_packet_gnrsResponse_init(void *pkt, const gps_guid_t *requestGuid, const gps_na_t *srcNa, const gps_na_t *dstNa,
                             const gps_na_t *requestedGuidNa, uint32_t version, uint32_t nonce) {
    gps_packet_set_type(pkt, GPS_PACKET_TYPE_GNRS_RESP);
    gps_packet_gnrsResponse_set_srcGuid(pkt, requestGuid);
    gps_guid_clear(gps_packet_gnrsResponse_get_dstGuid(pkt));
    gps_packet_gnrsResponse_set_srcNa(pkt, srcNa);
    gps_packet_gnrsResponse_set_dstNa(pkt, dstNa);
    gps_packet_gnrsResponse_set_requestedGuidNa(pkt, requestedGuidNa);
    gps_packet_gnrsResponse_set_version(pkt, version);
    gps_packet_gnrsResponse_set_nonce(pkt, nonce);
}

static inline void gps_packet_gnrsResponse_print(const void *pkt, const String &label) {
    const gps_guid_t *srcGuid = gps_packet_gnrsResponse_get_srcGuid(pkt),
            *dstGuid = gps_packet_gnrsResponse_get_dstGuid(pkt);
    const gps_na_t *srcNa = gps_packet_gnrsResponse_get_srcNa(pkt),
            *dstNa = gps_packet_gnrsResponse_get_dstNa(pkt),
            *requestedGuidNa = gps_packet_gnrsResponse_get_requestedGuidNa(pkt);
    uint32_t version = gps_packet_gnrsResponse_get_version(pkt),
            nonce = gps_packet_gnrsResponse_get_nonce(pkt);

    click_chatter(
            "%s: GNRS_RESPONSE srcGuid(request)=%s | dstGuid(empty)=%s | srcNa=%s | dstNa=%s | requestedGuidNa=%s | version=0x%08x | nonce=0x%08x",
            label.c_str(),
            gps_guid_unparse(srcGuid).c_str(),
            gps_guid_unparse(dstGuid).c_str(),
            gps_na_unparse(srcNa).c_str(),
            gps_na_unparse(dstNa).c_str(),
            gps_na_unparse(requestedGuidNa).c_str(),
            version,
            nonce);
}

struct gps_packet_gnrsAssociation {
    // premable.srcGuid -> requestedGuid
    // premable.dstGuid -> empty
    // premable.srcNa -> srcNa
    // premable.dstNa -> gnrsNa (dst)
    gps_packet_application_t premable;
    uint8_t fromEndhost;
} CLICK_SIZE_PACKED_ATTRIBUTE;

typedef struct gps_packet_gnrsAssociation gps_packet_gnrsAssociation_t;

static inline const gps_guid_t *gps_packet_gnrsAssociation_get_srcGuid(const void *pkt) {
    return gps_packet_application_get_srcGuid(&reinterpret_cast<const gps_packet_gnrsAssociation_t *>(pkt)->premable);
}

static inline const gps_guid_t *gps_packet_gnrsAssociation_get_dstGuid(const void *pkt) {
    return gps_packet_application_get_dstGuid(&reinterpret_cast<const gps_packet_gnrsAssociation_t *>(pkt)->premable);
}

static inline const gps_na_t *gps_packet_gnrsAssociation_get_srcNa(const void *pkt) {
    return gps_packet_application_get_srcNa(&reinterpret_cast<const gps_packet_gnrsAssociation_t *>(pkt)->premable);
}

static inline const gps_na_t *gps_packet_gnrsAssociation_get_dstNa(const void *pkt) {
    return gps_packet_application_get_dstNa(&reinterpret_cast<const gps_packet_gnrsAssociation_t *>(pkt)->premable);
}

static inline gps_guid_t *gps_packet_gnrsAssociation_get_srcGuid(void *pkt) {
    return gps_packet_application_get_srcGuid(&reinterpret_cast<gps_packet_gnrsAssociation_t *>(pkt)->premable);
}

static inline gps_guid_t *gps_packet_gnrsAssociation_get_dstGuid(void *pkt) {
    return gps_packet_application_get_dstGuid(&reinterpret_cast<gps_packet_gnrsAssociation_t *>(pkt)->premable);
}

static inline gps_na_t *gps_packet_gnrsAssociation_get_srcNa(void *pkt) {
    return gps_packet_application_get_srcNa(&reinterpret_cast<gps_packet_gnrsAssociation_t *>(pkt)->premable);
}

static inline gps_na_t *gps_packet_gnrsAssociation_get_dstNa(void *pkt) {
    return gps_packet_application_get_dstNa(&reinterpret_cast<gps_packet_gnrsAssociation_t *>(pkt)->premable);
}

static inline bool gps_packet_gnrsAssociation_is_fromEndhost(const void *pkt) {
    return static_cast<bool>(reinterpret_cast<const gps_packet_gnrsAssociation_t *>(pkt)->fromEndhost);
}

static inline void gps_packet_gnrsAssociation_set_srcGuid(void *pkt, const gps_guid_t *srcGuid) {
    gps_packet_application_set_srcGuid(&reinterpret_cast<gps_packet_gnrsAssociation_t *>(pkt)->premable, srcGuid);
}

static inline void gps_packet_gnrsAssociation_set_dstGuid(void *pkt, const gps_guid_t *dstGuid) {
    gps_packet_application_set_dstGuid(&reinterpret_cast<gps_packet_gnrsAssociation_t *>(pkt)->premable, dstGuid);
}

static inline void gps_packet_gnrsAssociation_set_srcNa(void *pkt, const gps_na_t *srcNa) {
    gps_packet_application_set_srcNa(&reinterpret_cast<gps_packet_gnrsAssociation_t *>(pkt)->premable, srcNa);
}

static inline void gps_packet_gnrsAssociation_set_dstNa(void *pkt, const gps_na_t *dstNa) {
    gps_packet_application_set_dstNa(&reinterpret_cast<gps_packet_gnrsAssociation_t *>(pkt)->premable, dstNa);
}

static inline void gps_packet_gnrsAssociation_set_fromEndhost(void *pkt, bool fromEndhost) {
    reinterpret_cast<gps_packet_gnrsAssociation_t *>(pkt)->fromEndhost = fromEndhost;
}

static inline void
gps_packet_gnrsAssociation_init(void *pkt, const gps_guid_t *srcGuid, const gps_na_t *srcNa,
                             const gps_na_t *dstNa, bool fromEndHost) {
    gps_packet_set_type(pkt, GPS_PACKET_TYPE_GNRS_ASSO);
    gps_packet_gnrsAssociation_set_srcGuid(pkt, srcGuid);
    gps_guid_clear(gps_packet_gnrsAssociation_get_dstGuid(pkt));
    gps_packet_gnrsAssociation_set_srcNa(pkt, srcNa);
    gps_packet_gnrsAssociation_set_dstNa(pkt, dstNa);
    gps_packet_gnrsAssociation_set_fromEndhost(pkt, fromEndHost);
}

static inline void gps_packet_gnrsAssociation_print(const void *pkt, const String &label) {
    const gps_guid_t *srcGuid = gps_packet_gnrsAssociation_get_srcGuid(pkt),
            *dstGuid = gps_packet_gnrsAssociation_get_dstGuid(pkt);
    const gps_na_t *srcNa = gps_packet_gnrsAssociation_get_srcNa(pkt),
            *dstNa = gps_packet_gnrsAssociation_get_dstNa(pkt);
    bool fromEndHost = gps_packet_gnrsAssociation_is_fromEndhost(pkt);
    click_chatter(
            "%s: SUBSCRIPTION srcGuid=%s | dstGuid=%s | srcNa=%s | dstNa=%s | fromEndhost=%s",
            label.c_str(),
            gps_guid_unparse(srcGuid).c_str(),
            gps_guid_unparse(dstGuid).c_str(),
            gps_na_unparse(srcNa).c_str(),
            gps_na_unparse(dstNa).c_str(),
            fromEndHost ? "TRUE" : "FALSE");
}

CLICK_ENDDECLS

#endif //CLICK_GPS_PACKETHEADER_HH

