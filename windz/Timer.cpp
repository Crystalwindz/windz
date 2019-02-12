#include "Timer.h"
#include "Atomic.h"
#include "EventLoop.h"
#include "Timestamp.h"

#include <assert.h>
#include <string.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <memory>

namespace windz {

AtomicInt64 Timer::timer_num_;

TimerManager::TimerManager(ObserverPtr<EventLoop> loop) : loop_(loop) {}

TimerId TimerManager::AddTimer(CallBack cb, const Timestamp &when, const Duration &interval) {
    std::shared_ptr<Timer> timer = std::make_shared<Timer>(std::move(cb), when, interval);
    TimerEntry timer_entry(timer);

    loop_->RunInLoop([this, timer_entry] { PushInHeap(timer_entry); });

    return TimerId(timer->when(), timer);
}

void TimerManager::Cancel(const TimerId &timerid) {
    loop_->RunInLoop([timerid] {
        if (timerid.Expired()) {
            return;
        } else {
            timerid.Get()->Cancel();
        }
    });
}

void TimerManager::HandleExpired() {
    Timestamp now = Timestamp::Now();
    while (!heap_.empty()) {
        TimerEntry timer = heap_[0];
        if (timer->canceled()) {
            PopInHeap();
        } else if (timer->when() <= now) {
            timer->Run();
            if (timer->repeat()) {
                timer->Restart(Timestamp::Now());
                DownInHeap(0);
            } else {
                PopInHeap();
            }
        } else {
            break;
        }
    }
}

int64_t TimerManager::NextTimeout() {
    if (heap_.empty()) {
        return INT64_MAX;
    } else {
        Timestamp now = Timestamp::Now();
        Timestamp next_time = heap_[0]->when();
        Duration timeout = next_time - now;
        return (timeout < Duration(0)) ? 0 : static_cast<int64_t>(timeout.Milliseconds());
    }
}

void TimerManager::PushInHeap(const TimerEntry &timer_entry) {
    heap_.push_back(timer_entry);
    size_t index = heap_.size() - 1;
    UpInHeap(index);
}

void TimerManager::PopInHeap() {
    SwapInHeap(0, heap_.size() - 1);
    heap_.pop_back();
    DownInHeap(0);
}

void TimerManager::UpInHeap(size_t index) {
    size_t parent = (index - 1) / 2;
    while (index > 0 && heap_[index]->when() < heap_[parent]->when()) {
        SwapInHeap(index, parent);
        index = parent;
        parent = (index - 1) / 2;
    }
}

void TimerManager::DownInHeap(size_t index) {
    size_t child = index * 2 + 1;
    while (child < heap_.size()) {
        size_t min_child = child;
        if (child < heap_.size() - 1) {
            min_child = (heap_[child]->when() < heap_[child + 1]->when()) ? child : child + 1;
        }
        if (heap_[index]->when() <= heap_[min_child]->when()) {
            break;
        } else {
            SwapInHeap(index, min_child);
            index = min_child;
            child = index * 2 + 1;
        }
    }
}

void TimerManager::SwapInHeap(size_t l, size_t r) { heap_[l].swap(heap_[r]); }

}  // namespace windz