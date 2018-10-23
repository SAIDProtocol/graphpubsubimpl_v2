//
// Created by root on 10/23/18.
//

#include "GPS_TEST_PrintCounter.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/straccum.hh>

CLICK_DECLS

GPS_TEST_PrintCounter::GPS_TEST_PrintCounter() : _cs(), _timer(this), _counter(0) {
    _cs.n = 2;
    _cs.keys[0] = 0;
    _cs.keys[1] = 1;
};

GPS_TEST_PrintCounter::~GPS_TEST_PrintCounter() = default;

int GPS_TEST_PrintCounter::configure(Vector<String> &conf, ErrorHandler *errh) {
    Vector<Element *> elements;
    if (Args(conf, this, errh)
                .read_p("INTERVAL", TimestampArg(), _interval)
                .read_all("ELEM", ElementArg(), elements)
                .complete() < 0)
        return -1;
    for (auto e : elements) {
        if (!strcmp(e->class_name(), "Counter")) {
            _elements.push_back(e);
        }
    }
    errh->debug("count: %d", _elements.size());

    return 0;
}

void GPS_TEST_PrintCounter::printCounts() {
    Timestamp ts = Timestamp::now() - _start;
    StringAccum sa;
    sa << (ts.sec() * 1000000000L + ts.nsec());
    for (auto ptr : _elements) {
        if (strcmp(ptr->class_name(), "Counter") != 0)
            continue;
        int ret;
        if ((ret = ptr->llrpc(CLICK_LLRPC_GET_COUNTS, &_cs))) {
            click_chatter("%s: err: %d", ptr->name().c_str(), ret);
            sa << "\t\t";
        } else {
            sa << '\t' << _cs.values[0] << '\t' << _cs.values[1];
//                click_chatter("%s: C:%d, BC:%d", ptr->name().c_str(), _cs.values[0], _cs.values[1]);
        }
    }
    click_chatter("%s", sa.c_str());
}

void GPS_TEST_PrintCounter::cleanup(Element::CleanupStage stage) {
    if (stage == CLEANUP_ROUTER_INITIALIZED) {
        printCounts();
    }
}


int GPS_TEST_PrintCounter::initialize(ErrorHandler *) {
    _start = Timestamp::now();
    _timer.initialize(this);
    click_chatter("INTERVAL: %s", _interval.unparse_interval().c_str());
    if (_interval) {
        Timestamp ts = _start + _interval * (++_counter);
        click_chatter("STAT: %s, NEXT: %s", _start.unparse().c_str(), ts.unparse().c_str());
        _timer.schedule_at(ts);
    }
    StringAccum sa;
    for (auto ptr : _elements)
        sa << '\t' << ptr->name();
    click_chatter("%s", sa.c_str());

    return 0;
}

void GPS_TEST_PrintCounter::run_timer(Timer *) {
    printCounts();
    _timer.schedule_at(_start + _interval * (++_counter));
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_TEST_PrintCounter)