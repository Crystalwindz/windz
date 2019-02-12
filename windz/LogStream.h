#ifndef WINDZ_LOGSTREAM_H
#define WINDZ_LOGSTREAM_H

#include "Noncopyable.h"

#include <string.h>

#include <string>

namespace windz {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template <int SIZE>
class FixBuffer : Noncopyable {
  public:
    FixBuffer() : cur_(data_) {}
    ~FixBuffer() = default;

    void Append(const char *data, size_t len) {
        if (Avail() > len) {
            memcpy(cur_, data, len);
            cur_ += len;
        }
    }

    const char *data() const { return data_; };
    size_t Length() const { return static_cast<size_t>(cur_ - data_); }

    char *cur() { return cur_; }
    size_t Avail() const { return static_cast<size_t>(end() - cur_); }
    void Add(size_t len) { cur_ += len; }

    void Reset() { cur_ = data_; }
    void Bzero() { memset(data_, 0, sizeof(data_)); }

  private:
    const char *end() const { return data_ + sizeof(data_); }

    char data_[SIZE];
    char *cur_;
};

class LogStream : Noncopyable {
  public:
    using Buffer = FixBuffer<kSmallBuffer>;

    LogStream &operator<<(bool);

    LogStream &operator<<(short);
    LogStream &operator<<(unsigned short);
    LogStream &operator<<(int);
    LogStream &operator<<(unsigned int);
    LogStream &operator<<(long);
    LogStream &operator<<(unsigned long);
    LogStream &operator<<(long long);
    LogStream &operator<<(unsigned long long);

    LogStream &operator<<(float);
    LogStream &operator<<(double);
    LogStream &operator<<(long double);

    LogStream &operator<<(char);
    LogStream &operator<<(const char *);
    LogStream &operator<<(const unsigned char *);
    LogStream &operator<<(const std::string &);

    LogStream &operator<<(const void *);

    void Append(const char *data, int len) { buffer_.Append(data, len); };
    const Buffer &buffer() const { return buffer_; }
    void ResetBuffer() { buffer_.Reset(); }

  private:
    template <typename T>
    void FormatInteger(T);

    Buffer buffer_;

    static const int kMaxNumericSize = 32;
};

}  // namespace windz

#endif  // WINDZ_LOGSTREAM_H
