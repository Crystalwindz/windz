#ifndef WINDZ_EVENTLOOPTHREADPOOL_H
#define WINDZ_EVENTLOOPTHREADPOOL_H

#include "EventLoopThread.h"
#include "Noncopyable.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace windz {

class EventLoopThreadPool : Noncopyable {
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

#endif  // WINDZ_EVENTLOOPTHREADPOOL_H
