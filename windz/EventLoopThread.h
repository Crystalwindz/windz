//
// Created by crystalwind on 18-12-31.
//

#ifndef WINDZ_EVENTLOOPTHREAD_H
#define WINDZ_EVENTLOOPTHREAD_H

#include "Noncopyable.h"
#include "Mutex.h"
#include "Condition.h"
#include "Thread.h"
#include <functional>
#include <string>

namespace windz{

class EventLoop;

class EventLoopThread : private Noncopyable {
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

#endif //WINDZ_EVENTLOOPTHREAD_H
