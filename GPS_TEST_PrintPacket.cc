//
// Created by jiachen on 9/20/18.
//

#include "GPS_TEST_PrintPacket.hh"
#include "GPS_PacketHeader.hh"
#include <click/args.hh>

CLICK_DECLS

GPS_TEST_PrintPacket::GPS_TEST_PrintPacket() = default;

GPS_TEST_PrintPacket::~GPS_TEST_PrintPacket() = default;

int GPS_TEST_PrintPacket::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh)
                .read_p("LABEL", _label)
                .complete() < 0) {
        return -1;
    }
    return 0;
}

Packet *GPS_TEST_PrintPacket::simple_action(Packet *p) {

    if (p->length() < sizeof(gps_packet_t)) {
        click_chatter("%s: error packet length (%d) < %d", _label.c_str(), p->length(), sizeof(gps_packet_t));
    } else {
        auto header = p->data();
        auto type = gps_packet_get_type(header);
        switch (type) {
            case GPS_PACKET_TYPE_LSA:
                if (p->length() < sizeof(gps_packet_lsa_t)) {
                    click_chatter("%s: LSA error packet length (%d) < %d", _label.c_str(), p->length(),
                                  sizeof(gps_packet_lsa_t));
                } else {
                    gps_packet_lsa_print(header, _label);
                }
                break;
            case GPS_PACKET_TYPE_PUBLICATION:
                if (p->length() < sizeof(gps_packet_publication_t)) {
                    click_chatter("%s: PUBLICATION error packet length (%d) < %d", _label.c_str(), p->length(),
                                  sizeof(gps_packet_publication_t));
                } else {
                    gps_packet_publication_print(header, _label, p->length());
                }
                break;
            case GPS_PACKET_TYPE_SUBSCRIPTION:
                if (p->length() < sizeof(gps_packet_subscription_t)) {
                    click_chatter("%s: SUBSCRIPTION error packet length (%d) < %d", _label.c_str(), p->length(),
                                  sizeof(gps_packet_subscription_t));
                } else {
                    gps_packet_subscription_print(header, _label);
                }
                break;
            case GPS_PACKET_TYPE_GNRS_REQ:
                if (p->length() < sizeof(gps_packet_gnrsRequest_t)) {
                    click_chatter("%s: SUBSCRIPTION error packet length (%d) < %d", _label.c_str(), p->length(),
                                  sizeof(gps_packet_gnrsRequest_t));
                } else {
                    gps_packet_gnrsRequest_print(header, _label);
                }
                break;
            case GPS_PACKET_TYPE_GNRS_RESP:
                if (p->length() < sizeof(gps_packet_gnrsResponse_t)) {
                    click_chatter("%s: GNRS_RESPONSE error packet length (%d) < %d", _label.c_str(), p->length(),
                                  sizeof(gps_packet_gnrsResponse_t));
                } else {
                    gps_packet_gnrsResponse_print(header, _label);
                }
                break;

            default:
                click_chatter("%s: unknown packet type: 0x%02x", _label.c_str(), type);
                break;
        }
    }
    return p;
}


CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_TEST_PrintPacket)