#ifndef WINDZ_MEMORY_H
#define WINDZ_MEMORY_H

#include "Noncopyable.h"

#include <functional>
#include <memory>

namespace windz {

class ExitCaller : Noncopyable {
  public:
    explicit ExitCaller(std::function<void()> &&functor) : functor_(std::move(functor)) {}
    ~ExitCaller() { functor_(); }

  private:
    std::function<void()> functor_;
};

template <typename T>
class ObserverPtr {
  public:
    ObserverPtr(T *ptr = nullptr) : ptr_(ptr) {}
    explicit ObserverPtr(const std::shared_ptr<T> &ptr) : ptr_(ptr.get()) {}

    T *Release() {
        T *ret = ptr_;
        ptr_ = nullptr;
        return ret;
    }
    void Reset(T *ptr = nullptr) { ptr_ = ptr; }
    T *Get() const { return ptr_; }
    T &operator*() const { return *ptr_; }
    T *operator->() const { return ptr_; }
    bool operator==(const T *ptr) const { return ptr == ptr_; }
    explicit operator bool() const { return Get() != nullptr; }

  private:
    T *ptr_;
};

template <typename T, typename... Ts>
std::unique_ptr<T> MakeUnique(Ts &&... params) {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

}  // namespace windz

#endif  // WINDZ_MEMORY_H
