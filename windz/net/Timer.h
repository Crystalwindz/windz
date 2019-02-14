#ifndef WINDZ_TIMERQUEUE_H
#define WINDZ_TIMERQUEUE_H

#include "windz/base/Atomic.h"
#include "windz/base/Memory.h"
#include "windz/base/Noncopyable.h"
#include "windz/base/Timestamp.h"

#include <assert.h>

#include <functional>
#include <memory>
#include <vector>

namespace windz {

class EventLoop;
class Timer;

class TimerId {
  public:
    TimerId(Timestamp when, const std::weak_ptr<Timer> &timer) : when_(when), timer_(timer) {}

    bool Expired() const { return timer_.expired(); }
    std::shared_ptr<Timer> Get() const { return timer_.lock(); }
    Timestamp when() const { return when_; }

  private:
    Timestamp when_;
    std::weak_ptr<Timer> timer_;
};

class Timer : Noncopyable {
  public:
    using CallBack = std::function<void()>;

    Timer(const CallBack &cb, const Timestamp &when, const Duration &interval)
        : cb_(cb), when_(when), interval_(interval), repeat_(!interval.IsZero()), canceled_(false) {
        ++timer_num_;
    }
    ~Timer() { --timer_num_; }

    void Run() const { cb_(); }
    void Restart(const Timestamp &when) {
        if (repeat_) {
            when_ = when + interval_;
        } else {
            when_ = Timestamp(0);
        }
    }
    void Cancel() {
        assert(!canceled_);
        canceled_ = true;
    }

    Timestamp when() const { return when_; }
    bool repeat() const { return repeat_; }
    bool canceled() const { return canceled_; }

    static int64_t timer_num() { return timer_num_.Get(); }

  private:
    static AtomicInt64 timer_num_;

    const CallBack cb_;
    Timestamp when_;
    const Duration interval_;
    const bool repeat_;
    bool canceled_;
};

class TimerManager : Noncopyable {
  public:
    using CallBack = std::function<void()>;
    using TimerEntry = std::shared_ptr<Timer>;
    using TimerHeap = std::vector<TimerEntry>;

    explicit TimerManager(ObserverPtr<EventLoop> loop);

    TimerId AddTimer(CallBack cb, const Timestamp &when, const Duration &interval);
    void Cancel(const TimerId &timerid);
    void HandleExpired();
    int64_t NextTimeout();

  private:
    void PushInHeap(const TimerEntry &timer_entry);
    void PopInHeap();
    void UpInHeap(size_t index);
    void DownInHeap(size_t index);
    void SwapInHeap(size_t l, size_t r);

    ObserverPtr<EventLoop> loop_;
    TimerHeap heap_;
};

}  // namespace windz

#endif  // WINDZ_TIMERQUEUE_H
