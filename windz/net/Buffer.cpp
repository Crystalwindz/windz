#include "windz/net/Buffer.h"

#include <sys/uio.h>

namespace windz {

void Buffer::Swap(Buffer &other) {
    buffer_.swap(other.buffer_);
    std::swap(read_index_, other.read_index_);
    std::swap(write_index_, other.write_index_);
}

void Buffer::Release(size_t len) {
    assert(len <= ReadableBytes());
    if (len < ReadableBytes()) {
        read_index_ += len;
    } else {
        read_index_ = kPrependSize;
        write_index_ = kPrependSize;
    }
}

void Buffer::ReleaseUntil(const char *c) {
    assert(Peek() <= c);
    Release(c - Peek());
}

void Buffer::ReleaseAll() { Release(ReadableBytes()); }

std::string Buffer::ReadAll() { return Read(ReadableBytes()); }

std::string Buffer::Read(size_t len) {
    assert(len <= ReadableBytes());
    std::string s(Peek(), len);
    Release(len);
    return s;
}

void Buffer::Write(const char *data, size_t len) {
    if (WritableBytes() < len) {
        ExtendSpace(len);
    }
    assert(WritableBytes() >= len);
    std::copy(data, data + len, &buffer_[write_index_]);
    write_index_ += len;
}

void Buffer::Write(const std::string &s) { Write(s.c_str(), s.size()); }

void Buffer::UnWrite(size_t len) {
    assert(len <= ReadableBytes());
    write_index_ -= len;
}

void Buffer::Prepend(const void *data, size_t len) {
    assert(len <= PrependableBytes());
    read_index_ -= len;
    const char *p = static_cast<const char *>(data);
    std::copy(p, p + len, &buffer_[read_index_]);
}

void Buffer::Prepend(const std::string &s) { Prepend(s.c_str(), s.size()); }

void Buffer::Shrink(size_t reserve) {
    AlignPrepend();
    buffer_.resize(kPrependSize + ReadableBytes() + reserve);
    buffer_.shrink_to_fit();
}

ssize_t Buffer::ReadFd(int fd) {
    char extra_buf[65536];
    struct iovec iov[2];
    const size_t write_byte = WritableBytes();
    iov[0].iov_base = &buffer_[write_index_];
    iov[0].iov_len = write_byte;
    iov[1].iov_base = extra_buf;
    iov[1].iov_len = sizeof(extra_buf);
    const ssize_t n = readv(fd, iov, 2);
    if (n < 0) {
        return n;
    } else if (static_cast<size_t>(n) <= write_byte) {
        write_index_ += n;
    } else {
        write_index_ = buffer_.size();
        Write(extra_buf, n - write_byte);
    }

    return n;
}

ssize_t Buffer::ReadSocket(const windz::Socket &socket) { return ReadFd(socket.sockfd()); }

void Buffer::AlignPrepend() {
    size_t read_byte = ReadableBytes();
    if (read_index_ == kPrependSize) {
        return;
    } else if (read_index_ < kPrependSize) {
        if (buffer_.size() < read_byte + kPrependSize) {
            buffer_.resize(read_byte + kPrependSize);
        }
        std::copy_backward(&buffer_[read_index_], &buffer_[write_index_],
                           &buffer_[read_byte + kPrependSize]);
    } else {
        std::copy(&buffer_[read_index_], &buffer_[write_index_], &buffer_[kPrependSize]);
    }
    read_index_ = kPrependSize;
    write_index_ = read_index_ + read_byte;
}

void Buffer::ExtendSpace(size_t len) {
    AlignPrepend();
    if (WritableBytes() < len) {
        buffer_.resize(write_index_ + len);
    }
}

}  // namespace windz