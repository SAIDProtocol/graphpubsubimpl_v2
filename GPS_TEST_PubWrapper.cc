//
// Created by jiachen on 9/20/18.
//

#include "GPS_TEST_PubWrapper.hh"
#include "GPS_PacketHeader.hh"
#include <click/args.hh>
#include <click/error.hh>

CLICK_DECLS

GPS_TEST_PubWrapper::GPS_TEST_PubWrapper() = default;

GPS_TEST_PubWrapper::~GPS_TEST_PubWrapper() = default;

int GPS_TEST_PubWrapper::configure(Vector<String> &conf, ErrorHandler *errh) {
    uint32_t numSrcGuid = 0, numDstGuid, numSrcNa = 0, numDstNa = 0;
    if (Args(conf, this, errh)
                .read_p("SRC_GUID", numSrcGuid)
                .read_mp("DST_GUID", numDstGuid)
                .read_p("SRC_NA", numSrcNa)
                .read_p("DST_NA", numDstNa)
                .complete() < 0) {
        return -1;
    }
    gps_guid_set_val(&_srcGuid, numSrcGuid);
    gps_guid_set_val(&_dstGuid, numDstGuid);

    gps_na_set_val(&_srcNa, numSrcNa);
    gps_na_set_val(&_dstNa, numDstNa);

    errh->message("numSrcGuid=0x%x, numDstGuid=0x%x, numSrcNa=0x%x, numDstNa=0x%x", numSrcGuid, numDstGuid, numSrcNa, numDstNa);
    errh->message("SRC_GUID=%s, DST_GUID=%s, SRC_NA=%s, DST_NA=%s",
                  gps_guid_unparse(&_srcGuid).c_str(),
                  gps_guid_unparse(&_dstGuid).c_str(),
                  gps_na_unparse(&_srcNa).c_str(),
                  gps_na_unparse(&_dstNa).c_str());
    return 0;
}

Packet *GPS_TEST_PubWrapper::simple_action(Packet *p) {
    uint32_t payloadSize = p->length();


    auto newPkt = p->push(sizeof(gps_packet_publication_t));

    gps_packet_publication_init(newPkt->data(), &_srcGuid, &_dstGuid, &_srcNa, &_dstNa, payloadSize);

    return p;
}

CLICK_ENDDECLS

EXPORT_ELEMENT(GPS_TEST_PubWrapper)