//
// Created by crystalwind on 18-12-29.
//

#ifndef WINDZ_ATOMIC_H
#define WINDZ_ATOMIC_H

#include "Util.h"
#include <stdint.h>

namespace windz {

template <typename T>
class Atomic : private noncopyable {
  public:
    Atomic() : value_(0) {}
    explicit Atomic(T value) : value_(value) {}

    T Get() {
        return __atomic_load_n(&value_, __ATOMIC_SEQ_CST);
    }
    T Set(T newvalue) {
        return __atomic_exchange_n(&value_, newvalue, __ATOMIC_SEQ_CST);
    }
    T GetAndAdd(T x) {
        return __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST);
    }
    T AddAndGet(T x) {
        return __atomic_add_fetch(&value_, x, __ATOMIC_SEQ_CST);
    }
    T operator++() {
        return AddAndGet(1);
    }
    T operator++(int) {
        return GetAndAdd(1);
    }
    T operator--() {
        return AddAndGet(-1);
    }
    T operator--(int) {
        return GetAndAdd(-1);
    }

  private:
    volatile T value_;
};

using AtomicInt32 = Atomic<int32_t>;
using AtomicInt64 = Atomic<int64_t>;

}  // namespace windz

#endif //WINDZ_ATOMIC_H
