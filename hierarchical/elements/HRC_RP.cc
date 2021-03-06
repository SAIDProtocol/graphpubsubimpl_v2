//
// Created by jiachen on 10/7/18.
//

#include "HRC_RP.hh"
#include "HRC_NA.hh"
#include "HRC_PacketHeader.hh"
#include "HRC_PacketAnno.hh"


CLICK_DECLS

HRC_RP::HRC_RP() = default;

HRC_RP::~HRC_RP() = default;


void HRC_RP::push(int, Packet *p) {
    auto header = p->data();
    auto type = hrc_packet_get_type(header);
    switch (type) {
        case HRC_PACKET_TYPE_INTEREST: {
            auto headerSize = hrc_packet_interest_get_header_size(header);
            p->pull(headerSize);
            auto newHeader = p->data();
            type = hrc_packet_get_type(newHeader);
            if (unlikely(type != HRC_PACKET_TYPE_PUBLICATION)) {
                ErrorHandler::default_handler()->error(
                        "[HRC_RP::push] Error inner packet type (%02x). Should be publication (%02x)",
                        type, HRC_PACKET_TYPE_PUBLICATION);
                p->kill();
            } else {
                hrc_na_clear(HRC_ANNO_PREV_HOP_NA(p));
                checked_output_push(0, p);
            }
            break;
        }
        case HRC_PACKET_TYPE_SUBSCRIPTION:
            ErrorHandler::default_handler()->debug("[HRC_RP::push] Getting subscription, discard.");
            p->kill();
            break;
        default:
            ErrorHandler::default_handler()->error("[HRC_RP::push] Error packet type (%02x). Should be interest (%02x)",
                                                   type, HRC_PACKET_TYPE_INTEREST);
            p->kill();
    }
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_RP)