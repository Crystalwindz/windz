//
// Created by crystalwind on 19-2-10.
//

#ifndef WINDZ_ASYNCLOGGING_H
#define WINDZ_ASYNCLOGGING_H

#include "Noncopyable.h"
#include "LogStream.h"
#include "Thread.h"
#include "CountDownLatch.h"
#include "Condition.h"
#include "Mutex.h"
#include "Atomic.h"
#include <vector>
#include <string>
#include <memory>

namespace windz {

class AsyncLogging : private Noncopyable {
  public:
    AsyncLogging(const std::string &basename,
    off_t roll_size, int flush_interval = 3);

    ~AsyncLogging();

    void Append(const char *logline, size_t len);
    void Start();
    void Stop();

  private:
    void ThreadFunc();

    using Buffer = FixBuffer<kLargeBuffer>;
    using BufferVector = std::vector<std::unique_ptr<Buffer>>;
    using BufferPtr = BufferVector::value_type;

    AtomicBool running_;
    const std::string basename_;
    const off_t roll_size_;
    const int flush_interval_;

    Thread thread_;
    CountDownLatch latch_;

    Mutex mutex_;
    Condition cond_;
    BufferPtr current_buffer_;
    BufferPtr next_buffer_;
    BufferVector buffers_;
};

}  // namespace windz

#endif //WINDZ_ASYNCLOGGING_H
