#ifndef WINDZ_BUFFER_H
#define WINDZ_BUFFER_H

#include "windz/net/Socket.h"

#include <assert.h>
#include <string.h>

#include <string>
#include <vector>

namespace windz {

class Buffer {
  public:
    static const size_t kPrependSize = 8;
    static const size_t kInitSize = 1024;

    explicit Buffer(size_t init_size = kInitSize)
        : buffer_(kPrependSize + init_size),
          read_index_(kPrependSize),
          write_index_(kPrependSize) {}

    void Swap(Buffer &other);

    size_t ReadableBytes() const { return write_index_ - read_index_; }
    size_t WritableBytes() const { return buffer_.size() - write_index_; }
    size_t PrependableBytes() const { return read_index_; }
    const char *Peek() const { return &buffer_[read_index_]; }

    void Release(size_t len);
    void ReleaseUntil(const char *c);
    void ReleaseAll();

    std::string Read(size_t len);
    std::string ReadAll();

    void Write(const char *data, size_t len);
    void Write(const std::string &s);
    void UnWrite(size_t len);

    void Prepend(const void *data, size_t len);
    void Prepend(const std::string &s);

    void Shrink(size_t reserve);
    ssize_t ReadFd(int fd);
    ssize_t ReadSocket(const Socket &socket);

  private:
    void AlignPrepend();
    void ExtendSpace(size_t len);

    std::vector<char> buffer_;
    size_t read_index_;
    size_t write_index_;
};

}  // namespace windz

#endif  // WINDZ_BUFFER_H
