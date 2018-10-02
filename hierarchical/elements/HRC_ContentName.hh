//
// Created by jiachen on 9/25/18.
//

#ifndef CLICK_HRC_CONTENTNAME_H
#define CLICK_HRC_CONTENTNAME_H

#include <click/config.h>
#include <click/glue.hh>
#include <click/string.hh>
#include <cstring>
#include <vector>

CLICK_DECLS

//#define HRC_CONTENTNAME_DEBUG

#define NAME_SEPARATOR ('/')

typedef struct cnParse {
    std::vector<const char *> components;
} cnParse_t;


static inline int hrc_cn_parse(const char *cn, cnParse_t *parseResult) {
    auto tmp = cn;
    int count = 0;
    // if string starts from "/", skip one
    if (*tmp == NAME_SEPARATOR)
        tmp++;
    parseResult->components.push_back(tmp);

    for (; *tmp != 0; tmp++) {
        if (*tmp == NAME_SEPARATOR) {
            parseResult->components.push_back(tmp + 1);
            count = 0;
        } else {
            count++;
        }
    }
    if (count > 0)
        parseResult->components.push_back(tmp + 1);
    return 0;
}

static inline size_t hrc_cn_get_size(const cnParse_t *parseResult) {
    return parseResult->components.size() - 1;
}

static inline size_t hrc_cn_get_part_size(const cnParse_t *parseResult, size_t part) {
    return parseResult->components[part + 1] - parseResult->components[part] - 1;
}

static inline int hrc_cn_part_cmp(const cnParse_t *parseResult, size_t part, const char *str) {
    auto p = parseResult->components[part];
    auto size = hrc_cn_get_part_size(parseResult, part);
    int v = strncmp(p, str, size);
    if (v) return v;
    if (str[size] == '\0' || str[size] == NAME_SEPARATOR) return 0;
    return str[size];
}

static inline char *hrc_cn_get_part(const cnParse_t *parseResult, size_t part) {
    auto size = hrc_cn_get_part_size(parseResult, part);
    auto ret = new char[size + 1];
    strncpy(ret, parseResult->components[part], size);
    ret[size] = '\0';
    return ret;
}


class CNPart {
public:
    inline static size_t getComponentSize(const char *component) {
        size_t ret = 0;
        for (const char *tmp = component; *tmp != '\0' && *tmp != NAME_SEPARATOR; ++tmp) ret++;
        return ret;
    }

    CNPart(const char *component, size_t size, bool createMemory) :
            _component(component), _size(size), _createMemory(createMemory) {
        if (createMemory) {
            auto tmp = new char[size];
#ifdef HRC_CONTENTNAME_DEBUG
            click_chatter("create mem at %p (%d)", tmp, size);
#endif
            memcpy(tmp, _component, size);
            _component = const_cast<const char *>(tmp);
        }
    }

    CNPart(const char *component, bool createMemory) : CNPart(component, getComponentSize(component), createMemory) {}

    CNPart(const cnParse_t *parseResult, size_t part, bool createMemory) :
            CNPart(parseResult->components[part], hrc_cn_get_part_size(parseResult, part), createMemory) {}

    CNPart(const CNPart &part, bool createMemory = true) : CNPart(part._component, part._size, createMemory) {}

    ~CNPart() {
        if (unlikely(_createMemory)) {
#ifdef HRC_CONTENTNAME_DEBUG
            click_chatter("delete mem at %p (%d)", _component, _size);
#endif
            delete[] _component;
        }
    }

    inline const char *get_component() const { return _component; }

    inline size_t get_size() const { return _size; }

    inline size_t get_hash() const {
        size_t hash = 5381;
        for (size_t i = 0; i < _size; i++)hash = ((hash << 5UL) + hash) + _component[i];
        return hash;
    }

    inline bool operator==(const CNPart &rhs) const {
        if (_size != rhs._size) return false;
        auto ret = strncmp(_component, rhs._component, _size);
        return static_cast<bool>(!ret);
    }

    inline bool operator!=(const CNPart &rhs) const {
        return !(rhs == *this);
    }

    inline bool operator==(const char *rhs) const {
        auto ret = strncmp(_component, rhs, _size);
        if (ret) return false;
        return rhs[_size] == '\0' || rhs[_size] == NAME_SEPARATOR;
    }

    inline bool operator!=(const char *rhs) const {
        return !(*this == rhs);
    }

    inline String unparse() const {
        auto str = String::make_uninitialized(_size);
        if (auto x = str.mutable_c_str())
            memcpy(x, _component, _size);
        return str;
    }

    inline bool hasMemory() const { return _createMemory; }

private:
    const char *_component;
    size_t _size;
    bool _createMemory;
};

namespace std {
    template<>
    struct hash<CNPart> {
        size_t operator()(const CNPart &guid) const { return hash<size_t>()(guid.get_hash()); }
    };
}


CLICK_ENDDECLS

#endif //CLICK_HRC_CONTENTNAME_H
