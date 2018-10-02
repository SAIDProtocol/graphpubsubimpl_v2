//
// Created by jiachen on 9/26/18.
//

#include "HRC_FIB.hh"
#include <click/args.hh>

CLICK_DECLS

HRC_FIB::HRC_FIB() = default;

HRC_FIB::~HRC_FIB() = default;

void HRC_FIB::push(int port, Packet *p) {
    switch (port) {
        case IN_PORT_DATA:
            handleData(p);
            break;
        case IN_PORT_SUBSCRIPTION:
            handleSubscription(p);
            break;
        case IN_PORT_ANNOUNCEMENT:
            handleAnnouncement(p);
            break;
        default:
            // from other ports?? should not reach here!
            click_chatter("[HRC_FIB::push] Got packet from unexpected port: %d, discarding...", port);
            p->kill();
            break;
    }
}

int HRC_FIB::parseArgFile(String &fileName, ErrorHandler *errh) {
    (void) fileName;
    (void) errh;
    return 0;
}

int HRC_FIB::configure(Vector<String> &conf, ErrorHandler *errh) {
    String fileName;
    if (Args(conf, this, errh)
                .read_mp("FILENAME", FilenameArg(), fileName)
                .complete() < 0) {
        return -1;
    }
    if (parseArgFile(fileName, errh) < 0) return -1;

    return 0;
}


void HRC_FIB::handleData(Packet *packet) {
    packet->kill();
}

void HRC_FIB::handleSubscription(Packet *packet) {
    packet->kill();
}

void HRC_FIB::handleAnnouncement(Packet *packet) {
    packet->kill();
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_FIB)

