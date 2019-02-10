//
// Created by crystalwind on 18-12-25.
//

#include "Util.h"
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <functional>
#include <map>
#include <string>
#include <memory>

using namespace std;

namespace windz {

string util::Format(const char *fmt, ...) {
    char buf[500];
    unique_ptr<char[]> release;
    char *base;
    for (int i = 0; i < 2; i++) {
        int bufsize;
        if (i == 0) {
            bufsize = sizeof(buf);
            base = buf;
        } else {
            bufsize = 30000;
            base = new char[bufsize];
            release.reset(base);
        }
        char *p = base;
        char *limit = base + bufsize;
        if (p < limit) {
            va_list ap;
            va_start(ap, fmt);
            p += vsnprintf(p, limit - p, fmt, ap);
            va_end(ap);
        }
        if (p >= limit) {
            if (i == 0) {
                continue;  // 扩大buf，再试一次
            } else {
                p = limit - 1;
                *p = '\0';
            }
        }
        break;
    }
    return string(base);
}

int util::AddFdFlag(int fd, int flag) {
    int ret = fcntl(fd, F_GETFL);
    return fcntl(fd, F_SETFL, ret | flag);
}

int util::SetNonBlockAndCloseOnExec(int fd) {
    int ret = AddFdFlag(fd, O_NONBLOCK);
    if (ret) {
        return ret;
    }
    int flags = fcntl(fd, F_GETFD);
    return fcntl(fd, F_SETFD, flags | FD_CLOEXEC);

}

ssize_t net::Writen(int fd, const void *buf, size_t n) {
    size_t nleft = n;
    ssize_t nwritten = 0;
    ssize_t writesum = 0;
    char *p = (char *)buf;

    while(nleft > 0) {
        if ((nwritten = write(fd, p, nleft)) <= 0) {
            if (nwritten < 0) {
                if (errno == EINTR) {
                    nwritten = 0;
                    continue;
                } else {
                    return -1;
                }
            }
        }
        writesum += nwritten;
        nleft -= nwritten;
        p += nwritten;
    }

    return writesum;
}

ssize_t net::WriteFd(int fd, std::string &buf) {
    return Writen(fd, buf.c_str(), buf.size());
}

ssize_t net::Readn(int fd, void *buf, size_t n) {
    size_t nleft = n;
    ssize_t nread = 0;
    ssize_t readsum = 0;
    char *p = (char *)buf;

    while(nleft > 0) {
        if ((nread = read(fd, p, nleft)) < 0) {
            if (errno == EINTR) {
                nread = 0;
                continue;
            } else if (errno == EWOULDBLOCK){
                break;
            } else {
                return -1;
            }
        } else if (nread == 0) {
            break;
        }
        readsum += nread;
        nleft -= nread;
        p += nread;
    }

    return readsum;
}

ssize_t net::ReadFd(int fd, std::string &buf) {
    ssize_t nread = 0;
    ssize_t readsum = 0;
    while(true) {
        char buff[4096];
        if ((nread = read(fd, buff, 4096)) < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EWOULDBLOCK) {
                break;
            } else {
                return -1;
            }
        } else if (nread == 0) {
            break;
        }
        readsum += nread;
        buf += std::string(buff, buff + nread);
    }

    return readsum;
}

}  // namespace windz