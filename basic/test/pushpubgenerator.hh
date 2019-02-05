//
// Created by root on 2/5/19.
//

#ifndef CLICK_BASIC_PUSHPUBGENERATOR_HH
#define CLICK_BASIC_PUSHPUBGENERATOR_HH

#include <click/config.h>
#include <click/element.hh>
#include <click/glue.hh>
#include <click/notifier.hh>
#include <click/task.hh>


CLICK_DECLS

class PushPubGenerator : public Element {
public:
    PushPubGenerator() CLICK_COLD;

    ~PushPubGenerator() override CLICK_COLD;

    bool run_task(Task *task) override;

    const char *class_name() const override { return "BASIC_PushPubGenerator"; }

    const char *port_count() const override { return "0/1"; }

    const char *processing() const override { return PUSH; }

    int configure(Vector<String> &conf, ErrorHandler *errh) override CLICK_COLD;

    int initialize(ErrorHandler *errh) override CLICK_COLD;

    void cleanup(CleanupStage stage) override;

private:
    Packet *_pkt;
    uint64_t _pktCount;
    Task _task;
    struct timeval _start, _end;
};

CLICK_ENDDECLS

#endif //CLICK_BASIC_PUSHPUBGENERATOR_HH
