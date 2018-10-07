//
// Created by jiachen on 10/6/18.
//

#include "HRC_PublicationEncapsulator.hh"
#include "HRC_Helper.hh"
#include "HRC_PacketHeader.hh"
#include <click/args.hh>
#include <click/error.hh>

CLICK_DECLS

//#define HRC_GNRSCACHE_DEBUG

static ErrorHandler *tmpErrh;

static void printInterestTable(const std::string &prefix, const char *const &v) {
    tmpErrh->debug("  %s (%p) -> %s (%p)", prefix.c_str(), &prefix, v, v);
}


HRC_PublicationEncapsulator::HRC_PublicationEncapsulator() {
    tmpErrh = ErrorHandler::default_handler();
};

HRC_PublicationEncapsulator::~HRC_PublicationEncapsulator() = default;

int HRC_PublicationEncapsulator::configure(Vector<String> &conf, ErrorHandler *errh) {
    String fileName;
    if (Args(conf, this, errh)
                .read_mp("FILENAME", FilenameArg(), fileName)
                .complete() < 0) {
        return -1;
    }

    if (parseGNRSFile(fileName, errh, _table) < 0) return -1;

    {
        tmpErrh = errh;
        _table.forEach(printInterestTable);
        tmpErrh = ErrorHandler::default_handler();
    }

    return 0;
}

void HRC_PublicationEncapsulator::push(int, Packet *p) {
    auto header = p->data();
    auto type = hrc_packet_get_type(header);
    if (unlikely(type != HRC_PACKET_TYPE_PUBLICATION)) {
        ErrorHandler::default_handler()->debug(
                "[HRC_PublicationEncapsulator::handlePublication] Error packet type (%02x). Should be publication (%02x)",
                type, HRC_PACKET_TYPE_PUBLICATION);
        p->kill();
    } else {
        auto cd = hrc_packet_publication_get_name(header);
        _lock.acquire_read();
        auto prp = _table.longestPrefixMatch(cd);
        if (!prp) {
            _lock.release_read();
            checked_output_push(OUT_PORT_DISCARD, p);
        }
        else {
            auto size = p->length();
            auto rp = *prp;
            auto headerSize = hrc_packet_interest_calculate_header_size(rp);
            auto wp = p->push(static_cast<uint32_t>(headerSize));
            auto writableHeader = wp->data();
            hrc_packet_interest_init(writableHeader, rp, size);
            _lock.release_read();
            checked_output_push(OUT_PORT_UNICAST, wp);
        }
    }
}

CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_PublicationEncapsulator)