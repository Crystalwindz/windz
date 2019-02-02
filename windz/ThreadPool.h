//
// Created by crystalwind on 19-1-15.
//

#ifndef WINDZ_THREADPOOL_H
#define WINDZ_THREADPOOL_H

#include "windz/Util.h"
#include "windz/Mutex.h"
#include "windz/Thread.h"
#include "windz/BlockingQueue.h"
#include <functional>
#include <memory>
#include <vector>

namespace windz {

class ThreadPool : private noncopyable {
  public:
    using Task = std::function<void()>;

    ThreadPool();
    ~ThreadPool();

    void SetQueueMaxSize(size_t maxsize) { queue_.set_maxsize(maxsize); }

    void Start(size_t thread_num);
    void Stop();

    void AddTask(const Task &task);
    void AddTask(Task &&task);

  private:
    std::vector<std::unique_ptr<Thread>> threads_;
    BlockingQueue<Task> queue_;
    bool running_;
};

}  // namespace windz

#endif //WINDZ_THREADPOOL_H
