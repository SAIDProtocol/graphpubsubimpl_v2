//
// Created by root on 1/17/19.
//

#include "countedsink.hh"
#include <click/straccum.hh>

CLICK_DECLS

CountedSink::CountedSink() : _counts{}, _start{} {
    int i;
    for (i = 0; i < LISTEN_DURATION; i++) {
        if (_counts[i])
            click_chatter("%d=%" PRIu32, i, _counts[i]);
    }
    click_chatter("init value check finished!!");
    gettimeofday(&_start, nullptr);
}

CountedSink::~CountedSink() = default;

void CountedSink::push(int, Packet *p) {
    struct timeval now{};
    gettimeofday(&now, nullptr);
    _counts[now.tv_sec - _start.tv_sec] ++;
    p->kill();
}

void CountedSink::cleanup(Element::CleanupStage) {
    click_chatter("cleanup!!");
    struct timeval now{};
    gettimeofday(&now, nullptr);
    time_t diff = now.tv_sec - _start.tv_sec;
    StringAccum sa;
    for (time_t i = 0; i <= diff; i++) {
        sa << _counts[i] << ' ';
    }
    click_chatter("%s", sa.c_str());
}


CLICK_ENDDECLS

EXPORT_ELEMENT(CountedSink)