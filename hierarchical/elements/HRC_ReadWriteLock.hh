//
// Created by jiachen on 10/1/18.
//

#ifndef CLICK_HRC_READWRITELOCK_HH
#define CLICK_HRC_READWRITELOCK_HH

#include <click/config.h>
#include <click/glue.hh>
#include <cassert>
#include <pthread.h>

CLICK_DECLS

/** @Class HRC_ReadWriteLock
 *
 * same function and interface as click ReadWriteLock (but ReadWriteLock only works in LINUX_MODULE and CLICK_SMP
 * implemented using pthread library
 * see: https://docs.oracle.com/cd/E19455-01/806-5257/6je9h032u/index.html
 */
class HRC_ReadWriteLock {
public:

    inline HRC_ReadWriteLock() : _lock() {
        int ret = pthread_rwlock_init(&_lock, nullptr);
        assert(!ret);
    }

    inline ~HRC_ReadWriteLock() {
        int ret = pthread_rwlock_destroy(&_lock);
        assert(!ret);
    }

    inline void acquire_read() {
        int ret = pthread_rwlock_rdlock(&_lock);
        assert(!ret);
    }

    inline bool attempt_read() {
        int ret = pthread_rwlock_tryrdlock(&_lock);
        return !ret;
    }

    inline void release_read() {
        pthread_rwlock_unlock(&_lock);
    }

    inline void acquire_write() {
        int ret = pthread_rwlock_wrlock(&_lock);
        assert(!ret);
    }

    inline bool attempt_write() {
        int ret = pthread_rwlock_trywrlock(&_lock);
        return !ret;
    }

    inline void release_write() {
        pthread_rwlock_unlock(&_lock);
    }

private:
    pthread_rwlock_t _lock;
};

CLICK_ENDDECLS

#endif //CLICK_HRC_READWRITELOCK_HH
