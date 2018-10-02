//
// Created by jiachen on 10/1/18.
//

#ifndef CLICK_HRC_INTERESTTABLE_HH
#define CLICK_HRC_INTERESTTABLE_HH

#include <click/config.h>
#include <click/glue.hh>
#include <string>
#include <unordered_map>
#include "HRC_ContentName.hh"

CLICK_DECLS

//#define HRC_CONTENTNAME_DEBUG

template<typename T>
class HRC_InterestTableEntry;

template<typename T>
class HRC_InterestTableEntry {
public:
    HRC_InterestTableEntry() : _val(nullptr) {}

    ~HRC_InterestTableEntry() {
        for (auto &it : _children) {
#ifdef HRC_CONTENTNAME_DEBUG
            click_chatter("DELETE CHILD");
#endif
            delete it.second;
        }
        if (_val) {
#ifdef HRC_CONTENTNAME_DEBUG
            click_chatter("DELETE VAL");
#endif
            delete _val;
        }
    }

    inline bool hasVal() const { return _val != nullptr; }

    inline T &getVal() const { return *_val; }

    inline bool isLeaf() const { return _children.empty(); }

    inline size_t getChildrenSize() const { return _children.size(); }

    // returns true "this" is empty (leaf && no value) after removal
    inline bool remove(const char *name) {
        if (*name == NAME_SEPARATOR) name++;
        if (*name == '\0') {
            removeVal();
            return _children.empty();
        } else {
            CNPart part(name, false);
            name += part.get_size();
            auto it = _children.find(part);
            // no such child
            if (it == _children.end()) return false;
            if (it->second->remove(name)) { // if child becomes empty
#ifdef HRC_CONTENTNAME_DEBUG
                click_chatter("DELETE CHILD");
#endif
                delete it->second;
                _children.erase(it);
                return _children.empty() && (!_val);
            }
            return false;
        }
    }

    // return true if a new name is added
    inline bool set(const char *name, T val) {
        if (*name == NAME_SEPARATOR) name++;
        if (*name == '\0') {
            return setVal(val);
        } else {
            CNPart part(name, false);
            name += part.get_size();
            auto &child = _children[part];
            if (!child) {
#ifdef HRC_CONTENTNAME_DEBUG
                click_chatter("NEW CHILD");
#endif
                child = new HRC_InterestTableEntry<T>();
            }
            return child->set(name, val);
        }
    }

    inline T *longestPrefixMatch(const char *name) const {
        if (*name == NAME_SEPARATOR) name++;
        if (*name == '\0') {
            return _val;
        } else {
            CNPart part(name, false);
            name += part.get_size();
            auto it = _children.find(part);
            // no such child, return my value
            if (it == _children.end()) return _val;
            auto ret = it->second->longestPrefixMatch(name);
            // if child has no value, return my value
            return ret ? ret : _val;
        }
    }

    inline T *exactMatch(const char *name) const {
        if (*name == NAME_SEPARATOR) name++;
        if (*name == '\0') {
            return _val;
        } else {
            CNPart part(name, false);
            name += part.get_size();
            auto it = _children.find(part);
            // no such child, return my value
            if (it == _children.end()) return nullptr;
            return it->second->exactMatch(name);
        }
    }

    inline void forEach(std::string prefix, void (*fun)(const std::string &, T &)) const {
        if (hasVal()) fun(prefix, *_val);
        for (const auto &it : _children) {
            std::string newPrefix = prefix + it.first.unparse().c_str() + "/";
            it.second->forEach(newPrefix, fun);
        }
    }

private:
    inline bool setVal(T &t) {
        if (_val) {
            *_val = t;
            return false;
        } else {
#ifdef HRC_CONTENTNAME_DEBUG
            click_chatter("NEW VAL");
#endif
            _val = new T(t);
            return true;
        }
    }

    inline void removeVal() {
        if (_val) {
            delete _val;
#ifdef HRC_CONTENTNAME_DEBUG
            click_chatter("DELETE VAL");
#endif
            _val = nullptr;
        }
    }

    std::unordered_map<CNPart, HRC_InterestTableEntry<T> *> _children;
    T *_val;
};

template<typename T>
class HRC_InterestTable {
public:
    HRC_InterestTable() = default;

    ~HRC_InterestTable() = default;

    inline bool remove(const char *name) { return _root.remove(name); }

    inline bool set(const char *name, T val) { return _root.set(name, val); }

    inline T *longestPrefixMatch(const char *name) const { return _root.longestPrefixMatch(name); }

    inline T *exactMatch(const char *name) const { return _root.exactMatch(name); }

    inline void forEach(void (*fun)(const std::string &, T &)) const { _root.forEach("/", fun); };


private:

    HRC_InterestTableEntry<T> _root;
};

CLICK_ENDDECLS
#endif //CLICK_HRC_INTERESTTABLE_HH
