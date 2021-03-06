//
// Created by root on 1/9/19.
//

#include "pubgenerator.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/etheraddress.hh>
#include <clicknet/ether.h>

CLICK_DECLS

#define USE_COUNT


PubGenerator::PubGenerator() : _pkt(nullptr), _pktCount(0) {}

PubGenerator::~PubGenerator() {
    if (_pkt) _pkt->kill();
}



Packet *PubGenerator::pull(int) {
    _pktCount++;
#ifdef USE_COUNT
    if (_pktCount == _reqCount) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &_end);
        uint64_t delta_ns = (_end.tv_sec - _start.tv_sec) * 1000000000 + (_end.tv_nsec - _start.tv_nsec);
        click_chatter("took: %luns", delta_ns);
    }
#endif
    return _pkt->clone();
}

int PubGenerator::configure(Vector<String> &conf, ErrorHandler *errh) {
    EtherAddress src, dst;
    uint32_t size;
    uint16_t type;
    _reqCount = UINT64_MAX;
    if (Args(conf, this, errh)
                .read_mp("SRC", src)
                .read_mp("DST", dst)
                .read_mp("SIZE", size)
                .read_mp("TYPE", type)
                .read_p("COUNT", _reqCount)
                .complete() < 0)
        return -1;
#ifdef USE_COUNT
	errh->debug("count=%" PRIu64, _reqCount);
#endif


    if (size < sizeof(click_ether)) {
        errh->error("size = %d < min ether size (%d)", size, sizeof(click_ether));
        return -1;
    }

    errh->debug("src=%s, dst=%s, size:%d", src.unparse_colon().c_str(), dst.unparse_colon().c_str(), size);

    auto pkt = Packet::make(size);
    if (!pkt) {
        errh->error("Cannot create packet.");
        return -2;
    }
    auto eth = reinterpret_cast<click_ether *>(pkt->data());
    memcpy(eth->ether_shost, src.data(), 6);
    memcpy(eth->ether_dhost, dst.data(), 6);
    eth->ether_type = htons(type);

    auto tmpPayloadBuf = new char[3 * size];
    for (decltype(size) i = 0; i < size; ++i)
        snprintf(tmpPayloadBuf + i * 3, 4, "%02x ", pkt->data()[i]);
    tmpPayloadBuf[3 * size - 1] = '\0';
    errh->debug("PKT: %s", tmpPayloadBuf);
    delete[] tmpPayloadBuf;


    _pkt = pkt;


    return 0;
}

void PubGenerator::add_handlers() {
    add_data_handlers("packetCount", Handler::f_read, &_pktCount);
}

int PubGenerator::initialize(ErrorHandler *) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &_start);
    return 0;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(PubGenerator)
