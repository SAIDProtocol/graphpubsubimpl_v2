//
// Created by ubuntu on 9/23/18.
//

#include "GPS_EtherEncap.hh"
#include "GPS_PacketAnno.h"
#include "GPS_Helper.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ether.h>

CLICK_DECLS

GPS_EtherEncap::GPS_EtherEncap() = default;

GPS_EtherEncap::~GPS_EtherEncap() = default;

Packet *GPS_EtherEncap::simple_action(Packet *p) {
    auto writablePacket = p->push_mac_header(sizeof(click_ether));
    auto macHeader = reinterpret_cast<click_ether *>(writablePacket->mac_header());
    copyEther(macHeader->ether_dhost, GPS_ANNO_NEXT_HOP_ETHER(p));
    copyEther(macHeader->ether_shost, _myEther.data());
    macHeader->ether_type = GPS_EtherEncap::GPS_DEFAULT_ETHER_TYPE();
    return writablePacket;
}

int GPS_EtherEncap::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh)
                .read_mp("MY_ETHER", EtherAddressArg(), _myEther)
                .complete() < 0) {
        return -1;
    }
    errh->message("MyEther: %s", _myEther.unparse_colon().c_str());

    return 0;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_EtherEncap)