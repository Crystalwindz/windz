#ifndef WINDZ_TIMESTAMP_H
#define WINDZ_TIMESTAMP_H

#include "windz/base/Duration.h"

#include <sys/time.h>

namespace windz {

class Timestamp {
  public:
    Timestamp() : nanosecond_(0) {}
    explicit Timestamp(int64_t nanosecond) : nanosecond_(nanosecond) {}
    explicit Timestamp(const struct timeval &t)
        : nanosecond_(t.tv_sec * Duration::kSecond + t.tv_usec * Duration::kMicrosecond) {}

    static Timestamp Now() {
        struct timeval t;
        gettimeofday(&t, nullptr);
        return Timestamp(t);
    }
    struct timeval TimeVal() const {
        struct timeval t;
        t.tv_sec = (long)(nanosecond_ / Duration::kSecond);
        t.tv_usec = (long)(nanosecond_ % Duration::kSecond) / (long)(Duration::kMicrosecond);
        return t;
    }

    int64_t UnixNano() const { return nanosecond_ / Duration::kNanosecond; }
    int64_t UnixMicro() const { return nanosecond_ / Duration::kMicrosecond; }
    int64_t UnixMilli() const { return nanosecond_ / Duration::kMillisecond; }
    int64_t Unix() const { return nanosecond_ / Duration::kSecond; }

    bool operator<(const Timestamp &other) const { return nanosecond_ < other.nanosecond_; }
    bool operator<=(const Timestamp &other) const { return nanosecond_ <= other.nanosecond_; }
    bool operator>(const Timestamp &other) const { return nanosecond_ > other.nanosecond_; }
    bool operator>=(const Timestamp &other) const { return nanosecond_ >= other.nanosecond_; }
    bool operator==(const Timestamp &other) const { return nanosecond_ == other.nanosecond_; }
    bool operator!=(const Timestamp &other) const { return nanosecond_ != other.nanosecond_; }

    Timestamp operator+=(const Duration &other) {
        nanosecond_ += other.Nanoseconds();
        return *this;
    }
    Timestamp operator+(const Duration &other) const {
        Timestamp temp(*this);
        temp += other;
        return temp;
    }
    Timestamp operator-=(const Duration &other) {
        nanosecond_ -= other.Nanoseconds();
        return *this;
    }
    Timestamp operator-(const Duration &other) const {
        Timestamp temp(*this);
        temp -= other;
        return temp;
    }
    Duration operator-(const Timestamp &other) const {
        return Duration(nanosecond_ - other.nanosecond_);
    }

  private:
    int64_t nanosecond_;
};

}  // namespace windz

#endif  // WINDZ_TIMESTAMP_H
