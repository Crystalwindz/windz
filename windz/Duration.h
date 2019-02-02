//
// Created by crystalwind on 19-1-23.
//

#ifndef WINDZ_DURATION_H
#define WINDZ_DURATION_H

#include <stdint.h>
#include <sys/time.h>

namespace windz {

class Duration {
  public:
    static const int64_t kNanosecond = 1LL;
    static const int64_t kMicrosecond = 1000 * kNanosecond;
    static const int64_t kMillisecond = 1000 * kMicrosecond;
    static const int64_t kSecond = 1000 * kMillisecond;
    static const int64_t kMinute = 60 * kSecond;
    static const int64_t kHour = 60 * kMinute;

    Duration() : nanosecond_(0) {}
    explicit Duration(int64_t nanosecond) : nanosecond_(nanosecond) {}
    explicit Duration(int nanosecond) : nanosecond_(nanosecond) {}
    explicit Duration(double second) : nanosecond_((int64_t)(second * kSecond)) {}
    explicit Duration(const struct timeval &t)
            : nanosecond_(t.tv_sec * kSecond + t.tv_usec * kMicrosecond) {}

    int64_t Nanoseconds() const { return nanosecond_; }
    double Microseconds() const { return (double)(nanosecond_) / kMicrosecond; }
    double Milliseconds() const { return (double)(nanosecond_) / kMillisecond; }
    double Seconds() const { return (double)(nanosecond_) / kSecond; }
    double Minutes() const { return (double)(nanosecond_) / kMinute; }
    double Hours() const { return (double)(nanosecond_) / kHour; }

    struct timeval TimeVal() const {
        struct timeval t;
        t.tv_sec = (long)(nanosecond_ / kSecond);
        t.tv_usec = (long)(nanosecond_ % kSecond) / (long)(kMicrosecond);
        return t;
    }

    bool IsZero() const { return nanosecond_ == 0; }
    bool operator< (const Duration &other) const { return nanosecond_ <  other.nanosecond_; }
    bool operator<=(const Duration &other) const { return nanosecond_ <= other.nanosecond_; }
    bool operator> (const Duration &other) const { return nanosecond_ >  other.nanosecond_; }
    bool operator>=(const Duration &other) const { return nanosecond_ >= other.nanosecond_; }
    bool operator==(const Duration &other) const { return nanosecond_ == other.nanosecond_; }
    bool operator!=(const Duration &other) const { return nanosecond_ != other.nanosecond_; }

    Duration operator+=(const Duration &other) {
        nanosecond_ += other.nanosecond_;
        return *this;
    }
    Duration operator-=(const Duration &other) {
        nanosecond_ -= other.nanosecond_;
        return *this;
    }
    Duration operator*=(int n) {
        nanosecond_ *= n;
        return *this;
    }
    Duration operator/=(int n) {
        nanosecond_ /= n;
        return *this;
    }

  private:
    int64_t nanosecond_;
};

}  // namespace windz

#endif //WINDZ_DURATION_H
