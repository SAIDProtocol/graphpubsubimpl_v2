//
// Created by jiachen on 10/3/18.
//

#include "HRC_TEST_SubscriptionTable.hh"
#include <click/error.hh>
#include <click/straccum.hh>

CLICK_DECLS

static ErrorHandler *_errhForPrint;

HRC_TEST_SubscriptionTable::HRC_TEST_SubscriptionTable() {
    _errhForPrint = ErrorHandler::default_handler();
};

HRC_TEST_SubscriptionTable::~HRC_TEST_SubscriptionTable() = default;

static inline StringAccum &fillValueList(StringAccum &sa, const std::unordered_set<int> &val) {
    for (auto i : val) {
        sa.append(' ');
        sa.append_numeric(static_cast<String::intmax_t >(i));
    }
    return sa;
}


static void printSubscriptionTable(const std::string &prefix, const std::unordered_set<int> &val) {
    StringAccum sa;
    _errhForPrint->debug("  %s -> (%p)%s", prefix.c_str(), &val, fillValueList(sa, val).c_str());
}

void HRC_TEST_SubscriptionTable::add_handlers() {
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

int HRC_TEST_SubscriptionTable::change_param(const String &s, Element *e, void *vparam, ErrorHandler *) {
    auto elem = dynamic_cast<HRC_TEST_SubscriptionTable *>(e);

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

void HRC_TEST_SubscriptionTable::insert(const String &param, ErrorHandler *errh) {
    (void) param;
    (void) errh;
}

void HRC_TEST_SubscriptionTable::remove(const String &param, ErrorHandler *errh) {
    (void) param;
    (void) errh;
}

void HRC_TEST_SubscriptionTable::lookup(const String &param, ErrorHandler *errh) {
    StringAccum sa;
    errh->debug("** LKP %s ->%s", param.c_str(), fillValueList(sa, _table.lookup(param.c_str())).c_str());
}

void HRC_TEST_SubscriptionTable::exactMatch(const String &param, ErrorHandler *errh) {
    StringAccum sa;
    auto res = _table.exactMatch(param.c_str());
    if (!res)
        errh->debug("** MAT %s -> (nil)", param.c_str());
    else
        errh->debug("** MAT %s -> (%p)%s", param.c_str(), res, fillValueList(sa, *res).c_str());
}

void HRC_TEST_SubscriptionTable::print(ErrorHandler *errh) {
    _errhForPrint = errh;
    errh->debug(">>>> Printing table... >>>>");
    _table.forEach(printSubscriptionTable);
    errh->debug("<<<< End Printing table... <<<<");
    _errhForPrint = ErrorHandler::default_handler();
}

void HRC_TEST_SubscriptionTable::insert(const char *name, int val, ErrorHandler *errh, bool printTable) {
    errh->debug("** INS %s -> %d", name, val);
    _table.insert(name, val);
    if (printTable) print(errh);
}

void HRC_TEST_SubscriptionTable::remove(const char *name, ErrorHandler *errh, bool printTable) {
    errh->debug("** REM %s", name);
    _table.remove(name);
    if (printTable) print(errh);
}

void HRC_TEST_SubscriptionTable::remove(const char *name, int val, ErrorHandler *errh, bool printTable) {
    errh->debug("** REM %s -> %d", name, val);
    _table.remove(name, val);
    if (printTable) print(errh);
}


int HRC_TEST_SubscriptionTable::initialize(ErrorHandler *errh) {
    StringAccum sa;

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

    return 0;
}


CLICK_ENDDECLS

EXPORT_ELEMENT(HRC_TEST_SubscriptionTable)