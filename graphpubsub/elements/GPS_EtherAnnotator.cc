//
// Created by ubuntu on 9/22/18.
//

#include "GPS_EtherAnnotator.hh"
#include "GPS_PacketAnno.hh"
#include "GPS_Helper.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ether.h>

CLICK_DECLS

GPS_EtherAnnotator::GPS_EtherAnnotator() = default;


GPS_EtherAnnotator::~GPS_EtherAnnotator() = default;

Packet *GPS_EtherAnnotator::simple_action(Packet *p) {
    auto etherHeader = p->ether_header();
    copyEther(GPS_ANNO_SRC_ETHER(p), etherHeader->ether_shost);
    *GPS_ANNO_SRC_PORT(p) = _myPort;
    return p;
}

int GPS_EtherAnnotator::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh)
                .read_mp("PORT", _myPort)
                .complete() < 0) {
        return -1;
    }

    errh->debug("port=0x%02x", _myPort);

    return 0;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_EtherAnnotator)