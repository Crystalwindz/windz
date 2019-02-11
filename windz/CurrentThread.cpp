//
// Created by crystalwind on 18-12-29.
//

#include "CurrentThread.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>

namespace windz {

namespace currentthread {

__thread pid_t t_tid = 0;
__thread char t_tid_str[32];
__thread int t_tid_str_len = 0;
__thread const char *t_name = "unknown";

namespace {

pid_t GetTid() {
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

}  // namespace

void CacheTid() {
    if (t_tid == 0) {
        t_tid = GetTid();
        t_tid_str_len = snprintf(t_tid_str, sizeof(t_tid_str), "%5d ", t_tid);
    }
}

bool IsMainThread() {
    return tid() == ::getpid();
}

}  // namespace currentthread

}  // namespace windz