//
// Created by ubuntu on 9/20/18.
//

#include "GPS_RoutingTable.hh"
#include "GPS_Helper.hh"
#include "GPS_PacketHeader.hh"
#include "GPS_PacketAnno.h"
#include <click/args.hh>
#include <click/error.hh>
#include <click/straccum.hh>

CLICK_DECLS

//#define GPS_ROUTINGTABLE_DEBUG


GPS_RoutingTable::GPS_RoutingTable() = default;

GPS_RoutingTable::~GPS_RoutingTable() = default;

int GPS_RoutingTable::configure(Vector<String> &conf, ErrorHandler *errh) {
    String fileName;
    if (Args(conf, this, errh)
                .read_mp("FILENAME", FilenameArg(), fileName)
                .complete() < 0) {
        return -1;
    }

    if (parseArgFile(fileName, errh, _naNextHops) < 0) return -1;

    {
        for (auto &it1 : _naNextHops) {
            StringAccum sa;

            for (auto &it2 : *it1.second.getCandidates()) {
                sa.append(" ");
                sa.append(gps_na_unparse(&it2.first).c_str());
                sa.append("|");
                sa.append_numeric(static_cast<String::uintmax_t>(it2.second), 10, false);
            }
            errh->debug("%s -> [%s|%d]%s",
                        gps_na_unparse(&it1.first).c_str(),
                        gps_na_unparse(it1.second.getMinNextHop()).c_str(),
                        it1.second.getMinDistance(),
                        sa.c_str()
            );
        }
    }


    return 0;
}

void GPS_RoutingTable::push(int port, Packet *p) {
    switch (port) {
        case IN_PORT_DATA:
            handleData(p);
            break;
        default:
            // from other ports?? should not reach here!
            click_chatter("[GPS_RoutingTable::push] Got packet from unexpected port: %d, discarding...", port);
            p->kill();
            break;
    }
}

int GPS_RoutingTable::parseArgFile(const String &fileName, ErrorHandler *errh,
                                   std::unordered_map<gps_na_t, GPS_RoutingTableEntry> &naNextHops) {
    auto fp = fopen(fileName.c_str(), "r");
    if (fp == nullptr) {
        errh->error("Cannot read file %s", fileName.c_str());
        return -1;
    }

    char *line = nullptr, *tmp;
    size_t len = 0, lineNum = 0;
    ssize_t pos;

    IntArg numArg;
    uint32_t numDstNa = 0, numNextHopNa = 0, distance = 0;
    gps_na_t dstNa, nextHopNa;


    while (getline(&line, &len, fp) != -1) {
        lineNum++;
#ifdef GPS_ROUTINGTABLE_DEBUG
        errh->debug(">>>>>>>>>>line: %d<<<<<<<<<<", lineNum);
        errh->debug("line: %s", line);
#endif

        tmp = line;
        pos = skipEmpty(tmp);
        //empty line
        if (pos == -1) continue;

        tmp += pos;
#ifdef GPS_ROUTINGTABLE_DEBUG
        errh->debug("skip empty pos: %d, line: %s", pos, tmp);
#endif
        // comment skip
        if (*tmp == '#') continue;

        // dstNa
        pos = getPart(tmp);
#ifdef GPS_ROUTINGTABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!numArg.parse(tmp, numDstNa)) {
            errh->error("Skip line %d: cannot parse dstNA.", lineNum);
            continue;
        }
        gps_na_set_val(&dstNa, numDstNa);
#ifdef GPS_ROUTINGTABLE_DEBUG
        errh->debug("dstNa: %s", gps_na_unparse(&dstNa).c_str());
#endif
        tmp += pos;

        // nextHopNa
#ifdef GPS_ROUTINGTABLE_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            errh->error("Skip line %d: cannot find nextHopNa.", lineNum);
            continue;
        }
        tmp += pos;
        pos = getPart(tmp);
#ifdef GPS_ROUTINGTABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!numArg.parse(tmp, numNextHopNa)) {
            errh->error("Skip line %d: cannot parse nextHopNa.", lineNum);
            continue;
        }
        gps_na_set_val(&nextHopNa, numNextHopNa);
#ifdef GPS_ROUTINGTABLE_DEBUG
        errh->debug("nextHopNa: %s", gps_na_unparse(&nextHopNa).c_str());
#endif
        tmp += pos;

        // distance
#ifdef GPS_ROUTINGTABLE_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            errh->error("Skip line %d: cannot find distance.", lineNum);
            continue;
        }
        tmp += pos;
        pos = getPart(tmp);
#ifdef GPS_ROUTINGTABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!numArg.parse(tmp, distance)) {
            errh->error("Skip line %d: cannot parse distance.", lineNum);
            continue;
        }
#ifdef GPS_ROUTINGTABLE_DEBUG
        errh->debug("distance: %d", distance);
#endif
        tmp += pos;
        (void) tmp;

        naNextHops[dstNa].insertValue(&nextHopNa, distance);
    }
    for (auto &it : naNextHops) {
        it.second.updateMinimal();
    }

    fclose(fp);
    if (line) free(line);


    return 0;
}

void GPS_RoutingTable::handleData(Packet *p) {

    auto header = p->data();
    if (unlikely(!gps_packet_is_application(header))) {
        click_chatter("[GPS_RoutingTable::handleData] Error packet type: 0x%x, should match mask: 0x%x", gps_packet_get_type(p),
                      GPS_PACKET_TYPE_APPLICATION_MASK);
        p->kill();
        return;
    }

    auto dstNa = gps_packet_application_get_dstNa(header);


    if (unlikely(gps_na_is_broadcast(dstNa))) {
        gps_na_set_broadcast(NEXT_HOP_NA_ANNO(p));
        checked_output_push(OUT_PORT_DATA, p);
    } else {
        auto it = _naNextHops.find(*dstNa);
        if (unlikely(it == _naNextHops.end())) {
            // cannot find routing, discard
            checked_output_push(OUT_PORT_DISCARD, p);
        } else {
            gps_na_set_val(NEXT_HOP_NA_ANNO(p), it->second.getMinNextHop());
            checked_output_push(OUT_PORT_DATA, p);
        }
    }

}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_RoutingTable)