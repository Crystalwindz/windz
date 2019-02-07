//
// Created by crystalwind on 18-12-25.
//

#ifndef WINDZ_UTIL_H
#define WINDZ_UTIL_H

#include <string>
#include <functional>
#include <memory>

namespace windz {

class noncopyable {
  protected:
    noncopyable() = default;
    ~noncopyable() = default;

    noncopyable(const noncopyable &) = delete;
    noncopyable &operator=(const noncopyable &) = delete;
};

class ExitCaller : private noncopyable {
  public:
    explicit ExitCaller(std::function<void()> &&functor) : functor_(std::move(functor)) {}
    ~ExitCaller() { functor_(); }

  private:
    std::function<void()> functor_;
};

template <typename T>
class ObserverPtr {
  public:
    explicit ObserverPtr(T *ptr = nullptr) : ptr_(ptr) {}
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

template<typename T, typename... Ts>
std::unique_ptr<T> MakeUnique(Ts&&... params)
{
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

namespace util {

std::string Format(const char *fmt, ...);
int AddFdFlag(int fd, int flag);
int SetNonBlockAndCloseOnExec(int fd);

}  // namespace util

namespace net {

ssize_t Readn(int fd, void *buf, size_t n);
ssize_t Readn(int fd, std::string &buf);
ssize_t Writen(int fd, const void *buf, size_t n);
ssize_t Writen(int fd, std::string &buf);

}  // namespace net

}  // namespace windz

#endif //WINDZ_UTIL_H
