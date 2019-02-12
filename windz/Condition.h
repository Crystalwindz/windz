#ifndef WINDZ_CONDITION_H
#define WINDZ_CONDITION_H

#include "Noncopyable.h"
#include "Mutex.h"

#include <pthread.h>
#include <time.h>

#include <functional>

namespace windz {

class Condition : Noncopyable {
  public:
    explicit Condition(Mutex &mutex) : mutex_(mutex) {
        pthread_cond_init(&cond_, nullptr);
    }
    ~Condition() {
        pthread_cond_destroy(&cond_);
    }
    void Wait() {
        pthread_cond_wait(&cond_, mutex_.Get());
    }
    void Wait(const std::function<bool()> &pred) {
        while (!pred()) {
            Wait();
        }
    }
    bool TimedWait(time_t sec) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += sec;
        return pthread_cond_timedwait(&cond_, mutex_.Get(), &ts) == 0;
    }
    void Notify() {
        pthread_cond_signal(&cond_);
    }
    void NotifyAll() {
        pthread_cond_broadcast(&cond_);
    }

  private:
    Mutex &mutex_;
    pthread_cond_t cond_;
};

}  // namespace windz

#endif //WINDZ_CONDITION_H
