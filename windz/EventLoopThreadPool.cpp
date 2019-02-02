//
// Created by crystalwind on 18-12-31.
//

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <assert.h>
#include <memory>
#include <string>

namespace windz {

EventLoopThreadPool::EventLoopThreadPool()
        : started_(false),
          next_(0),
          tid_(currentthread::tid())
{ }

EventLoopThreadPool::~EventLoopThreadPool() {
    /* empty */
}

void EventLoopThreadPool::Start(size_t thread_num) {
    assert(!started_);
    assert(tid_ == currentthread::tid());

    started_ = true;

    for (size_t i = 0; i < thread_num; i++) {
        EventLoopThread *thread = new EventLoopThread();
        threads_.emplace_back(thread);
        loops_.push_back(thread->Start());
    }
}

EventLoop *EventLoopThreadPool::Next() {
    assert(tid_ == currentthread::tid());
    assert(started_);

    EventLoop *loop = nullptr;

    if (!loops_.empty()) {
        loop = loops_[next_].Get();
        ++next_;
        if (next_ >= loops_.size()) {
            next_ = 0;
        }
    }
    return loop;
}

}  // namespace windz