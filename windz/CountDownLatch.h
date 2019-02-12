#ifndef WINDZ_COUNTDOWNLATCH_H
#define WINDZ_COUNTDOWNLATCH_H

#include "Condition.h"
#include "Mutex.h"
#include "Noncopyable.h"

namespace windz {

class CountDownLatch : Noncopyable {
  public:
    explicit CountDownLatch(int count) : mutex_(), cond_(mutex_), count_(count) {}
    void Wait() {
        LockGuard lock(mutex_);
        cond_.Wait([this]() { return this->count_ <= 0; });
    }
    void CountDown() {
        LockGuard lock(mutex_);
        --count_;
        if (count_ == 0) cond_.NotifyAll();
    }

  private:
    mutable Mutex mutex_;
    Condition cond_;
    int count_;
};

}  // namespace windz

#endif  // WINDZ_COUNTDOWNLATCH_H
