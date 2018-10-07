//
// Created by jiachen on 9/26/18.
//

#include "HRC_FIB.hh"
#include "HRC_Helper.hh"
#include "HRC_PacketHeader.hh"
#include "HRC_PacketAnno.hh"
#include <click/args.hh>

//#define HRC_FIB_DEBUG

CLICK_DECLS

static ErrorHandler *tmpErrh;

static void printInterestTable(const std::string &prefix, const hrc_na_t &v) {
    tmpErrh->debug("  %s (%p) -> %s (%p)", prefix.c_str(), &prefix, hrc_na_unparse(&v).c_str(), &v);
}

HRC_FIB::HRC_FIB() {
    tmpErrh = ErrorHandler::default_handler();
}

HRC_FIB::~HRC_FIB() = default;

void HRC_FIB::push(int port, Packet *p) {
    switch (port) {
        case IN_PORT_DATA:
            handleInterest(p);
            break;
        case IN_PORT_SUBSCRIPTION:
            handleSubscription(p);
            break;
        case IN_PORT_ANNOUNCEMENT:
            handleAnnouncement(p);
            break;
        default:
            // from other ports?? should not reach here!
            tmpErrh->debug("[HRC_FIB::push] Got packet from unexpected port: %d, discarding...", port);
            p->kill();
            break;
    }
}

int HRC_FIB::parseArgFile(String &fileName, ErrorHandler *errh, HRC_InterestTable<hrc_na_t> &interestTable) {
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
#ifdef HRC_FIB_DEBUG
        errh->debug(">>>>>>>>>>line: %d<<<<<<<<<<", lineNum);
        errh->debug("line: %s", line);
#endif
        tmp = line;
        pos = skipEmpty(tmp);
        //empty line
        if (pos == -1) continue;

        tmp += pos;
#ifdef HRC_FIB_DEBUG
        errh->debug("skip empty pos: %d, line: %s", pos, tmp);
#endif
        // comment skip
        if (*tmp == '#') continue;

        // name
        pos = getPart(tmp);
#ifdef HRC_FIB_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        name = tmp;
#ifdef HRC_FIB_DEBUG
        errh->debug("name: %s", name);
#endif
        tmp += pos;

        // na
#ifdef HRC_FIB_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            errh->error("Skip line %d: cannot find na.", lineNum);
            continue;
        }
        tmp += pos;
        pos = getPart(tmp);
#ifdef HRC_FIB_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!intArg.parse(tmp, numNa)) {
            errh->error("Skip line %d: cannot parse na.", lineNum);
            continue;
        }
        hrc_na_set_val(&na, numNa);
#ifdef HRC_FIB_DEBUG
        errh->debug("na: %s", hrc_na_unparse(&na).c_str());
#endif
        tmp += pos;
        (void) tmp;

#ifdef HRC_FIB_DEBUG
        errh->debug("Entry: %s -> %s", name, hrc_na_unparse(&na).c_str());
#endif
        interestTable.set(name, na);
    }
    fclose(fp);
    if (line) free(line);
    return 0;
}

int HRC_FIB::configure(Vector<String> &conf, ErrorHandler *errh) {
    String fileName;
    uint32_t numMyNa;
    if (Args(conf, this, errh)
                .read_mp("FILENAME", FilenameArg(), fileName)
                .read_mp("NA", numMyNa)
                .complete() < 0) {
        return -1;
    }
    if (parseArgFile(fileName, errh, _interestTable) < 0) return -1;
    hrc_na_set_val(&_myNa, numMyNa);

    {
        tmpErrh = errh;
        _interestTable.forEach(printInterestTable);
        tmpErrh = ErrorHandler::default_handler();
    }
    return 0;
}

inline void HRC_FIB::forwardBasedOnName(Packet *packet, const char *name, bool usingAnno = false) {
    _lock.acquire_read();
    auto na = _interestTable.longestPrefixMatch(name);
    if (unlikely(usingAnno)) { // prioritize unicast over subscription
        delete[] name;
        *HRC_ANNO_DST_NAME(packet) = nullptr;
    }

    if (na) {
        hrc_na_set_val(HRC_ANNO_NEXT_HOP_NA(packet), na);
        int outPort = (*na == _myNa) ? OUT_PORT_TO_LOCAL : OUT_PORT_TO_OTHER_ROUTER;
        _lock.release_read();
        checked_output_push(outPort, packet);
    } else {
        _lock.release_read();
        checked_output_push(OUT_PORT_DISCARD, packet);
    }
}

void HRC_FIB::handleInterest(Packet *packet) {
    auto header = packet->data();
    auto type = hrc_packet_get_type(header);
    if (unlikely(type != HRC_PACKET_TYPE_INTEREST)) {
        tmpErrh->debug("[HRC_FIB::handleInterest] Error packet type (%02x). Should be interest (%02x)", type,
                       HRC_PACKET_TYPE_INTEREST);
        packet->kill();
    } else
        forwardBasedOnName(packet, hrc_packet_interest_get_name(header), false);
}

void HRC_FIB::handleSubscription(Packet *packet) {
    auto header = packet->data();
    auto type = hrc_packet_get_type(header);
    if (unlikely(type != HRC_PACKET_TYPE_SUBSCRIPTION)) {
        tmpErrh->debug("[HRC_FIB::handleSubscription] Error packet type (%02x). Should be subscription (%02x)", type,
                       HRC_PACKET_TYPE_SUBSCRIPTION);
        packet->kill();
    } else {
        forwardBasedOnName(packet, *HRC_ANNO_DST_NAME(packet), true);
    }
}

void HRC_FIB::handleAnnouncement(Packet *packet) {
    packet->kill();
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_FIB)

