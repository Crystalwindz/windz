//
// Created by crystalwind on 18-12-31.
//

#include "EventLoop.h"
#include "EventLoopThread.h"
#include <assert.h>

namespace windz {

EventLoopThread::EventLoopThread(const std::string &name)
    : loop_(nullptr),
      exiting_(false),
      thread_([this]{ ThreadFunc(); }, name),
      mutex_(),
      cond_(mutex_)
{ }

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_.Get() != nullptr) {
        loop_->Quit();
        thread_.Join();
    }
}

ObserverPtr<EventLoop> EventLoopThread::Start() {
    assert(!thread_.started());
    thread_.Start();

    ObserverPtr<EventLoop> loop;
    {
        LockGuard lock(mutex_);
        cond_.Wait([this]{ return loop_.Get() != nullptr; });
        loop = loop_;
    }

    return loop;
}

void EventLoopThread::ThreadFunc() {
    EventLoop loop;

    {
        LockGuard lock(mutex_);
        loop_.Reset(&loop);
        cond_.Notify();
    }

    loop.Loop();

    LockGuard lock(mutex_);
    loop_.Reset(nullptr);
}

}  // namespace windz