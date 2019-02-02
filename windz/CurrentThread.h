//
// Created by crystalwind on 18-12-29.
//

#ifndef WINDZ_CURRENTTHREAD_H
#define WINDZ_CURRENTTHREAD_H

#include <stdint.h>
#include <unistd.h>

namespace windz {

namespace currentthread {

extern __thread pid_t t_tid;
extern __thread char t_tid_str[32];
extern __thread int t_tid_str_len;
extern __thread const char *t_name;

void CacheTid();
bool IsMainThread();

inline pid_t tid() {
    if (t_tid == 0) {
        CacheTid();
    }
    return t_tid;
}

inline const char *tid_str() {
    return t_tid_str;
}

inline int tid_str_len() {
    return t_tid_str_len;
}

inline const char *name() {
    return t_name;
}

}  // namespace currentthread

}  // namespace windz

#endif //WINDZ_CURRENTTHREAD_H
