#ifndef WINDZ_BLOCKINGQUEUE_H
#define WINDZ_BLOCKINGQUEUE_H

#include "windz/base/Condition.h"
#include "windz/base/Mutex.h"
#include "windz/base/Noncopyable.h"

#include <assert.h>

#include <queue>
#include <utility>

namespace windz {

template <typename T>
class BlockingQueue : Noncopyable {
  public:
    // maxsize == 0 means no limit
    explicit BlockingQueue(size_t maxsize = 0)
        : mutex_(), not_empty_(mutex_), not_full_(mutex_), maxsize_(maxsize), wakeup_(false) {}

    void Push(const T &x) {
        LockGuard lock(mutex_);
        not_full_.Wait([this] {
            if (wakeup_) {
                return true;
            } else if ((maxsize_ == 0) || (queue_.size() < maxsize_)) {
                return true;
            } else {
                return false;
            }
        });
        if (wakeup_) {
            return;
        }
        assert((maxsize_ == 0) || (queue_.size() < maxsize_));
        queue_.push(x);
        not_empty_.Notify();
    }
    void Push(T &&x) {
        LockGuard lock(mutex_);
        not_full_.Wait([this] {
            if (wakeup_) {
                return true;
            } else if ((maxsize_ == 0) || (queue_.size() < maxsize_)) {
                return true;
            } else {
                return false;
            }
        });
        if (wakeup_) {
            return;
        }
        assert((maxsize_ == 0) || (queue_.size() < maxsize_));
        queue_.push(std::move(x));
        not_empty_.Notify();
    }
    T Pop() {
        LockGuard lock(mutex_);
        not_empty_.Wait([this]() {
            if (wakeup_) {
                return true;
            }
            return !queue_.empty();
        });
        if (wakeup_) {
            return T();
        }
        assert(!queue_.empty());
        T front(std::move(queue_.front()));
        queue_.pop();
        not_full_.Notify();
        return std::move(front);
    }
    size_t Size() const {
        LockGuard lock(mutex_);
        return queue_.size();
    }
    bool Empty() const { return Size() == 0; }
    // Wakeup thread blocking on the Pop() or Push().
    // After invoke Wakeup(), Push() and Pop() are invalid,
    // ensure all threads are wakeup, then invoke Restore().
    void WakeUp() {
        LockGuard lock(mutex_);
        wakeup_ = true;
        not_empty_.NotifyAll();
        not_full_.NotifyAll();
    }
    void Restore() {
        LockGuard lock(mutex_);
        wakeup_ = false;
    }

    void set_maxsize(size_t maxsize) { maxsize_ = maxsize; }
    bool wakeup() { return wakeup_; }

  private:
    mutable Mutex mutex_;
    Condition not_empty_;
    Condition not_full_;
    std::queue<T> queue_;
    size_t maxsize_;
    bool wakeup_;
};

}  // namespace windz

#endif  // WINDZ_BLOCKINGQUEUE_H
