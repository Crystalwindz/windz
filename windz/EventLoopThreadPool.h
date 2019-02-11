//
// Created by crystalwind on 18-12-31.
//

#ifndef WINDZ_EVENTLOOPTHREADPOOL_H
#define WINDZ_EVENTLOOPTHREADPOOL_H

#include "Noncopyable.h"
#include "Util.h"
#include "EventLoopThread.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace windz {

class EventLoopThreadPool : private Noncopyable {
  public:
    explicit EventLoopThreadPool();
    ~EventLoopThreadPool();

    void Start(size_t thread_num);

    ObserverPtr<EventLoop> Next();

  private:
    bool started_;
    size_t next_;
    pid_t tid_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<ObserverPtr<EventLoop>> loops_;
};

}  // namespace windz

#endif //WINDZ_EVENTLOOPTHREADPOOL_H
