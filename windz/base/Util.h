#ifndef WINDZ_UTIL_H
#define WINDZ_UTIL_H

#include "windz/base/Noncopyable.h"

#include <string>

namespace windz {

namespace util {

std::string Format(const char *fmt, ...);
int AddFdFlag(int fd, int flag);
int SetNonBlockAndCloseOnExec(int fd);

}  // namespace util

namespace net {

ssize_t Readn(int fd, void *buf, size_t n);
ssize_t ReadFd(int fd, std::string &buf);
ssize_t Writen(int fd, const void *buf, size_t n);
ssize_t WriteFd(int fd, std::string &buf);

}  // namespace net

}  // namespace windz

#endif  // WINDZ_UTIL_H
