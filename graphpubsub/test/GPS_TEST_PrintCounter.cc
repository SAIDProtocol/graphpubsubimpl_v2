//
// Created by root on 10/23/18.
//

#include "GPS_TEST_PrintCounter.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/llrpc.h>

CLICK_DECLS

GPS_TEST_PrintCounter::GPS_TEST_PrintCounter() = default;

GPS_TEST_PrintCounter::~GPS_TEST_PrintCounter() = default;

int GPS_TEST_PrintCounter::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
                .read_all("ELEM", ElementArg(), _elements)
                .complete() < 0)
        return -1;
    errh->debug("count: %d", _elements.size());
    for (auto ptr : _elements)
        errh->debug("%s::%s", ptr->name().c_str(), ptr->class_name());

    return 0;
}

void GPS_TEST_PrintCounter::cleanup(Element::CleanupStage stage) {
    if (stage == CLEANUP_ROUTER_INITIALIZED) {
        for (auto ptr : _elements) {
            if (strcmp(ptr->class_name(), "Counter") != 0)
                continue;
            uint32_t count = 0;
            int ret;
            if ((ret = ptr->llrpc(CLICK_LLRPC_GET_COUNT, &count))) {
                click_chatter("%s: err: %d", ptr->name().c_str(), ret);
            } else {
                click_chatter("%s: %d", ptr->name().c_str(), count);
            }
        }
        Timestamp ts = Timestamp::now();
        ts -= _start;
        click_chatter("Duration: %s", ts.unparse_interval().c_str());
    }
}

int GPS_TEST_PrintCounter::initialize(ErrorHandler *) {
    _start = Timestamp::now();
    return 0;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_TEST_PrintCounter)