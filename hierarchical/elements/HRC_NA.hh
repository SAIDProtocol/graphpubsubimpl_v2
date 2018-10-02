//
// Created by jiachen on 10/1/18.
//

#ifndef CLICK_HRC_NA_HH
#define CLICK_HRC_NA_HH

#include <click/config.h>
#include <click/glue.hh>
#include <click/string.hh>
#include <cstdint>
#include <functional>
#include <arpa/inet.h>

CLICK_DECLS

typedef struct hrc_na {
    uint32_t value;
} hrc_na_t;


static inline void hrc_na_clear(hrc_na_t *na) { na->value = 0; }

static inline void hrc_na_set_val(hrc_na_t *na, uint32_t val) {
    na->value = htonl(val);
}

static inline void hrc_na_set_val(hrc_na_t *na, const hrc_na_t *val) {
    na->value = val->value;
}

static inline void hrc_na_set_broadcast(hrc_na_t *na) {
    na->value = static_cast<uint32_t>(-1);
}

static inline bool hrc_na_is_broadcast(const hrc_na_t *na) {
    return na->value == static_cast<uint32_t >(-1);
}

static inline bool hrc_na_is_empty(const hrc_na_t *na) {
    return na->value == 0;
}

static inline String hrc_na_unparse(const hrc_na_t *na) {
    auto str = String::make_uninitialized(2 * sizeof(hrc_na_t::value));
    if (auto x = str.mutable_c_str())
        snprintf(x, 9, "%08x", ntohl(na->value));
    return str;
}

static inline size_t hrc_na_get_hash(const hrc_na_t *na) {
    return static_cast<size_t>(na->value);
}

static inline bool operator==(const hrc_na_t &lhs, const hrc_na_t &rhs) {
    return lhs.value == rhs.value;
}

static inline bool operator!=(const hrc_na_t &lhs, const hrc_na_t &rhs) { return !(lhs == rhs); }

namespace std {
    template<>
    struct hash<hrc_na_t> {
        size_t operator()(const hrc_na_t &na) const { return hash<size_t>()(hrc_na_get_hash(&na)); }
    };
}

CLICK_ENDDECLS
#endif //CLICK_HRC_NA_HH
