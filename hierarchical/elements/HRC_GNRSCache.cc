//
// Created by jiachen on 10/7/18.
//

#include "HRC_GNRSCache.hh"
#include "HRC_Helper.hh"
#include "HRC_PacketAnno.hh"
#include "HRC_PacketHeader.hh"
#include <click/args.hh>
#include <click/error.hh>

CLICK_DECLS

static ErrorHandler *tmpErrh;

static void printInterestTable(const std::string &prefix, const char *const &v) {
    tmpErrh->debug("  %s (%p) -> %s (%p)", prefix.c_str(), &prefix, v, v);
}


HRC_GNRSCache::HRC_GNRSCache() {
    tmpErrh = ErrorHandler::default_handler();
}

HRC_GNRSCache::~HRC_GNRSCache() = default;

int HRC_GNRSCache::configure(Vector<String> &conf, ErrorHandler *errh) {
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

void HRC_GNRSCache::push(int, Packet *p) {
    auto header = p->data();
    auto type = hrc_packet_get_type(header);
    if (unlikely(type != HRC_PACKET_TYPE_SUBSCRIPTION)) {
        ErrorHandler::default_handler()->debug(
                "[HRC_GNRSCache::push] Error packet type (%02x). Should be subscription (%02x)",
                type, HRC_PACKET_TYPE_SUBSCRIPTION);
        p->kill();
    } else {
        auto cd = hrc_packet_publication_get_name(header);
        _lock.acquire_read();
        auto prp = _table.longestPrefixMatch(cd);
        if (!prp) {
            _lock.release_read();
            checked_output_push(OUT_PORT_DISCARD, p);
        } else {
            auto len = strlen(*prp) + 1;
            auto tmp = new char[len];
            memcpy(tmp, *prp, len);
            _lock.release_read();
            *HRC_ANNO_DST_NAME(p) = tmp;
            checked_output_push(OUT_PORT_SUBSCRIPTION, p);
        }
    }
}


CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_GNRSCache)