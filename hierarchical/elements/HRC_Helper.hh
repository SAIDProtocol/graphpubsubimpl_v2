//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_HRC_HELPER_HH
#define CLICK_HRC_HELPER_HH

#include <click/config.h>
#include <click/error.hh>
#include <click/glue.hh>
#include <click/string.hh>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <sys/time.h>
#include "HRC_InterestTable.hh"


CLICK_DECLS

static inline ssize_t skipEmpty(const char *line) {
    char c;
    ssize_t ret = 0;
    while ((c = line[ret]) == ' ' || c == '\t')
        ret++;
    return (c == '\r' || c == '\n' || c == '\0') ? -1 : ret;
}

// place a '\0' at where the part ends
// if end of line reached, return -1
// otherwise, return the length of the part (with the trailing '\0')

static inline ssize_t getPart(char *line) {
    char c;
    ssize_t ret = 0;
    while ((c = line[ret]) != ' ' && c != '\t' && c != '\0' && c != '\r' && c != '\n')
        ret++;
    if (c == '\0') {
        return -1;
    } else {
        line[ret] = '\0';
        return ret + 1;
    }
}

typedef struct timeval timeval_t;
typedef struct timezone timezone_t;

static inline void printTime() {
    timeval_t tv;
    timezone_t tz;
    struct tm *tm;
    gettimeofday(&tv, &tz);
    tm = localtime(&tv.tv_sec);
    click_chatter("%02d:%02d:%02d:%06ld", tm->tm_hour, tm->tm_min, tm->tm_sec, (tv.tv_usec));

}

static inline void copyEther(uint8_t *target, const uint8_t *src) {
    memcpy(target, src, 6);
}


static inline int parseGNRSFile(String &fileName, ErrorHandler *errh, HRC_InterestTable<const char *> &table) {
    auto fp = fopen(fileName.c_str(), "r");
    if (fp == nullptr) {
        errh->error("Cannot read file %s", fileName.c_str());
        return -1;
    }

    char *line = nullptr, *tmp;
    size_t len = 0, lineNum = 0;
    ssize_t pos;

    char *cd, *rp;

    while (getline(&line, &len, fp) != -1) {
        lineNum++;
#ifdef HRC_PARSEGNRSFILE_DEBUG
        errh->debug(">>>>>>>>>>line: %d<<<<<<<<<<", lineNum);
        errh->debug(R"(line: (%p) "%s")", line, line);
#endif
        tmp = line;
        pos = skipEmpty(tmp);
        //empty line
        if (pos == -1) continue;

        tmp += pos;
#ifdef HRC_PARSEGNRSFILE_DEBUG
        errh->debug(R"(skip empty pos: %d, line: "%s")", pos, tmp);
#endif
        // comment skip
        if (*tmp == '#') continue;

        // cd
        pos = getPart(tmp);
#ifdef HRC_PARSEGNRSFILE_DEBUG
        errh->debug(R"(get part pos: %d, line: "%s")", pos, tmp);
#endif
        cd = tmp;
#ifdef HRC_PARSEGNRSFILE_DEBUG
        errh->debug(R"(cd (%p): "%s")", cd, cd);
#endif
        tmp += pos;

        // rp
#ifdef HRC_PARSEGNRSFILE_DEBUG
        errh->debug(R"(line: "%s")", tmp);
#endif
        pos = skipEmpty(tmp);
#ifdef HRC_PARSEGNRSFILE_DEBUG
        errh->debug(R"(skip empty pos: %d, line: "%s")", pos, tmp);
#endif
        if (pos == -1) {
            errh->error("Skip line %d: cannot find rp.", lineNum);
            continue;
        }
        tmp += pos;
        pos = getPart(tmp);
#ifdef HRC_PARSEGNRSFILE_DEBUG
        errh->debug(R"(get part pos: %d, line: (%p) "%s")", pos, tmp, tmp);
#endif
        if (pos == -1) pos = strlen(tmp) + 1;
        rp = new char[pos];
        memcpy(rp, tmp, static_cast<size_t>(pos));
#ifdef HRC_PARSEGNRSFILE_DEBUG
        errh->debug(R"(rp: (%p) "%s")", rp, rp);
#endif
        tmp += pos;
        (void) tmp;

#ifdef HRC_PARSEGNRSFILE_DEBUG
        errh->debug(R"(Entry: (%p) "%s" -> "%s")", cd, cd, rp);
#endif
        auto exist = table.exactMatch(cd);
        if (exist) {
#ifdef HRC_PARSEGNRSFILE_DEBUG
            errh->debug("exist=%p, *exist=%p, delete", exist, *exist);
#endif
            delete[] *exist;
        } else {
#ifdef HRC_PARSEGNRSFILE_DEBUG
            errh->debug("not exist");
#endif
        }
        table.set(cd, rp);
    }
    fclose(fp);
    if (line) free(line);


    return 0;
}


CLICK_ENDDECLS

#endif //CLICK_HRC_HELPER_HH

