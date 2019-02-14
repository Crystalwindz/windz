#include "windz/base/Mutex.h"
#include "windz/net/EventLoop.h"
#include "windz/net/Channel.h"
#include "windz/net/Epoller.h"
#include "windz/net/Timer.h"
#include "windz/log/Logger.h"

#include <assert.h>
#include <sys/eventfd.h>

#include <algorithm>
#include <set>

namespace windz {

__thread EventLoop *t_eventloop_ptr = nullptr;

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      event_handling_(false),
      pending_functors_calling_(false),
      tid_(currentthread::tid()),
      epoller_(MakeUnique<Epoller>(this)),
      timer_manager_(MakeUnique<TimerManager>(this)) {
    if (t_eventloop_ptr) {
        LOG_FATAL << "another EventLoop " << t_eventloop_ptr
                  << " exists in this thread " << tid_;
    } else {
        t_eventloop_ptr = this;
        wakeup_fd_ = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if (wakeup_fd_ < 0) {
            LOG_SYSFATAL << "create eventfd failed";
        }
        wakeup_channel_ = std::make_shared<Channel>(this, wakeup_fd_);
        wakeup_channel_->SetReadHandler([this] { this->HandleRead(); });
        wakeup_channel_->EnableRead();
    }
}

EventLoop::~EventLoop() {
    assert(!looping_);
    t_eventloop_ptr = nullptr;
    while (!channels_.empty()) {
        ChannelSPtr ch = *channels_.begin();
        ch->DisableReadWrite();
        ch->Close();
    }
}

void EventLoop::Loop() {
    assert(!looping_);
    assert(IsInLoopThread());
    looping_ = true;
    quit_ = false;

    while (!quit_) {
        active_channels_.clear();
        int64_t waitms = std::min(int64_t(10000), timer_manager_->NextTimeout());
        epoller_->LoopOnce(waitms, active_channels_);
        for (const auto &i : active_channels_) {
            i->HandleEvents();
        }
        timer_manager_->HandleExpired();
        CallPendingFunctors();
    }
    looping_ = false;
}

void EventLoop::Quit() {
    quit_ = true;
    if (!IsInLoopThread()) {
        WakeUp();
    }
}

void EventLoop::RunInLoop(Functor func) {
    if (IsInLoopThread()) {
        func();
    } else {
        QueueInLoop(func);
    }
}

void EventLoop::QueueInLoop(Functor func) {
    pending_functors_.Push(std::move(func));
    if (!IsInLoopThread() || pending_functors_calling_) {
        WakeUp();
    }
}

TimerId EventLoop::RunAt(const Timestamp &when, Functor func) {
    return timer_manager_->AddTimer(std::move(func), when, Duration(0));
}

TimerId EventLoop::RunAfter(const Duration &delay, Functor func) {
    return timer_manager_->AddTimer(std::move(func), Timestamp::Now() + delay, Duration(0));
}

TimerId EventLoop::RunEvery(const Duration &interval, Functor func) {
    return timer_manager_->AddTimer(std::move(func), Timestamp::Now() + interval, interval);
}

void EventLoop::CancelTimer(const TimerId &timerid) { timer_manager_->Cancel(timerid); }

void EventLoop::WakeUp() {
    uint64_t one = 1;
    ssize_t n = write(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR << "EventLoop::WakeUp() write " << n << " bytes instead of 8";
    }
}

void EventLoop::AddChannel(const ChannelSPtr &ch) {
    assert(ch->loop() == this);
    assert(IsInLoopThread());
    channels_.insert(ch);
    epoller_->AddChannel(ChannelOPtr(ch));
}

void EventLoop::UpdateChannel(const ChannelSPtr &ch) {
    assert(ch->loop() == this);
    assert(IsInLoopThread());
    if (HasChannel(ch)) {
        epoller_->UpdateChannel(ChannelOPtr(ch));
    } else {
        AddChannel(ch);
    }
}

void EventLoop::RemoveChannel(const ChannelSPtr &ch) {
    assert(ch->loop() == this);
    assert(IsInLoopThread());
    channels_.erase(ch);
    epoller_->RemoveChannel(ChannelOPtr(ch));
}

bool EventLoop::HasChannel(const ChannelSPtr &ch) {
    auto iter = channels_.find(ch);
    return iter != channels_.end();
}

ObserverPtr<EventLoop> EventLoop::GetThisThreadEventLoopPtr() { return t_eventloop_ptr; }

void EventLoop::HandleRead() {
    uint64_t one = 1;
    ssize_t n = read(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR << "EventLoop::HandleRead() read " << n << " bytes instead 8";
    }
}

void EventLoop::CallPendingFunctors() {
    pending_functors_calling_ = true;

    size_t size = pending_functors_.Size();
    for (size_t i = 0; i < size; ++i) {
        Functor func = pending_functors_.Pop();
        func();
    }

    pending_functors_calling_ = false;
}

}  // namespace windz