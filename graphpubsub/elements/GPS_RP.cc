//
// Created by ubuntu on 9/21/18.
//

#include "GPS_RP.hh"
#include "GPS_Helper.hh"
#include "GPS_PacketHeader.hh"
#include <click/error.hh>
#include <click/args.hh>
#include <click/straccum.hh>
#include <deque>

CLICK_DECLS
//#define GPS_RP_DEBUG

GPS_RP::GPS_RP() = default;

GPS_RP::~GPS_RP() = default;

int GPS_RP::configure(Vector<String> &conf, ErrorHandler *errh) {
    String fileName;
    uint32_t numMyNa;
    if (Args(conf, this, errh)
                .read_mp("FILENAME", FilenameArg(), fileName)
                .read_mp("MY_NA", numMyNa)
                .complete() < 0) {
        return -1;
    }
    if (parseArgFile(fileName, errh) < 0) return -1;
    gps_na_set_val(&_myNa, numMyNa);

    {
        errh->debug("myNa=%s", gps_na_unparse(&_myNa).c_str());
        for (auto &it : _mappingTable) {
            StringAccum sa;
            for (auto &it2 : it.second) {
                sa.append(" ");
                sa.append(gps_guid_unparse(&it2).c_str());
            }
            errh->debug("%s ->%s", gps_guid_unparse(&it.first).c_str(), sa.c_str());
        }
    }

    return 0;
}

void GPS_RP::push(int port, Packet *p) {
    switch (port) {
        case IN_PORT_DATA:
            handleData(p);
            break;
        default:
            // from other ports?? should not reach here!
            click_chatter("[GPS_RP::push] Got packet from unexpected port: %d, discarding...", port);
            p->kill();
            break;
    }
}

void GPS_RP::handleData(Packet *packet) {
    auto header = packet->data();
    auto type = gps_packet_get_type(header);
    if (unlikely(type != GPS_PACKET_TYPE_PUBLICATION)) {
        click_chatter("[GPS_RP::handleData] Error packet type (%02x). Should be publication (%02x)", type,
                      GPS_PACKET_TYPE_PUBLICATION);
        packet->kill();
    } else {
        auto dstGuid = gps_packet_publication_get_dstGuid(header);

        std::deque<const gps_guid_t *> todos;
        std::unordered_set<const gps_guid_t *> finished;

        _lock.acquire_read();

        todos.push_back(dstGuid);

        while (!todos.empty()) {
            auto todo = todos[0];
            todos.pop_front();

            // already passed, continue
            if (!finished.insert(todo).second) continue;

            auto writablePacket = packet->clone()->uniqueify();
            auto writableHeader = writablePacket->data();
            gps_packet_publication_set_dstGuid(writableHeader, todo);


            // find children
            auto it = _mappingTable.find(*todo);

            if (it == _mappingTable.end()) { // cannot find it, send it to another RP
                gps_na_clear(gps_packet_publication_get_srcNa(writableHeader));
                gps_na_clear(gps_packet_publication_get_dstNa(writableHeader));
                checked_output_push(OUT_PORT_TO_OTHER_RP, writablePacket);
            } else { // found it, send it via subscription table
                gps_packet_publication_set_srcNa(writableHeader, &_myNa);
                checked_output_push(OUT_PORT_TO_SUBSCRIBERS, writablePacket);

                for (auto &it2 : it->second) todos.push_back(&it2);
            }
            finished.insert(todo);
        }

        _lock.release_read();
    }

}

int GPS_RP::parseArgFile(const String &fileName, ErrorHandler *errh) {
    auto fp = fopen(fileName.c_str(), "r");
    if (fp == nullptr) {
        errh->error("Cannot read file %s", fileName.c_str());
        return -1;
    }

    char *line = nullptr, *tmp;
    size_t len = 0, lineNum = 0;
    ssize_t pos;

    IntArg numArg;
    uint32_t numParentGuid, numChildGuid;
    gps_guid_t parentGuid, childGuid;

    while (getline(&line, &len, fp) != -1) {
        lineNum++;
#ifdef GPS_RP_DEBUG
        errh->debug(">>>>>>>>>>line: %d<<<<<<<<<<", lineNum);
        errh->debug("line: %s", line);
#endif

        tmp = line;
        pos = skipEmpty(tmp);
        //empty line
        if (pos == -1) continue;

        tmp += pos;
#ifdef GPS_RP_DEBUG
        errh->debug("skip empty pos: %d, line: %s", pos, tmp);
#endif
        // comment skip
        if (*tmp == '#') continue;

        // parent guid
        pos = getPart(tmp);
#ifdef GPS_RP_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!numArg.parse(tmp, numParentGuid)) {
            errh->error("Skip line %d: cannot parse parent GUID.", lineNum);
            continue;
        }
        gps_guid_set_val(&parentGuid, numParentGuid);
#ifdef GPS_RP_DEBUG
        errh->debug("parent GUID: %s", gps_guid_unparse(&parentGuid).c_str());
#endif
        tmp += pos;

        // child guid
#ifdef GPS_RP_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            // if not exist, add a new entry with empty childGuids
            _mappingTable[parentGuid];
            continue;
        }
        tmp += pos;
        pos = getPart(tmp);
#ifdef GPS_RP_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!numArg.parse(tmp, numChildGuid)) {
            errh->error("Skip line %d: cannot parse child GUID.", lineNum);
            continue;
        }
        gps_guid_set_val(&childGuid, numChildGuid);
#ifdef GPS_RP_DEBUG
        errh->debug("child GUID: %s", gps_guid_unparse(&childGuid).c_str());
#endif
        tmp += pos;
        (void) tmp;

#ifdef GPS_RP_DEBUG
        errh->debug("Entry: %s -> %s", gps_guid_unparse(&parentGuid).c_str(), gps_guid_unparse(&childGuid).c_str());
#endif
        _mappingTable[parentGuid].insert(childGuid);
    }


    return 0;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_RP)
