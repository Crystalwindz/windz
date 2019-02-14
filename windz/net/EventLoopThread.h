#ifndef WINDZ_EVENTLOOPTHREAD_H
#define WINDZ_EVENTLOOPTHREAD_H

#include "windz/base/Condition.h"
#include "windz/base/Mutex.h"
#include "windz/base/Noncopyable.h"
#include "windz/base/Thread.h"
#include "windz/net/EventLoop.h"

#include <functional>
#include <string>

namespace windz {

class EventLoopThread : Noncopyable {
  public:
    explicit EventLoopThread(const std::string &name = std::string());
    ~EventLoopThread();
    ObserverPtr<EventLoop> Start();

  private:
    void ThreadFunc();

    ObserverPtr<EventLoop> loop_;
    bool exiting_;
    Thread thread_;
    Mutex mutex_;
    Condition cond_;
};

}  // namespace windz

#endif  // WINDZ_EVENTLOOPTHREAD_H
