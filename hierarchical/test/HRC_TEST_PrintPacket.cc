//
// Created by jiachen on 10/3/18.
//

#include "HRC_TEST_PrintPacket.hh"
#include "../elements/HRC_PacketHeader.hh"
#include <click/args.hh>
#include <click/error.hh>

CLICK_DECLS

HRC_TEST_PrintPacket::HRC_TEST_PrintPacket() : _limit(20) {

};

HRC_TEST_PrintPacket::~HRC_TEST_PrintPacket() = default;

Packet *HRC_TEST_PrintPacket::simple_action(Packet *p) {
    if (p->length() < sizeof(hrc_packet_t)) {
        click_chatter("Packet size too small (%d)", p->length());
    } else {
        uint32_t type = hrc_packet_get_type(p->data());
        switch(type) {
            case HRC_PACKET_TYPE_INTEREST:
                hrc_packet_interest_print(p->data(), _label, p->length(), _limit, p->headroom(), p->tailroom());
                break;
            case HRC_PACKET_TYPE_PUBLICATION:
                break;
            case HRC_PACKET_TYPE_SUBSCRIPTION:
                break;
            default:
                click_chatter("Unknown packet type: 0x%02x", type);
                break;
        }
    }

    return p;
}

int HRC_TEST_PrintPacket::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh)
                .read_mp("LABEL", _label)
                .read_p("MAXLENGTH", _limit)
                .complete() < 0)
        return -1;
    return 0;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_TEST_PrintPacket)