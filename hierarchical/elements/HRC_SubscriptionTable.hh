//
// Created by jiachen on 10/3/18.
//

#ifndef CLICK_HRC_SUBSCRIPTIONTABLE_HH
#define CLICK_HRC_SUBSCRIPTIONTABLE_HH

#include <click/config.h>
#include <click/glue.hh>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "HRC_ContentName.hh"

CLICK_DECLS

//#define HRC_SUBSCRIPTIONTABLEDEBUG

template<typename T>
class HRC_SubscriptionTableEntry {
public:
    HRC_SubscriptionTableEntry() = default;

    ~HRC_SubscriptionTableEntry() {
        for (auto &it : _children) {
#ifdef HRC_SUBSCRIPTIONTABLEDEBUG
            click_chatter("DELETE CHILD");
#endif
            delete it.second;
        }
    }

    inline bool hasVal() const { return !_vals.empty(); }

    inline const std::unordered_set<T> &getVal() const { return _vals; }

    inline bool isLeaf() const { return _children.empty(); }

    inline size_t getChildrenSize() const { return _children.size(); }

    // returns true "this" is empty (leaf && no value) after removal
    inline bool remove(const char *name) {
        if (*name == NAME_SEPARATOR) name++;
        if (*name == '\0') {
            clearVal();
            return isLeaf();
        } else {
            CNPart part(name, false);
            name += part.get_size();
            auto it = _children.find(part);
            // no such child
            if (it == _children.end()) return false;
            if (it->second->remove(name)) { // if child becomes empty
#ifdef HRC_SUBSCRIPTIONTABLEDEBUG
                click_chatter("DELETE CHILD");
#endif
                delete it->second;
                _children.erase(it);
                return isLeaf() && !hasVal();
            } else {
                return false;
            }
        }
    }

    // returns true "this" is empty (leaf && no value) after removal
    inline bool remove(const char *name, const T &val) {
        if (*name == NAME_SEPARATOR) name++;
        if (*name == '\0') {
            removeVal(val);
            return isLeaf() && !hasVal();
        } else {
            CNPart part(name, false);
            name += part.get_size();
            auto it = _children.find(part);
            // no such child
            if (it == _children.end()) return false;
            if (it->second->remove(name, val)) { // if child becomes empty
#ifdef HRC_SUBSCRIPTIONTABLEDEBUG
                click_chatter("DELETE CHILD");
#endif
                delete it->second;
                _children.erase(it);
                return isLeaf() && !hasVal();
            } else {
                return false;
            }
        }
    }

    inline bool insert(const char *name, const T &val) {
        if (*name == NAME_SEPARATOR) name++;
        if (*name == '\0') {
            return insertVal(val);
        } else {
            CNPart part(name, false);
            name += part.get_size();
            auto &child = _children[part];
            if (!child) {
#ifdef HRC_SUBSCRIPTIONTABLEDEBUG
                click_chatter("NEW CHILD");
#endif
                child = new HRC_SubscriptionTableEntry<T>();
            }
            return child->insert(name, val);
        }
    }

    inline const std::unordered_set<T> *exactMatch(const char *name) const {
        if (*name == NAME_SEPARATOR) name++;
        if (*name == '\0') {
            return &_vals;
        } else {
            CNPart part(name, false);
            name += part.get_size();
            auto it = _children.find(part);
            // no such child
            if (it == _children.end()) return nullptr;
            else return it->second->exactMatch(name);
        }
    }

    inline void lookup(const char *name, std::unordered_set<T> &result) const {
        result.insert(_vals.begin(), _vals.end());
        if (*name == NAME_SEPARATOR) name++;
        if (*name != '\0') {
            CNPart part(name, false);
            name += part.get_size();
            auto it = _children.find(part);
            if (it != _children.end())
                it->second->lookup(name, result);
        }
    }

    inline void forEach(std::string prefix, void (*fun)(const std::string &, const std::unordered_set<T> &)) const {
        if (hasVal()) fun(prefix, _vals);
        for (const auto &it : _children) {
            auto newPrefix = prefix + it.first.unparse().c_str() + "/";
            it.second->forEach(newPrefix, fun);
        }
    }


private:
    inline bool insertVal(const T &t) { return _vals.insert(t).second; }

    inline void removeVal(const T &t) { _vals.erase(t); }

    inline void clearVal() { _vals.clear(); }

    std::unordered_map<CNPart, HRC_SubscriptionTableEntry<T> *> _children;
    std::unordered_set<T> _vals;
};

template<typename T>
class HRC_SubscriptionTable {
public:
    HRC_SubscriptionTable() = default;

    ~HRC_SubscriptionTable() = default;

    inline bool remove(const char *name) { return _root.remove(name); }

    inline bool remove(const char *name, const T &val) { return _root.remove(name, val); }

    inline bool insert(const char *name, const T &val) { return _root.insert(name, val); }

    inline const std::unordered_set<T> lookup(const char *name) const {
        std::unordered_set<T> ret;
        _root.lookup(name, ret);
        return ret;
    }

    inline const std::unordered_set<T> *exactMatch(const char *name) const { return _root.exactMatch(name); }

    inline void forEach(void (*fun)(const std::string &, const std::unordered_set<T> &)) const {
        _root.forEach("/", fun);
    }

private:
    HRC_SubscriptionTableEntry<T> _root;
};

CLICK_ENDDECLS

#endif //CLICK_HRC_SUBSCRIPTIONTABLE_HH
