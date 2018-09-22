//
// Created by jiachen on 9/20/18.
//

#include "HRC_ARPTable.hh"
//#include "HRC_GUID.hh"
//#include "HRC_Helper.h"
//#include "HRC_PacketAnno.h"
#include <click/error.hh>
#include <click/args.hh>

CLICK_DECLS

//#define HRC_ARPTABLE_DEBUG

HRC_ARPTable::HRC_ARPTable() = default;

HRC_ARPTable::~HRC_ARPTable() = default;


int HRC_ARPTable::parseArgFile(const String &fileName, ErrorHandler *errh) {
    (void) fileName;
    (void) errh;
    return 0;
}

int HRC_ARPTable::configure(Vector<String> &conf, ErrorHandler *errh) {
    (void) conf;
    errh->message("HRC_ARPTable configuring");
    return 0;
}


void HRC_ARPTable::push(int port, Packet *p) {
    switch (port) {
        case IN_PORT_DATA:
            handleData(p);
            break;
        case IN_PORT_LSA:
            handleLSA(p);
            break;
        default:
            // from other ports?? should not reach here!
            click_chatter("[HRC_ARPTable::push] Got packet from unexpected port: %d, discarding...", port);
            p->kill();
            break;
    }
}

void HRC_ARPTable::handleData(Packet *packet) {
    (void) packet;
}

void HRC_ARPTable::handleLSA(Packet *p) {
    (void) p;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_ARPTable)
