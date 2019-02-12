#include "AsyncLogging.h"
#include "LogFile.h"
#include "Memory.h"

#include <assert.h>

namespace windz {

AsyncLogging::AsyncLogging(const std::string &basename,
                           off_t roll_size,
                           int flush_interval)
        : running_(false),
          basename_(basename),
          roll_size_(roll_size),
          flush_interval_(flush_interval),
          thread_([this]{ ThreadFunc(); }, "Logging") ,
          latch_(1),
          mutex_(),
          cond_(mutex_),
          current_buffer_(MakeUnique<Buffer>()),
          next_buffer_(MakeUnique<Buffer>()),
          buffers_() {
    current_buffer_->Bzero();
    next_buffer_->Bzero();
    buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging() {
    if (running_) {
        Stop();
    }
}

void AsyncLogging::Append(const char *logline, size_t len) {
    LockGuard lock(mutex_);
    if (current_buffer_->Avail() > len) {
        current_buffer_->Append(logline, len);
    } else {
        buffers_.push_back(std::move(current_buffer_));
        if (next_buffer_) {
            current_buffer_ = std::move(next_buffer_);
        }  else {
            current_buffer_ = MakeUnique<Buffer>();
        }
        current_buffer_->Append(logline, len);
        cond_.Notify();
    }
}

void AsyncLogging::Start() {
    running_.SetTrue();
    thread_.Start();
    latch_.Wait();
}

void AsyncLogging::Stop() {
    running_.SetFalse();
    cond_.Notify();
    thread_.Join();
}

void AsyncLogging::ThreadFunc() {
    assert(running_);
    latch_.CountDown();
    LogFile output(basename_, roll_size_, false);
    auto new_buffer1 = MakeUnique<Buffer>();
    auto new_buffer2 = MakeUnique<Buffer>();
    BufferVector buffers_to_write;
    new_buffer1->Bzero();
    new_buffer2->Bzero();
    buffers_to_write.reserve(16);
    while (running_) {
        assert(new_buffer1 && new_buffer1->Length() == 0);
        assert(new_buffer2 && new_buffer2->Length() == 0);
        assert(buffers_to_write.empty());
        {
            LockGuard lock(mutex_);
            if (buffers_.empty()) {
                cond_.TimedWait(flush_interval_);
            }
            buffers_.push_back(std::move(current_buffer_));
            current_buffer_ = std::move(new_buffer1);
            buffers_to_write.swap(buffers_);
            if (!next_buffer_) {
                next_buffer_ = std::move(new_buffer2);
            }
        }
        assert(!buffers_to_write.empty());
        if (buffers_to_write.size() > 25) {
            char buf[256];
            snprintf(buf, sizeof(buf), "Dropped log message, %zd buffers\n",
            buffers_to_write.size() - 2);
            fputs(buf, stderr);
            output.Append(buf, strlen(buf));
            buffers_to_write.erase(buffers_to_write.begin()+2, buffers_to_write.end());
        }
        for (const auto &buffer: buffers_to_write) {
            output.Append(buffer->data(), buffer->Length());
        }
        if (buffers_to_write.size() > 2) {
            buffers_to_write.resize(2);
        }
        if (!new_buffer1) {
            assert(!buffers_to_write.empty());
            new_buffer1 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            new_buffer1->Reset();
        }
        if (!new_buffer2) {
            assert(!buffers_to_write.empty());
            new_buffer2 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            new_buffer2->Reset();
        }
        buffers_to_write.clear();
        output.Flush();
    }
    output.Flush();
}

}  // namespace windz