//
// Created by jiachen on 10/3/18.
//

#include "HRC_TEST_MulticastTable.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/straccum.hh>
#include <sstream>

CLICK_DECLS

//#define HRC_TEST_MULTICASTTABLE_DEBUG

static ErrorHandler *_errhForPrint;

HRC_TEST_MulticastTable::HRC_TEST_MulticastTable() {
    _errhForPrint = ErrorHandler::default_handler();
};

HRC_TEST_MulticastTable::~HRC_TEST_MulticastTable() = default;

static inline StringAccum &fillValueList(StringAccum &sa, const std::unordered_set<int> &val) {
    for (auto i : val) {
        sa.append(' ');
        sa.append_numeric(static_cast<String::intmax_t >(i));
    }
    return sa;
}


static void printMulticastTable(const std::string &prefix, const std::unordered_set<int> &val) {
    StringAccum sa;
    _errhForPrint->debug("  %s -> (%p)%s", prefix.c_str(), &val, fillValueList(sa, val).c_str());
}

void HRC_TEST_MulticastTable::add_handlers() {
    add_data_handlers("lookup", Handler::f_read | Handler::f_write, &_lookup);
    add_write_handler("lookup", change_param, h_lookup, Handler::f_raw);
    add_data_handlers("exactMatch", Handler::f_read | Handler::f_write, &_exactMatch);
    add_write_handler("exactMatch", change_param, h_exact, Handler::f_raw);
    add_data_handlers("insert", Handler::f_read | Handler::f_write, &_insert);
    add_write_handler("insert", change_param, h_insert, Handler::f_raw);
    add_data_handlers("remove", Handler::f_read | Handler::f_write, &_remove);
    add_write_handler("remove", change_param, h_remove, Handler::f_raw);
    add_write_handler("print", change_param, h_print, Handler::BUTTON);
}

int HRC_TEST_MulticastTable::change_param(const String &s, Element *e, void *vparam, ErrorHandler *) {
    auto elem = dynamic_cast<HRC_TEST_MulticastTable *>(e);

    switch (reinterpret_cast<intptr_t >(vparam)) {
        case h_lookup:
            elem->lookup(s, ErrorHandler::default_handler());
            break;
        case h_exact:
            elem->exactMatch(s, ErrorHandler::default_handler());
            break;
        case h_print:
            elem->print(ErrorHandler::default_handler());
            break;
        case h_insert:
            elem->insert(s, ErrorHandler::default_handler());
            break;
        case h_remove:
            elem->remove(s, ErrorHandler::default_handler());
            break;
        default:
            break;
    }
    return 0;
}

void HRC_TEST_MulticastTable::insert(const String &param, ErrorHandler *errh) {
    _insert = param;
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
    click_chatter("Insert: %s", _insert.c_str());
#endif

    std::istringstream stream(_insert.c_str());
    std::string line, strVal;
    IntArg intArg;
    uint32_t val;
    int lineNum = 0;
    while (std::getline(stream, line)) {
        ++lineNum;
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
        click_chatter(R"(line %d: "%s")", lineNum, line.c_str());
#endif
        std::string name, strNum;
        auto skip = line.find_first_not_of(" \t\r\n");
        if (skip == std::string::npos) {
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
            click_chatter("Empty line");
#endif
            continue;
        }
        // comment
        if (line[skip] == '#') {
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
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
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
            click_chatter(R"(Name: "%s", line: "%s")", name.c_str(), line.c_str());
#endif
        }

        size = line.find_first_of(" \t\r\n");
        strVal = size == std::string::npos ? line : line.substr(0, size);
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
        click_chatter(R"(NA: "%s")", strVal.c_str());
#endif
        if (!intArg.parse(strVal.c_str(), val)) {
            click_chatter("Skipping line %d, cannot parse val.", lineNum);
            continue;
        }
        insert(name.c_str(), val, errh, false);
    }
    print(errh);
}

