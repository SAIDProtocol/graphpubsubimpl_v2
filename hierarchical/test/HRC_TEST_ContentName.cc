//
// Created by jiachen on 9/25/18.
//

#include "HRC_TEST_ContentName.hh"
#include "../elements/HRC_ContentName.hh"
#include <unordered_map>
#include <click/args.hh>
#include <click/error.hh>

CLICK_DECLS

HRC_TEST_ContentName::HRC_TEST_ContentName() = default;

HRC_TEST_ContentName::~HRC_TEST_ContentName() = default;

Packet *HRC_TEST_ContentName::simple_action(Packet *p) {
    return p;
}


int HRC_TEST_ContentName::configure(Vector<String> &conf, ErrorHandler *errh) {
    String name, cmp;
    size_t part;
    cnParse_t parse;
    bool hasPart, hasCmp;
    if (Args(conf, this, errh)
                .read_mp("NAME", name)
                .read_p("PART", part)
                .read_status(hasPart)
                .read_p("COMP", cmp)
                .read_status(hasCmp)
                .complete() < 0) {
        return -1;
    }
    errh->debug("Name: %s", name.c_str());
    if (hasCmp) errh->debug("COMP: %s", cmp.c_str());

    hrc_cn_parse(name.c_str(), &parse);

    for (unsigned int i = 0; i < hrc_cn_get_size(&parse); i++) {
        char *buf = hrc_cn_get_part(&parse, i);
        errh->debug(">Part %d (%d): %s", i, hrc_cn_get_part_size(&parse, i), buf);
        delete[] buf;
    }

    for (const char *tmp = name.c_str();;) {
        if (*tmp == '/') ++tmp;
        if (*tmp == '\0') break;
        CNPart part(tmp, false);
        errh->debug(">>Part (%d): %s %s", part.get_size(), part.unparse().c_str(),
                    hasCmp ? (part == cmp.c_str() ? "EQ" : "NEQ") : "");
        tmp += part.get_size();
    }


    if (hasPart) {
        if (part >= hrc_cn_get_size(&parse)) {
            errh->debug("Cannot find part %d", part);
        } else {
            char *buf = hrc_cn_get_part(&parse, part);
            errh->debug("Part %d (%d): %s", part, hrc_cn_get_part_size(&parse, part), buf);
            delete[] buf;
            if (hasCmp) {
                errh->debug("Compare with %s: %d", cmp.c_str(), hrc_cn_part_cmp(&parse, part, cmp.c_str()));

            }
        }
    }
    {
        CNPart p("abc/de", false);
        errh->debug("p=%s, hasMem=%d, %p", p.unparse().c_str(), p.hasMemory(), p.get_component());
        auto p1 = new CNPart(p);
        errh->debug("p1=%s, hasMem=%d, %p", p1->unparse().c_str(), p1->hasMemory(), p1->get_component());

        std::unordered_map<CNPart, int> map;
        errh->debug("%d", map[p]);

        for (auto &it : map) {
            errh->debug("%s (%p) -> %d, %d", it.first.unparse().c_str(), it.first.get_component(), it.second,
                        it.first.hasMemory());
        }

        map[*p1] = 2;

        for (auto &it : map) {
            errh->debug("%s (%p) -> %d, %d", it.first.unparse().c_str(), it.first.get_component(), it.second,
                        it.first.hasMemory());
        }

        delete p1;
    }

    return 0;
}


CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_TEST_ContentName)