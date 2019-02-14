#ifndef WINDZ_MUTEX_H
#define WINDZ_MUTEX_H

#include "windz/base/Noncopyable.h"

#include <pthread.h>
#include <time.h>

namespace windz {

class Mutex : Noncopyable {
    friend class Condition;

  public:
    Mutex() { pthread_mutex_init(&mutex_, nullptr); }
    ~Mutex() {
        pthread_mutex_lock(&mutex_);
        pthread_mutex_unlock(&mutex_);
        pthread_mutex_destroy(&mutex_);
    }
    void Lock() {
        pthread_mutex_lock(&mutex_);
    }
    bool TimedLock(time_t sec) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += sec;
        return pthread_mutex_timedlock(&mutex_, &ts) == 0;
    }
    bool TryLock() {
        return pthread_mutex_trylock(&mutex_) == 0;
    }
    void Unlock() {
        pthread_mutex_unlock(&mutex_);
    }

  private:
    pthread_mutex_t mutex_;

    pthread_mutex_t *Get() { return &mutex_; }
};

class LockGuard : Noncopyable {
  public:
    explicit LockGuard(Mutex &mutex) : mutex_(mutex) { mutex_.Lock(); }
    ~LockGuard() { mutex_.Unlock(); }

  private:
    Mutex &mutex_;
};

#define LockGuard(x) static_assert(false, "missing mutex guard variable name")

}  // namespace windz

#endif  // WINDZ_MUTEX_H
