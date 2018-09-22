//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_GPS_GUID_HH
#define CLICK_GPS_GUID_HH

#include <click/config.h>
#include <click/glue.hh>
#include <click/string.hh>
#include <cstdint>
#include <functional>
#include <arpa/inet.h>

CLICK_DECLS

#define GPS_GUID_SIZE (20)
#define GPS_GUID_LOG_LENGTH (2 * GPS_GUID_SIZE)

typedef struct gps_guid {
    uint8_t value[GPS_GUID_SIZE];
} gps_guid_t;

static inline void gps_guid_clear(gps_guid_t *guid) { memset(guid->value, 0, GPS_GUID_SIZE); }

static inline void gps_guid_set_val(gps_guid_t *guid, uint32_t val) {
    gps_guid_clear(guid);
    *reinterpret_cast<uint32_t *>(guid->value + GPS_GUID_SIZE - sizeof(uint32_t)) = htonl(val);
}

static inline void gps_guid_set_val(gps_guid_t *guid, const gps_guid_t *val) {
    memcpy(guid->value, val->value, GPS_GUID_SIZE);
}

static inline String gps_guid_unparse(const gps_guid_t *guid) {
    auto str = String::make_uninitialized(GPS_GUID_LOG_LENGTH);
    if (auto x = str.mutable_c_str())
        for (int i = 0; i < GPS_GUID_SIZE; i++)
            snprintf(x + 2 * i, 3, "%02X", guid->value[i]);
    return str;
}

static inline size_t gps_guid_get_hash(const gps_guid_t *guid) {
    auto tmp = reinterpret_cast<const uint32_t *>(guid->value);
    uint32_t ret = 5381;
    ret = ((ret << 5UL) + ret) + tmp[0];
    ret = ((ret << 5UL) + ret) + tmp[1];
    ret = ((ret << 5UL) + ret) + tmp[2];
    ret = ((ret << 5UL) + ret) + tmp[3];
    ret = ((ret << 5UL) + ret) + tmp[4];
    return static_cast<size_t>(ret);
}

static inline bool operator!=(const gps_guid_t &lhs, const gps_guid_t &rhs) {
    return static_cast<bool>(memcmp(lhs.value, rhs.value, GPS_GUID_SIZE));
}

static inline bool operator==(const gps_guid_t &lhs, const gps_guid_t &rhs) { return !(lhs != rhs); }

namespace std {
    template<>
    struct hash<gps_guid_t> {
        size_t operator()(const gps_guid_t &guid) const { return hash<size_t>()(gps_guid_get_hash(&guid)); }
    };
}


CLICK_ENDDECLS

#endif //CLICK_GPS_GUID_HH
