//
// Created by jiachen on 10/1/18.
//

#include "HRC_TEST_InterestTable.hh"
#include <click/args.hh>
#include <sstream>

CLICK_DECLS

//#define HRC_TEST_INTERESTTABLE_DEBUG

HRC_TEST_InterestTable::HRC_TEST_InterestTable() = default;

HRC_TEST_InterestTable::~HRC_TEST_InterestTable() = default;


static ErrorHandler *_errhForPrint;

static void printInterestTable(const std::string &prefix, const int &v) {
    if (_errhForPrint) _errhForPrint->debug("%s (%p) -> %d (%p)", prefix.c_str(), &prefix, v, &v);
    else click_chatter("%s (%p) -> %d (%p)", prefix.c_str(), &prefix, v, &v);
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
#ifdef HRC_TEST_INTERESTTABLE_DEBUG
    click_chatter("Set: %s", _set.c_str());
#endif
    _set = param;

    std::istringstream stream(param.c_str());
    std::string line, strVal;
    IntArg intArg;
    uint32_t val;
    int lineNum = 0;
    while (std::getline(stream, line)) {
        ++lineNum;
#ifdef HRC_TEST_INTERESTTABLE_DEBUG
        click_chatter(R"(line %d: "%s")", lineNum, line.c_str());
#endif
        std::string name, strNum;
        auto skip = line.find_first_not_of(" \t\r\n");
        if (skip == std::string::npos) {
#ifdef HRC_TEST_INTERESTTABLE_DEBUG
            click_chatter("Empty line");
#endif
            continue;
        }
        // comment
        if (line[skip] == '#') {
#ifdef HRC_TEST_INTERESTTABLE_DEBUG
            click_chatter("Comment line");
#endif
            continue;
        }
        line = line.substr(skip);

        auto size = line.find_first_of(" \t\r\n");
        skip = line.find_first_not_of(" \t\r\n", size);
        if (size == std::string::npos || skip == std::string::npos) {
            click_chatter("Skipping line %d with only 1 part.", lineNum);
            continue;
        } else {
            name = line.substr(0, size);
            line = line.substr(skip);
#ifdef HRC_TEST_INTERESTTABLE_DEBUG
            click_chatter(R"(Name: "%s", line: "%s")", name.c_str(), line.c_str());
#endif
        }

        size = line.find_first_of(" \t\r\n");
        strVal = size == std::string::npos ? line.substr() : line.substr(0, size);
#ifdef HRC_TEST_INTERESTTABLE_DEBUG
        click_chatter(R"(NA: "%s")", strVal.c_str());
#endif
        if (!intArg.parse(strVal.c_str(), val)) {
            click_chatter("Skipping line %d, cannot parse val.", lineNum);
            continue;
        }
        click_chatter("Entry: %s -> %d", name.c_str(), val);
        _table.set(name.c_str(), val);
    }
    print();
}

void HRC_TEST_InterestTable::remove(const String &param) {
    _remove = param;
    click_chatter("Remove: %s", _remove.c_str());
    _table.remove(_remove.c_str());
    print();
}

void HRC_TEST_InterestTable::print(ErrorHandler *errh) {
    _errhForPrint = errh;
    errh->debug(">>>> Printing table... >>>>");
    _table.forEach(printInterestTable);
    errh->debug("<<<< End Printing table... <<<<");
    _errhForPrint = nullptr;
}

void HRC_TEST_InterestTable::print() {
    _errhForPrint = nullptr;
    click_chatter(">>>> Printing table... >>>>");
    _table.forEach(printInterestTable);
    click_chatter("<<<< End Printing table... <<<<");
    _errhForPrint = nullptr;
}


int HRC_TEST_InterestTable::change_param(const String &s, Element *e, void *vparam, ErrorHandler *errh) {
    (void) errh;
    auto elem = dynamic_cast<HRC_TEST_InterestTable *>(e);

    switch (reinterpret_cast<intptr_t >(vparam)) {
        case h_lpm:
            elem->longestPrefixMatch(s);
            break;
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
    errh->debug("set /a/bb -> 1");
    _table.set("/a/bb", 1);
    errh->debug("set /a/ccc -> 2");
    _table.set("/a/ccc", 2);
    errh->debug("set /a/bb/dddd -> 3");
    _table.set("/a/bb/dddd", 3);
    errh->debug("set /a//eeeee -> 4");
    _table.set("/a//eeeee", 4);
    errh->debug("set /a/ccc -> 5");
    _table.set("/a/ccc", 5);
    print(errh);

    errh->debug("remove /a//eeeee");
    _table.remove("/a//eeeee");
    print(errh);

    errh->debug("remove /a/bb");
    _table.remove("/a/bb");
    print(errh);

    errh->debug("remove /a/bb");
    _table.remove("/a/bb");
    print(errh);

    errh->debug("remove /a/bb/dddd");
    _table.remove("/a/bb/dddd");
    print(errh);

    errh->debug("set /a/bb -> 1");
    _table.set("/a/bb", 1);
    errh->debug("set /a/ccc -> 2");
    _table.set("/a/ccc", 2);
    errh->debug("set /a/bb/dddd -> 3");
    _table.set("/a/bb/dddd", 3);
    errh->debug("set /a//eeeee -> 4");
    _table.set("/a//eeeee", 4);
    print(errh);

    return 0;
}


EXPORT_ELEMENT(HRC_TEST_InterestTable)

CLICK_ENDDECLS