void HRC_TEST_MulticastTable::remove(const String &param, ErrorHandler *errh) {
    _remove = param;
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
    click_chatter("Remove: %s", _remove.c_str());
#endif

    std::istringstream stream(_remove.c_str());
    std::string line, strVal;
    IntArg intArg;
    uint32_t val;
    int lineNum = 0;
    while (std::getline(stream, line)) {
        ++lineNum;
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
        click_chatter(R"(line %d: "%s")", lineNum, line.c_str());
#endif
        std::string name, strNum;
        auto skip = line.find_first_not_of(" \t\r\n");
        if (skip == std::string::npos) {
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
            click_chatter("Empty line");
#endif
            continue;
        }
        // comment
        if (line[skip] == '#') {
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
            click_chatter("Comment line");
#endif
            continue;
        }
        line = line.substr(skip);

        auto size = line.find_first_of(" \t\r\n");
        skip = line.find_first_not_of(" \t\r\n", size);
        if (size == std::string::npos) {
            // the whole line is a name
            // remove name directly
            remove(line.c_str(), errh, false);
            continue;
        }
        if (skip == std::string::npos) {
            // the line has a name and spaces
            // remove name directly
            name = line.substr(0, size);
            remove(name.c_str(), errh, false);
            continue;
        }
        // the line has a name and a value

        name = line.substr(0, size);
        line = line.substr(skip);
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
        click_chatter(R"(Name: "%s", line: "%s")", name.c_str(), line.c_str());
#endif

        size = line.find_first_of(" \t\r\n");
        strVal = size == std::string::npos ? line : line.substr(0, size);
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
        click_chatter(R"(NA: "%s")", strVal.c_str());
#endif
        if (!intArg.parse(strVal.c_str(), val)) {
            click_chatter("Skipping line %d, cannot parse val.", lineNum);
            continue;
        }
        remove(name.c_str(), val, errh, false);
    }
    print(errh);
}

void HRC_TEST_MulticastTable::lookup(const String &param, ErrorHandler *errh) {
    _lookup = param;
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
    click_chatter("Lookup: %s", _lookup.c_str());
#endif
    StringAccum sa;
    errh->debug("** LKP %s ->%s", param.c_str(), fillValueList(sa, _table.lookup(param.c_str())).c_str());
}

void HRC_TEST_MulticastTable::exactMatch(const String &param, ErrorHandler *errh) {
    _exactMatch = param;
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
    click_chatter("ExactMatch: %s", _exactMatch.c_str());
#endif
    StringAccum sa;
    auto res = _table.exactMatch(param.c_str());
    if (!res)
        errh->debug("** MAT %s -> (nil)", param.c_str());
    else
        errh->debug("** MAT %s -> (%p)%s", param.c_str(), res, fillValueList(sa, *res).c_str());
}

void HRC_TEST_MulticastTable::print(ErrorHandler *errh) {
#ifdef HRC_TEST_MULTICASTTABLE_DEBUG
    click_chatter("Print");
#endif
    _errhForPrint = errh;
    errh->debug(">>>> Printing table... >>>>");
    _table.forEach(printMulticastTable);
    errh->debug("<<<< End Printing table... <<<<");
    _errhForPrint = ErrorHandler::default_handler();
}

void HRC_TEST_MulticastTable::insert(const char *name, int val, ErrorHandler *errh, bool printTable) {
    errh->debug("** INS %s -> %d", name, val);
    _table.insert(name, val);
    if (printTable) print(errh);
}

void HRC_TEST_MulticastTable::remove(const char *name, ErrorHandler *errh, bool printTable) {
    errh->debug("** REM %s", name);
    _table.remove(name);
    if (printTable) print(errh);
}

void HRC_TEST_MulticastTable::remove(const char *name, int val, ErrorHandler *errh, bool printTable) {
    errh->debug("** REM %s -> %d", name, val);
    _table.remove(name, val);
    if (printTable) print(errh);
}


int HRC_TEST_MulticastTable::initialize(ErrorHandler *errh) {
    insert("sports/football", 1, errh, false);
    insert("sports/football", 2, errh, true);
    insert("sports", 3, errh, true);
    insert("sports/basketball", 4, errh, true);
    insert("sports/basketball/team", 5, errh, true);
    insert("sports/basketball/match", 6, errh, true);
    insert("sports/football//test", 7, errh, false);
    insert("sports/football//test", 8, errh, true);

    lookup("sports/football", errh);
    lookup("sports/football//test", errh);

    insert("sports/football//test", 3, errh, true);
    lookup("sports/football//test", errh);

    remove("sports/football//test", 7, errh, true);
    remove("sports/football//test", errh, true);

    insert("sports/football//test", 7, errh, false);
    insert("sports/football//test", 8, errh, true);
    remove("sports/football/", errh, true);

    lookup("sports/football//test", errh);

    lookup("sports/basketball/team", errh);

    remove("sports/basketball/team", 6, errh, true);
    remove("sports/basketball/team", 5, errh, true);
    remove("sports/basketball/match", 6, errh, true);

    lookup("sports/basketball/team", errh);
    lookup("error", errh);

    exactMatch("sports/football//", errh);
    exactMatch("sports/football//test", errh);
    lookup("sports/football//test", errh);

    _lookup = "";
    _exactMatch = "";

    return 0;
}


CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_TEST_MulticastTable)