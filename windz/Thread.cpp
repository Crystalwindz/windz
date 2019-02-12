#include "Thread.h"
#include "CurrentThread.h"

#include <assert.h>
#include <pthread.h>
#include <sys/prctl.h>

#include <string>
#include <utility>

namespace windz {

namespace {

class ThreadData {
  public:
    using ThreadFunc = windz::Thread::ThreadFunc;

    ThreadData(ThreadFunc func, std::string name, pid_t *tid, CountDownLatch *latch)
            : func_(std::move(func)), name_(std::move(name)), tid_(tid), latch_(latch)
    {}

    void RunInThread() {
        *tid_ = currentthread::tid();
        tid_ = nullptr;
        latch_->CountDown();
        latch_ = nullptr;
        currentthread::t_name = name_.empty() ? "default" : name_.c_str();

        func_();
        currentthread::t_name = "finished";
    }

  private:
    ThreadFunc func_;
    std::string name_;
    pid_t *tid_;
    CountDownLatch *latch_;
};

void *StartThread(void *arg) {
    ThreadData *data = static_cast<ThreadData *>(arg);
    data->RunInThread();
    delete data;
    return nullptr;
}

}  // namespace

AtomicInt32 Thread::thread_num_;

Thread::Thread(const ThreadFunc &func, const std::string &name)
    : started_(false), joined_(false), pthread_id_(0), tid_(0),
      func_(func), name_(name), latch_(1) {}

Thread::Thread(ThreadFunc &&func, const std::string &name)
        : started_(false), joined_(false), pthread_id_(0), tid_(0),
          func_(std::move(func)), name_(name), latch_(1) {}

Thread::~Thread() {
    if (started_ && !joined_) {
        pthread_detach(pthread_id_);
    }
}

bool Thread::Start() {
    assert(!started_);
    started_ = true;
    ThreadData *data = new ThreadData(func_, name_, &tid_, &latch_);
    if (pthread_create(&pthread_id_, nullptr, StartThread, data)) {
        started_ = false;
        delete data;
        return false;
    } else {
        latch_.Wait();// 确保线程真正开始后才返回
        return true;
    }
}

int Thread::Join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthread_id_, nullptr);
}

}  // namespace windz