//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_GPS_NA_HH
#define CLICK_GPS_NA_HH

#include <click/config.h>
#include <click/glue.hh>
#include <click/string.hh>
#include <cstdint>
#include <functional>
#include <arpa/inet.h>

CLICK_DECLS

typedef struct gps_na {
    uint32_t value;
} gps_na_t;

static inline void gps_na_clear(gps_na_t *na) { na->value = 0; }

static inline void gps_na_set_val(gps_na_t *na, uint32_t val) {
    na->value = htonl(val);
}

static inline void gps_na_set_val(gps_na_t *na, const gps_na_t *val) {
    na->value = val->value;
}

static inline void gps_na_set_broadcast(gps_na_t *na) {
    na->value = static_cast<uint32_t>(-1);
}

static inline bool gps_na_is_broadcast(const gps_na_t *na) {
    return na->value == static_cast<uint32_t >(-1);
}

static inline bool gps_na_is_empty(const gps_na_t *na) {
    return na->value == 0;
}

static inline String gps_na_unparse(const gps_na_t *na) {
    auto str = String::make_uninitialized(2 * sizeof(uint32_t));
    if (auto x = str.mutable_c_str())
        snprintf(x, 9, "%08x", ntohl(na->value));
    return str;
}

static inline size_t gps_na_get_hash(const gps_na_t *na) {
    return static_cast<size_t>(na->value);
}

static inline bool operator==(const gps_na_t &lhs, const gps_na_t &rhs) {
    return lhs.value == rhs.value;
}

static inline bool operator!=(const gps_na_t &lhs, const gps_na_t &rhs) { return !(lhs == rhs); }

namespace std {
    template<>
    struct hash<gps_na_t> {
        size_t operator()(const gps_na_t &na) const { return hash<size_t>()(gps_na_get_hash(&na)); }
    };
}


CLICK_ENDDECLS
#endif //CLICK_GPS_NA_HH
