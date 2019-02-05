//
// Created by root on 2/5/19.
//

#include "pushpubgenerator.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/etheraddress.hh>
#include <click/standard/scheduleinfo.hh>
#include <clicknet/ether.h>
#include <sys/time.h>
#include <clocale>


CLICK_DECLS

PushPubGenerator::PushPubGenerator() : _pkt(nullptr), _pktCount(0), _task(this) {}

PushPubGenerator::~PushPubGenerator() {
    if (_pkt) _pkt->kill();
}

bool PushPubGenerator::run_task(Task *task) {

    push(0, _pkt->clone());
    _pktCount++;
    task->fast_reschedule();
    return true;

}

int PushPubGenerator::configure(Vector<String> &conf, ErrorHandler *errh) {
    setlocale(LC_NUMERIC, "");
    EtherAddress src, dst;
    uint32_t size;
    uint16_t type;
    if (Args(conf, this, errh)
                .read_mp("SRC", src)
                .read_mp("DST", dst)
                .read_mp("SIZE", size)
                .read_mp("TYPE", type)
                .complete() < 0)
        return -1;

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

int PushPubGenerator::initialize(ErrorHandler *errh) {
    gettimeofday(&_start, nullptr);
    ScheduleInfo::initialize_task(this, &_task, errh);
    return 0;
}

void PushPubGenerator::cleanup(Element::CleanupStage) {
    gettimeofday(&_end, nullptr);
    printf("count=%'" PRIu64 ", duration=%'" PRIu64 "\n", _pktCount,
                  (_end.tv_sec - _start.tv_sec) * 1000000 + (_end.tv_usec - _start.tv_usec));
}

CLICK_ENDDECLS
EXPORT_ELEMENT(PushPubGenerator)