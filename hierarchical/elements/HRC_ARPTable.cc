//
// Created by jiachen on 9/20/18.
//

#include "HRC_ARPTable.hh"
#include "HRC_Helper.hh"
#include "HRC_PacketAnno.hh"
#include "HRC_PacketHeader.hh"
#include <click/error.hh>
#include <click/args.hh>

CLICK_DECLS

//#define HRC_ARPTABLE_DEBUG

HRC_ARPTable::HRC_ARPTable() = default;

HRC_ARPTable::~HRC_ARPTable() = default;

int HRC_ARPTable::parseArgFile(const String &fileName, ErrorHandler *errh,
                               std::unordered_map<hrc_na_t, HRC_NAAddress> &naAddresses,
                               std::unordered_map<HRC_NAAddress, hrc_na_t> &inverseNaAddresses) {
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
    hrc_na_t na;

    while (getline(&line, &len, fp) != -1) {
        lineNum++;
#ifdef HRC_ARPTABLE_DEBUG
        errh->debug(">>>>>>>>>>line: %d<<<<<<<<<<", lineNum);
        errh->debug("line: %s", line);
#endif

        tmp = line;
        pos = skipEmpty(tmp);
        //empty line
        if (pos == -1) continue;

        tmp += pos;
#ifdef HRC_ARPTABLE_DEBUG
        errh->debug("skip empty pos: %d, line: %s", pos, tmp);
#endif
        // comment skip
        if (*tmp == '#') continue;

        // na
        pos = getPart(tmp);
#ifdef HRC_ARPTABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!intArg.parse(tmp, numNa)) {
            errh->error("Skip line %d: cannot parse NA.", lineNum);
            continue;
        }
        hrc_na_set_val(&na, numNa);
#ifdef HRC_ARPTABLE_DEBUG
        errh->debug("na: %s", hrc_na_unparse(&na).c_str());
#endif
        tmp += pos;

        // port
#ifdef HRC_ARPTABLE_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            errh->error("Skip line %d: cannot find port.", lineNum);
            continue;
        }
        tmp += pos;
        pos = getPart(tmp);
#ifdef HRC_ARPTABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!intArg.parse(tmp, port)) {
            errh->error("Skip line %d: cannot parse port.", lineNum);
            continue;
        }
#ifdef HRC_ARPTABLE_DEBUG
        errh->debug("port: %d", port);
#endif
        tmp += pos;

        // ether
#ifdef HRC_ARPTABLE_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            errh->error("Skip line %d: cannot find ether.", lineNum);
            continue;
        }
        tmp += pos;
        pos = getPart(tmp);
#ifdef HRC_ARPTABLE_DEBUG
        errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
        if (!etherArg.parse(tmp, ether)) {
            errh->error("Skip line %d: cannot parse ether.", lineNum);
            continue;
        }
#ifdef HRC_ARPTABLE_DEBUG
        errh->debug("ether: %s", ether.unparse().c_str());
#endif
        tmp += pos;

        // ip
#ifdef HRC_ARPTABLE_DEBUG
        errh->debug("line: %s", tmp);
#endif
        pos = skipEmpty(tmp);
        if (pos == -1) {
            ip = IPAddress();
        } else {
            tmp += pos;
            pos = getPart(tmp);
#ifdef HRC_ARPTABLE_DEBUG
            errh->debug("get part pos: %d, line: %s", pos, tmp);
#endif
            if (!IPAddressArg::parse(tmp, ip)) {
                errh->error("Skip line %d: cannot parse ip.", lineNum);
                continue;
            }
#ifdef HRC_ARPTABLE_DEBUG
            errh->debug("ip: %s", ip.unparse().c_str());
#endif
            tmp += pos;
            (void) tmp;
        }

#ifdef HRC_ARPTABLE_DEBUG
        errh->debug("Entry: %s -> %d (%s, %s)", hrc_na_unparse(&na).c_str(), port, ether.unparse().c_str(),
                    ip.unparse().c_str());
#endif
        addNAAddress(na, HRC_NAAddress(port, ether, ip), naAddresses, inverseNaAddresses);
    }
    fclose(fp);
    if (line) free(line);
    return 0;
}

