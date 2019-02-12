#include "ThreadPool.h"
#include "Memory.h"

#include <assert.h>

namespace windz {

ThreadPool::ThreadPool()
        : running_(false)
{ }

ThreadPool::~ThreadPool() {
    if (running_) {
        Stop();
    }
}

void ThreadPool::Start(size_t thread_num) {
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(thread_num);
    for (size_t i = 0; i < thread_num; ++i) {
        threads_.emplace_back(MakeUnique<Thread>(
                [this]{
                    while (running_) {
                        Task task = queue_.Pop();
                        if (task){
                            task();
                        }
                    }
                }));
        threads_[i]->Start();
    }
}

void ThreadPool::Stop() {
    running_ = false;
    queue_.WakeUp();
    for (const auto &i: threads_) {
        i->Join();
    }
    threads_.clear();
    queue_.Restore();
}

void ThreadPool::AddTask(const Task &task) {
    queue_.Push(task);
}

void ThreadPool::AddTask(Task &&task) {
    queue_.Push(std::move(task));
}

}  // namespace windz