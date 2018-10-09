//
// Created by jiachen on 10/5/18.
//

#include "HRC_SubscriptionTable.hh"
#include "HRC_Helper.hh"
#include "HRC_PacketAnno.hh"
#include "HRC_PacketHeader.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/straccum.hh>

CLICK_DECLS

//#define HRC_SUBSCRIPTIONTABLE_DEBUG

static ErrorHandler *tmpErrh;

static inline StringAccum &fillValueList(StringAccum &sa, const std::unordered_set<hrc_na_t> &val) {
    for (auto i : val) sa << ' ' << hrc_na_unparse(&i);
    return sa;
}

static void printMulticastTable(const std::string &prefix, const std::unordered_set<hrc_na_t> &val) {
    StringAccum sa;
    tmpErrh->debug("  %s -> (%p)%s", prefix.c_str(), &val, fillValueList(sa, val).c_str());
}

HRC_SubscriptionTable::HRC_SubscriptionTable() {
    tmpErrh = ErrorHandler::default_handler();
};

HRC_SubscriptionTable::~HRC_SubscriptionTable() = default;

int
HRC_SubscriptionTable::parseArgFile(const String &fileName, ErrorHandler *errh, HRC_MulticastTable<hrc_na_t> &table) {
    auto fp = fopen(fileName.c_str(), "r");
    if (fp == nullptr) {
        errh->error("Cannot read file %s", fileName.c_str());
        return -1;
    }

    char *line = nullptr, *tmp;
    size_t len = 0, lineNum = 0;
    ssize_t pos;

    IntArg intArg;
    uint32_t numNa;
    hrc_na_t na;
    const char *name;

    while (getline(&line, &len, fp) != -1) {
        lineNum++;
#ifdef HRC_SUBSCRIPTIONTABLE_DEBUG
        errh->debug(">>>>>>>>>>line: %d<<<<<<<<<<", lineNum);
        errh->debug("line: %s", line);
#endif
        tmp = line;
        pos = skipEmpty(tmp);
        //empty line
        if (pos == -1) continue;

        tmp += pos;
#ifdef HRC_SUBSCRIPTIONTABLE_DEBUG
        errh->debug("skip empty pos: %d, line: %s", pos, tmp);
#endif
        // comment skip
        if (*tmp == '#') continue;

        // name
        pos = getPart(tmp);
#ifdef HRC_SUBSCRIPTIONTABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        name = tmp;
#ifdef HRC_SUBSCRIPTIONTABLE_DEBUG
        errh->debug("name: %s", name);
#endif
        tmp += pos;

        // na
#ifdef HRC_SUBSCRIPTIONTABLE_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            errh->error("Skip line %d: cannot find na.", lineNum);
            continue;
        }
        tmp += pos;
        pos = getPart(tmp);
#ifdef HRC_SUBSCRIPTIONTABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!intArg.parse(tmp, numNa)) {
            errh->error("Skip line %d: cannot parse na.", lineNum);
            continue;
        }
        hrc_na_set_val(&na, numNa);
#ifdef HRC_SUBSCRIPTIONTABLE_DEBUG
        errh->debug("na: %s", hrc_na_unparse(&na).c_str());
#endif
        tmp += pos;
        (void) tmp;

#ifdef HRC_SUBSCRIPTIONTABLE_DEBUG
        errh->debug("Entry: %s -> %s", name, hrc_na_unparse(&na).c_str());
#endif
        table.insert(name, na);
    }
    fclose(fp);
    if (line) free(line);
    return 0;
}

int HRC_SubscriptionTable::configure(Vector<String> &conf, ErrorHandler *errh) {
    String fileName;
    if (Args(conf, this, errh)
                .read_mp("FILENAME", FilenameArg(), fileName)
                .complete() < 0) {
        return -1;
    }
    if (parseArgFile(fileName, errh, _table) < 0) return -1;

    {
        tmpErrh = errh;
        _table.forEach(printMulticastTable);
        tmpErrh = ErrorHandler::default_handler();
    }
    return 0;
}

void HRC_SubscriptionTable::push(int port, Packet *p) {
    switch (port) {
        case IN_PORT_PUBLICATION:
            handlePublication(p);
            break;
        case IN_PORT_SUBSCRIPTION:
            handleSubscription(p);
            break;
        default:
            // from other ports?? should not reach here!
            tmpErrh->debug("[HRC_SubscriptionTable::push] Got packet from unexpected port: %d, discarding...", port);
            p->kill();
            break;
    }
}

void HRC_SubscriptionTable::handlePublication(Packet *packet) {
    auto header = packet->data();
    auto type = hrc_packet_get_type(header);

    if (unlikely(type != HRC_PACKET_TYPE_PUBLICATION)) {
        ErrorHandler::default_handler()->debug(
                "[HRC_SubscriptionTable::handlePublication] Error packet type (%02x). Should be publication (%02x)",
                type, HRC_PACKET_TYPE_PUBLICATION);
        packet->kill();
    } else {
        auto name = hrc_packet_publication_get_name(header);
        _lock.acquire_read();
        auto res = _table.lookup(name);
        _lock.release_read();
        // remove prev hop
        res.erase(*HRC_ANNO_PREV_HOP_NA(packet));
        if (res.empty()) {
            checked_output_push(OUT_PORT_DISCARD, packet);
        } else {
            for (auto &next : res) {
                auto wp = packet->clone();
                hrc_na_set_val(HRC_ANNO_NEXT_HOP_NA(wp), &next);
                checked_output_push(OUT_PORT_PUBLICATION, wp);
            }
            packet->kill();
        }
    }

}

void HRC_SubscriptionTable::handleSubscription(Packet *packet) {
    auto header = packet->data();
    auto type = hrc_packet_get_type(header);

    if (unlikely(type != HRC_PACKET_TYPE_SUBSCRIPTION)) {
        ErrorHandler::default_handler()->debug(
                "[HRC_SubscriptionTable::handleSubscription] Error packet type (%02x). Should be subscription (%02x)",
                type, HRC_PACKET_TYPE_SUBSCRIPTION);
        packet->kill();
    } else {
        ErrorHandler::default_handler()->debug("Processing of subscription");
        packet->kill();
    }
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_SubscriptionTable)