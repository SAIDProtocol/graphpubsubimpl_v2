//
// Created by jiachen on 9/20/18.
//

#include "GPS_ARPTable.hh"
#include "GPS_GUID.hh"
#include "GPS_Helper.hh"
#include "GPS_PacketAnno.h"
#include <click/error.hh>
#include <click/args.hh>

CLICK_DECLS

//#define GPS_ARPTABLE_DEBUG

GPS_ARPTable::GPS_ARPTable() = default;

GPS_ARPTable::~GPS_ARPTable() = default;


int GPS_ARPTable::parseArgFile(const String &fileName, ErrorHandler *errh) {
    auto fp = fopen(fileName.c_str(), "r");
    if (fp == nullptr) {
        errh->error("Cannot read file %s", fileName.c_str());
        return -1;
    }

    char *line = nullptr, *tmp;
    size_t len = 0, lineNum = 0;
    ssize_t pos;

    IntArg intArg;
    EtherAddressArg etherArg;
    uint32_t numNa, port;
    EtherAddress ether;
    IPAddress ip;
    gps_na_t na;

    while (getline(&line, &len, fp) != -1) {
        lineNum++;
#ifdef GPS_ARPTABLE_DEBUG
        errh->debug(">>>>>>>>>>line: %d<<<<<<<<<<", lineNum);
        errh->debug("line: %s", line);
#endif

        tmp = line;
        pos = skipEmpty(tmp);
        //empty line
        if (pos == -1) continue;

        tmp += pos;
#ifdef GPS_ARPTABLE_DEBUG
        errh->debug("skip empty pos: %d, line: %s", pos, tmp);
#endif
        // comment skip
        if (*tmp == '#') continue;

        // na
        pos = getPart(tmp);
#ifdef GPS_ARPTABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!intArg.parse(tmp, numNa)) {
            errh->error("Skip line %d: cannot parse NA.", lineNum);
            continue;
        }
        gps_na_set_val(&na, numNa);
#ifdef GPS_ARPTABLE_DEBUG
        errh->debug("na: %s", gps_na_unparse(na).c_str());
#endif
        tmp += pos;

        // port
#ifdef GPS_ARPTABLE_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            errh->error("Skip line %d: cannot find port.", lineNum);
            continue;
        }
        tmp += pos;
        pos = getPart(tmp);
#ifdef GPS_ARPTABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!intArg.parse(tmp, port)) {
            errh->error("Skip line %d: cannot parse port.", lineNum);
            continue;
        }
#ifdef GPS_ARPTABLE_DEBUG
        errh->debug("port: %d", port);
#endif
        tmp += pos;

        // ether
#ifdef GPS_ARPTABLE_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            errh->error("Skip line %d: cannot find ether.", lineNum);
            continue;
        }
        tmp += pos;
        pos = getPart(tmp);
#ifdef GPS_ARPTABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!etherArg.parse(tmp, ether)) {
            errh->error("Skip line %d: cannot parse ether.", lineNum);
            continue;
        }
#ifdef GPS_ARPTABLE_DEBUG
        errh->debug("ether: %s", ether.unparse().c_str());
#endif
        tmp += pos;

        // ip
#ifdef GPS_ARPTABLE_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            ip = IPAddress::make_broadcast();
        } else {
            tmp += pos;
            pos = getPart(tmp);
#ifdef GPS_ARPTABLE_DEBUG
            errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
            if (!IPAddressArg::parse(tmp, ip)) {
                errh->error("Skip line %d: cannot parse ip.", lineNum);
                continue;
            }
#ifdef GPS_ARPTABLE_DEBUG
            errh->debug("ip: %s", ip.unparse().c_str());
#endif
            tmp += pos;
            (void)tmp;
        }

#ifdef GPS_ARPTABLE_DEBUG
        errh->debug("Entry: %s -> %d (%s, %s)", gps_na_unparse(na).c_str(), port, ether.unparse().c_str(),
                    ip.unparse().c_str());
#endif

        _naAddresses[na] = GPS_NAAddress(port, ether, ip);
    }
    fclose(fp);
    if (line) free(line);
    return 0;
}

int GPS_ARPTable::configure(Vector<String> &conf, ErrorHandler *errh) {
    String fileName;
    if (Args(conf, this, errh)
                .read_mp("FILENAME", FilenameArg(), fileName)
                .complete() < 0) {
        return -1;
    }
    if (parseArgFile(fileName, errh) < 0) return -1;

    for (auto &it:_naAddresses) {
        const GPS_NAAddress &val = it.second;
        errh->debug("%s -> %d | %s | %s",
                    gps_na_unparse(&it.first).c_str(),
                    val.getPort(),
                    val.getEther().unparse_colon().c_str(),
                    val.getIp().unparse().c_str());
    }
    return 0;
}


void GPS_ARPTable::push(int port, Packet *p) {
    switch (port) {
        case IN_PORT_DATA:
            handleData(p);
            break;
        case IN_PORT_LSA:
            handleLSA(p);
            break;
        default:
            // from other ports?? should not reach here!
            click_chatter("[GPS_ARPTable::push] Got packet from unexpected port: %d, discarding...", port);
            p->kill();
            break;
    }
}

void GPS_ARPTable::handleData(Packet *packet) {
    auto na = NEXT_HOP_NA_ANNO(packet);

    if (unlikely(gps_na_is_broadcast(na))) { // broadcast
        _lock.acquire_read();

        for (auto &it : _naAddresses) {
            auto p = packet->clone();
            auto &val = it.second;

            *reinterpret_cast<EtherAddress *> (NEXT_HOP_ETHER_ANNO(p)) = val.getEther();
            *reinterpret_cast<IPAddress *> (NEXT_HOP_IP_ANNO(p)) = val.getIp();
            *NEXT_HOP_PORT_ANNO(p) = static_cast<uint8_t>(val.getPort());
            checked_output_push(OUT_PORT_DATA, p);
        }
        _lock.release_read();
        packet->kill();
    } else {
        _lock.acquire_read();
        auto it = _naAddresses.find(*na);
        if (unlikely(it == _naAddresses.end())) { // no neighbor with NA
            _lock.release_read();
            checked_output_push(OUT_PORT_DISCARD, packet);
        } else {
            auto &val = it->second;
            *reinterpret_cast<EtherAddress *> (NEXT_HOP_ETHER_ANNO(packet)) = val.getEther();
            *reinterpret_cast<IPAddress *> (NEXT_HOP_IP_ANNO(packet)) = val.getIp();
            *NEXT_HOP_PORT_ANNO(packet) = static_cast<uint8_t>(val.getPort());
            _lock.release_read();
            checked_output_push(OUT_PORT_DATA, packet);
        }
    }
}

void GPS_ARPTable::handleLSA(Packet *p) {
    _lock.acquire_write();

    _lock.release_write();
    p->kill();
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_ARPTable)