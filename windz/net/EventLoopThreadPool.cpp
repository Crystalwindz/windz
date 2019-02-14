#include "windz/net/EventLoopThreadPool.h"
#include "windz/net/EventLoop.h"
#include "windz/net/EventLoopThread.h"

#include <assert.h>

#include <memory>
#include <string>

namespace windz {

EventLoopThreadPool::EventLoopThreadPool()
    : started_(false),
      next_(0),
      tid_(currentthread::tid())
{}

EventLoopThreadPool::~EventLoopThreadPool() { /* empty */
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

ObserverPtr<EventLoop> EventLoopThreadPool::Next() {
    assert(tid_ == currentthread::tid());
    assert(started_);

    ObserverPtr<EventLoop> loop(nullptr);

    if (!loops_.empty()) {
        loop = loops_[next_];
        ++next_;
        if (next_ >= loops_.size()) {
            next_ = 0;
        }
    }
    return loop;
}

}  // namespace windz
