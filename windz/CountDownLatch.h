//
// Created by crystalwind on 18-12-25.
//

#ifndef WINDZ_COUNTDOWNLATCH_H
#define WINDZ_COUNTDOWNLATCH_H

#include "Util.h"
#include "Mutex.h"
#include "Condition.h"

namespace windz {

class CountDownLatch : private noncopyable {
  public:
    explicit CountDownLatch(int count) : mutex_(), cond_(mutex_), count_(count) {}
    void Wait() {
        LockGuard lock(mutex_);
        cond_.Wait([this](){ return this->count_ <= 0; });
    }
    void CountDown() {
        LockGuard lock(mutex_);
        --count_;
        if (count_ == 0)
            cond_.NotifyAll();
    }

  private:
    mutable Mutex mutex_;
    Condition cond_;
    int count_;
};

}  // namespace windz

#endif //WINDZ_COUNTDOWNLATCH_H
