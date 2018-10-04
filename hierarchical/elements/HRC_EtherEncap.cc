//
// Created by ubuntu on 10/4/18.
//

#include "HRC_EtherEncap.hh"
#include "HRC_PacketAnno.hh"
#include "HRC_Helper.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ether.h>

CLICK_DECLS

HRC_EtherEncap::HRC_EtherEncap() = default;

HRC_EtherEncap::~HRC_EtherEncap() = default;

int HRC_EtherEncap::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh)
                .read_mp("MY_ETHER", EtherAddressArg(), _myEther)
                .complete() < 0) {
        return -1;
    }
    errh->message("MyEther: %s", _myEther.unparse_colon().c_str());

    return 0;
}

Packet *HRC_EtherEncap::simple_action(Packet *p) {
    auto writablePacket = p->push_mac_header(sizeof(click_ether));
    auto macHeader = reinterpret_cast<click_ether *>(writablePacket->mac_header());
    copyEther(macHeader->ether_dhost, HRC_ANNO_NEXT_HOP_ETHER(p));
    copyEther(macHeader->ether_shost, _myEther.data());
    macHeader->ether_type = HRC_EtherEncap::HRC_DEFAULT_ETHER_TYPE();
    return writablePacket;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_EtherEncap)