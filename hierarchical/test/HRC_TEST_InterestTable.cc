//
// Created by jiachen on 10/1/18.
//

#include "HRC_TEST_InterestTable.hh"
#include <click/args.hh>

CLICK_DECLS

HRC_TEST_InterestTable::HRC_TEST_InterestTable() = default;

HRC_TEST_InterestTable::~HRC_TEST_InterestTable() = default;


static void printInterestTable(const std::string &prefix, int &v) {
    click_chatter("%s (%p) -> %d (%p)", prefix.c_str(), &prefix, v, &v);
}

void HRC_TEST_InterestTable::longestPrefixMatch(const String &param) {
    _longestPrefixMatch = param;
    auto v = _table.longestPrefixMatch(_longestPrefixMatch.c_str());
    if (v) click_chatter("longestPrefixMatch: %s -> %d", _longestPrefixMatch.c_str(), *v);
    else click_chatter("longestPrefixMatch: %s -> (nil)", _longestPrefixMatch.c_str());
}

void HRC_TEST_InterestTable::exactMatch(const String &param) {
    _exactMatch = param;
    auto v = _table.exactMatch(_exactMatch.c_str());
    if (v) click_chatter("exactMatch: %s -> %d", _exactMatch.c_str(), *v);
    else click_chatter("exactMatch: %s -> (nil)", _exactMatch.c_str());
}

void HRC_TEST_InterestTable::set(const String &param) {
    click_chatter("Set: %s", _set.c_str());
    _set = param;
    String conf = cp_uncomment(param);
    const char *s = conf.begin(), *end = conf.end();

    while (s < end) {
        const char *nl = find(s, end, '\n');
        uint32_t val;
        String line = conf.substring(s, nl);
        String first_word = cp_shift_spacevec(line);
        if (cp_real10(line, 0, &val)) {
            click_chatter(R"(Parse line "%s" "%s")", first_word.c_str(), line.c_str());
            click_chatter(R"(  set: "%s" -> %d)", first_word.c_str(), val);
            _table.set(first_word.c_str(), val);
        } else {
            click_chatter(R"(Cannot parse line "%s" "%s")", first_word.c_str(), line.c_str());
        }
        s = nl + 1;
    }
    print();
}

void HRC_TEST_InterestTable::remove(const String &param) {
    _remove = param;
    click_chatter("Remove: %s", _remove.c_str());
    _table.remove(_remove.c_str());
    print();
}

void HRC_TEST_InterestTable::print() {
    click_chatter(">>>>Printing table...");
    _table.forEach(printInterestTable);
    click_chatter(">>>>End Printing table...");
}

int HRC_TEST_InterestTable::change_param(const String &s, Element *e, void *vparam, ErrorHandler *errh) {
    (void) errh;
    auto elem = dynamic_cast<HRC_TEST_InterestTable *>(e);

    switch (reinterpret_cast<intptr_t >(vparam)) {
        case h_lpm: {
            elem->longestPrefixMatch(s);
            break;
        }
        case h_exact:
            elem->exactMatch(s);
            break;
        case h_print:
            elem->print();
            break;
        case h_set:
            elem->set(s);
            break;
        case h_remove:
            elem->remove(s);
            break;
        default:
            break;
    }
    return 0;
}

void HRC_TEST_InterestTable::add_handlers() {
    add_data_handlers("longestPrefixMatch", Handler::f_read | Handler::f_write, &_longestPrefixMatch);
    add_write_handler("longestPrefixMatch", change_param, h_lpm, Handler::f_raw);
    add_data_handlers("exactMatch", Handler::f_read | Handler::f_write, &_exactMatch);
    add_write_handler("exactMatch", change_param, h_exact, Handler::f_raw);
    add_data_handlers("set", Handler::f_read | Handler::f_write, &_set);
    add_write_handler("set", change_param, h_set, Handler::f_raw);
    add_data_handlers("remove", Handler::f_read | Handler::f_write, &_remove);
    add_write_handler("remove", change_param, h_remove, Handler::f_raw);
    add_write_handler("print", change_param, h_print, Handler::BUTTON);
}


int HRC_TEST_InterestTable::initialize(ErrorHandler *errh) {
    (void) errh;
    click_chatter("set /a/bb -> 1");
    _table.set("/a/bb", 1);
    click_chatter("set /a/ccc -> 2");
    _table.set("/a/ccc", 2);
    click_chatter("set /a/bb/dddd -> 3");
    _table.set("/a/bb/dddd", 3);
    click_chatter("set /a//eeeee -> 4");
    _table.set("/a//eeeee", 4);
    click_chatter("set /a/ccc -> 5");
    _table.set("/a/ccc", 5);
    _table.forEach(printInterestTable);

    click_chatter("remove /a//eeeee");
    _table.remove("/a//eeeee");
    _table.forEach(printInterestTable);

    click_chatter("remove /a/bb");
    _table.remove("/a/bb");
    _table.forEach(printInterestTable);

    click_chatter("remove /a/bb");
    _table.remove("/a/bb");
    _table.forEach(printInterestTable);

    click_chatter("remove /a/bb/dddd");
    _table.remove("/a/bb/dddd");
    _table.forEach(printInterestTable);

    click_chatter("set /a/bb -> 1");
    _table.set("/a/bb", 1);
    click_chatter("set /a/ccc -> 2");
    _table.set("/a/ccc", 2);
    click_chatter("set /a/bb/dddd -> 3");
    _table.set("/a/bb/dddd", 3);
    click_chatter("set /a//eeeee -> 4");
    _table.set("/a//eeeee", 4);
    _table.forEach(printInterestTable);

    return 0;
}


EXPORT_ELEMENT(HRC_TEST_InterestTable)

CLICK_ENDDECLS



