//
// Created by crystalwind on 18-12-25.
//

#ifndef WINDZ_THREAD_H
#define WINDZ_THREAD_H

#include "Util.h"
#include "Atomic.h"
#include "CountDownLatch.h"
#include "CurrentThread.h"
#include <pthread.h>
#include <functional>
#include <string>

namespace windz {

class Thread : private noncopyable {
  public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(const ThreadFunc &func, const std::string &name = std::string());
    explicit Thread(ThreadFunc &&func, const std::string &name = std::string());
    ~Thread();

    bool Start();
    int Join();
    bool started() const { return started_; }
    bool joined() const { return joined_; }
    pid_t tid() const { return tid_; }
    const std::string &name() const { return name_; }

    static int thread_num() { return thread_num_.Get(); }

  private:
    static AtomicInt32 thread_num_;

    bool started_;
    bool joined_;
    pthread_t pthread_id_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;
};

}  //namespace windz

#endif //WINDZ_THREAD_H