void HRC_ARPTable::addNAAddress(const hrc_na_t &na, const HRC_NAAddress &addr,
                                std::unordered_map<hrc_na_t, HRC_NAAddress> &naAddresses,
                                std::unordered_map<HRC_NAAddress, hrc_na_t> &inverseNaAddresses) {
    // find original bindings and remove
    auto it1 = naAddresses.find(na);
    if (it1 != naAddresses.end()) inverseNaAddresses.erase(it1->second);
    auto it2 = inverseNaAddresses.find(addr);
    if (it2 != inverseNaAddresses.end()) naAddresses.erase(it2->second);

    naAddresses[na] = addr;
    inverseNaAddresses[addr] = na;
}


int HRC_ARPTable::configure(Vector<String> &conf, ErrorHandler *errh) {
    String fileName;
    if (Args(conf, this, errh)
                .read_mp("FILENAME", FilenameArg(), fileName)
                .complete() < 0) {
        return -1;
    }
    if (parseArgFile(fileName, errh, _naAddresses, _inverseNaAddresses) < 0) return -1;

    for (auto &it:_naAddresses) {
        const HRC_NAAddress &val = it.second;
        errh->debug("  %s -> %d | %s | %s",
                    hrc_na_unparse(&it.first).c_str(),
                    val.getPort(),
                    val.getEther().unparse_colon().c_str(),
                    val.getIp().unparse().c_str());
    }
    return 0;
}


void HRC_ARPTable::push(int port, Packet *p) {
    switch (port) {
        case IN_PORT_OUT_DATA:
            handleOutData(p);
            break;
        case IN_PORT_IN_PACKET:
            handleInPacket(p);
            break;
        default:
            // from other ports?? should not reach here!
            click_chatter("[HRC_ARPTable::push] Got packet from unexpected port: %d, discarding...", port);
            p->kill();
            break;
    }
}

void HRC_ARPTable::handleOutData(Packet *packet) {
    auto na = HRC_ANNO_NEXT_HOP_NA(packet);

    if (unlikely(hrc_na_is_broadcast(na))) { // broadcast
        _lock.acquire_read();

        for (auto &it : _naAddresses) {
            auto p = packet->clone();
            auto &val = it.second;

            *reinterpret_cast<EtherAddress *> (HRC_ANNO_NEXT_HOP_ETHER(p)) = val.getEther();
            *reinterpret_cast<IPAddress *> (HRC_ANNO_NEXT_HOP_IP(p)) = val.getIp();
            *HRC_ANNO_NEXT_HOP_PORT(p) = static_cast<uint8_t>(val.getPort());
            checked_output_push(OUT_PORT_OUT_DATA, p);
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
            *reinterpret_cast<EtherAddress *> (HRC_ANNO_NEXT_HOP_ETHER(packet)) = val.getEther();
            *reinterpret_cast<IPAddress *> (HRC_ANNO_NEXT_HOP_IP(packet)) = val.getIp();
            *HRC_ANNO_NEXT_HOP_PORT(packet) = static_cast<uint8_t>(val.getPort());
            _lock.release_read();
            checked_output_push(OUT_PORT_OUT_DATA, packet);
        }
    }
}

void HRC_ARPTable::handleInPacket(Packet *packet) {

    auto header = packet->data();

//  if is lsa, update table with na in packet, ether and ip in anno. discard directly
    if (hrc_packet_get_type(header) == HRC_PACKET_TYPE_LSA) {
        click_chatter("This is LSA packet!");
//    _lock.acquire_write();
//
//    _lock.release_write();
        packet->kill();
    } else {
        auto ether = reinterpret_cast<EtherAddress *> (HRC_ANNO_PREV_HOP_ETHER(packet));
        auto ip = reinterpret_cast<IPAddress *> (HRC_ANNO_PREV_HOP_IP(packet));
        auto port = HRC_ANNO_PREV_HOP_PORT(packet);
        // else, check if it is from neighbor, if so, annotate srcNa, to OUT_PORT_IN_PACKET
        //             else to OUT_PORT_DISCARD
        click_chatter("eth=%s, ip=%s, port=%d", ether->unparse_colon().c_str(), ip->unparse().c_str(), *port);
        auto it = _inverseNaAddresses.find(HRC_NAAddress(*port, *ether, *ip));
        if (it == _inverseNaAddresses.end()) {
            click_chatter("Cannot find neighbor NA with port=%d, ether=%s, ip=%s, discard...", *port, ether->unparse_colon().c_str(), ip->unparse().c_str());
            checked_output_push(OUT_PORT_DISCARD, packet);
        } else {
            *HRC_ANNO_PREV_HOP_NA(packet) = it->second;
            checked_output_push(OUT_PORT_IN_PACKET, packet);
        }
    }
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_ARPTable)
