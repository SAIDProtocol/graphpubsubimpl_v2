//
// Created by jiachen on 9/20/18.
//

#include "GPS_SubscriptionTable.hh"
#include "GPS_PacketHeader.hh"
#include "GPS_Helper.hh"
#include "GPS_PacketAnno.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/straccum.hh>

CLICK_DECLS

//#define GPS_SUBSCRIPTIONABLE_DEBUG

GPS_SubscriptionTable::GPS_SubscriptionTable() = default;

GPS_SubscriptionTable::~GPS_SubscriptionTable() = default;

int GPS_SubscriptionTable::parseArgFile(const String &fileName, ErrorHandler *errh,
                                        std::unordered_map<gps_guid_t, std::unordered_set<gps_na_t>> &subscriptions) {
    auto fp = fopen(fileName.c_str(), "r");
    if (fp == nullptr) {
        errh->error("Cannot read file %s", fileName.c_str());
        return -1;
    }

    char *line = nullptr, *tmp;
    size_t len = 0, lineNum = 0;
    ssize_t pos;

    IntArg numArg;
    uint32_t numGuid = 0, numNa = 0;
    gps_guid_t guid;
    gps_na_t na;


    while (getline(&line, &len, fp) != -1) {
        lineNum++;
#ifdef GPS_SUBSCRIPTIONABLE_DEBUG
        errh->debug(">>>>>>>>>>line: %d<<<<<<<<<<", lineNum);
        errh->debug("line: %s", line);
#endif

        tmp = line;
        pos = skipEmpty(tmp);
        //empty line
        if (pos == -1) continue;

        tmp += pos;
#ifdef GPS_SUBSCRIPTIONABLE_DEBUG
        errh->debug("skip empty pos: %d, line: %s", pos, tmp);
#endif
        // comment skip
        if (*tmp == '#') continue;

        // guid
        pos = getPart(tmp);
#ifdef GPS_SUBSCRIPTIONABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!numArg.parse(tmp, numGuid)) {
            errh->error("Skip line %d: cannot parse GUID.", lineNum);
            continue;
        }
        gps_guid_set_val(&guid, numGuid);
#ifdef GPS_SUBSCRIPTIONABLE_DEBUG
        errh->debug("guid: %s", gps_guid_unparse(&guid).c_str());
#endif
        tmp += pos;

        // na
#ifdef GPS_SUBSCRIPTIONABLE_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            errh->error("Skip line %d: cannot find NA.", lineNum);
            continue;
        }
        tmp += pos;
        pos = getPart(tmp);
#ifdef GPS_SUBSCRIPTIONABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!numArg.parse(tmp, numNa)) {
            errh->error("Skip line %d: cannot parse NA.", lineNum);
            continue;
        }
        gps_na_set_val(&na, numNa);
#ifdef GPS_SUBSCRIPTIONABLE_DEBUG
        errh->debug("na: %s", gps_na_unparse(&na).c_str());
#endif
        tmp += pos;
        (void) tmp;

        subscriptions[guid].insert(na);
    }
    return 0;
}

int GPS_SubscriptionTable::configure(Vector<String> &conf, ErrorHandler *errh) {
    String fileName;

    if (Args(conf, this, errh)
                .read_mp("FILENAME", FilenameArg(), fileName)
                .complete() < 0) {
        return -1;
    }
    if (parseArgFile(fileName, errh, _subscriptions) < 0) return -1;

    {
        for (auto &it1 : _subscriptions) {
            StringAccum sa;
            for (auto &it2 : it1.second) {
                sa.append(" ");
                sa.append(gps_na_unparse(&it2).c_str());
            }
            errh->message("%s ->%s", gps_guid_unparse(&it1.first).c_str(), sa.c_str());
        }
    }

    return 0;
}

void GPS_SubscriptionTable::push(int port, Packet *p) {
    switch (port) {
        case IN_PORT_PUBLICATION:
            handlePublication(p);
            break;
        case IN_PORT_SUBSCRIPTION:
            handleSubscription(p);
            break;
        default:
            // from other ports?? should not reach here!
            click_chatter("[GPS_SubscriptionTable::push] Got packet from unexpected port: %d, discarding...", port);
            p->kill();
            break;
    }
}

void GPS_SubscriptionTable::handlePublication(Packet *packet) {
    //    Publication
    auto header = packet->data();
    auto type = gps_packet_get_type(header);
    //check type
    if (unlikely(type != GPS_PACKET_TYPE_PUBLICATION)) {
        // Error! got a packet that is not publication!
        click_chatter("[GPS_SubscriptionTable::handlePublication] Error packet type: 0x%x, should be publication: 0x%x",
                      type, GPS_PACKET_TYPE_PUBLICATION);
        packet->kill();
        return;
    }

    _lock.acquire_read();
    auto it = _subscriptions.find(*gps_packet_publication_get_dstGuid(header));
    if (unlikely(it == _subscriptions.end())) {
        // No subscribers for GUID,
        _lock.release_read();
        checked_output_push(OUT_PORT_PUBLICATION_DISCARD, packet);
    } else {

        auto hasSub = false;
        for (auto &it2 : it->second) {
            hasSub = true;
            auto toForward = packet->clone();
            //            click_chatter("p_in: %p, p_in.data: %p, packet: %p, anno: %p, data: %p",
            //                    packet, packet->data(), toForward, GPS_ANNO_NEXT_HOP_NA(toForward), toForward->data());
            gps_na_set_val(GPS_ANNO_NEXT_HOP_NA(toForward), &it2);
            checked_output_push(OUT_PORT_PUBLICATION, toForward);
        }

        _lock.release_read();

        if (likely(hasSub)) packet->kill();
        else checked_output_push(OUT_PORT_PUBLICATION_DISCARD, packet);
    }
}

void GPS_SubscriptionTable::handleSubscription(Packet *packet) {
    auto header = packet->data();
    auto type = gps_packet_get_type(header);
    if (unlikely(type != GPS_PACKET_TYPE_SUBSCRIPTION)) {
        // Error! got a packet that is not publication!
        click_chatter(
                "[GPS_SubscriptionTable::handleSubscription] Error packet type: 0x%x, should be subscription: 0x%x",
                type, GPS_PACKET_TYPE_SUBSCRIPTION);
        packet->kill();
        return;
    }
    _lock.acquire_write();
    // TODO: implement subscription logic
    _lock.release_write();
    packet->kill();
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_SubscriptionTable)
