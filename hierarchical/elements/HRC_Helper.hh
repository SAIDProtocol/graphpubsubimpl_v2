//
// Created by jiachen on 9/20/18.
//

#ifndef CLICK_HRC_HELPER_HH
#define CLICK_HRC_HELPER_HH

#include <click/config.h>
#include <click/glue.hh>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <sys/time.h>

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

CLICK_ENDDECLS

#endif //CLICK_HRC_HELPER_HH

