#ifndef WINDZ_MUTEX_H
#define WINDZ_MUTEX_H

#include "Noncopyable.h"

#include <pthread.h>
#include <time.h>

namespace windz {

class Mutex : Noncopyable {
    friend class Condition;

  public:
    Mutex() {
        pthread_mutex_init(&mutex_, nullptr);
    }
    ~Mutex() {
        pthread_mutex_lock(&mutex_);
        pthread_mutex_unlock(&mutex_);
        pthread_mutex_destroy(&mutex_);
    }
    void Lock() {// 只应该被LockGuard调用
        pthread_mutex_lock(&mutex_);
    }
    bool TimedLock(time_t sec) {// 实际没用到
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += sec;
        return pthread_mutex_timedlock(&mutex_, &ts) == 0;
    }
    bool TryLock() {// 实际没用到
        return pthread_mutex_trylock(&mutex_) == 0;
    }
    void Unlock() {// 只应该被LockGuard调用
        pthread_mutex_unlock(&mutex_);
    }

  private:
    pthread_mutex_t mutex_;

    pthread_mutex_t *Get() {
        return &mutex_;
    }
};

class LockGuard : Noncopyable {
  public:
    explicit LockGuard(Mutex &mutex) : mutex_(mutex) {
        mutex_.Lock();
    }
    ~LockGuard() {
        mutex_.Unlock();
    }

  private:
    Mutex &mutex_;
};

#define LockGuard(x) static_assert(false, "missing mutex guard variable name")

}  // namespace windz

#endif //WINDZ_MUTEX_H
