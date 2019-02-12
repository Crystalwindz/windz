#ifndef WINDZ_CHANNEL_H
#define WINDZ_CHANNEL_H

#include "Noncopyable.h"
#include "Memory.h"

#include <sys/epoll.h>

#include <functional>
#include <memory>

namespace windz {

class EventLoop;

class Channel : Noncopyable, public std::enable_shared_from_this<Channel> {
  public:
    using EventCallBack = std::function<void()>;

    Channel(ObserverPtr<EventLoop> loop, int fd);
    ~Channel();

    void HandleEvents();
    void Close();

    void Tie(const std::shared_ptr<void> &p);

    void SetReadHandler(const EventCallBack &readcb) { read_cb_ = readcb; }
    void SetReadHandler(EventCallBack &&readcb) { read_cb_ = std::move(readcb); }
    void SetWriteHandler(const EventCallBack &writecb) { write_cb_ = writecb; }
    void SetWriteHandler(EventCallBack &&writecb) { write_cb_ = std::move(writecb); }
    void SetErrorHandler(const EventCallBack &errorcb) { error_cb_ = errorcb; }
    void SetErrorHandler(EventCallBack &&errorcb) { error_cb_ = std::move(errorcb); }

    void EnableRead() { events_ |= EPOLLIN; Update(); }
    void DisableRead() { events_ &= ~EPOLLIN; Update(); }
    void EnableWrite() { events_ |= EPOLLOUT; Update(); }
    void DisableWrite() { events_ &= ~EPOLLOUT; Update(); }
    void EnableReadWrite() { events_ |= (EPOLLIN | EPOLLOUT); Update(); }
    void DisableReadWrite() { events_ &= ~(EPOLLIN | EPOLLOUT); Update(); }
    bool ReadEnabled() const { return events_ & EPOLLIN; }
    bool WriteEnabled() const { return events_ & EPOLLOUT; }

    ObserverPtr<EventLoop> loop() const { return loop_; }
    int fd() const { return fd_; }
    uint32_t events() const { return events_; }
    void set_revents(uint32_t revents) { revents_ = revents; }

  private:
    void Update();

    ObserverPtr<EventLoop> loop_;
    int fd_;
    uint32_t events_;
    uint32_t revents_;

    bool event_handling_;
    EventCallBack read_cb_, write_cb_, error_cb_;

    bool tied_;
    std::weak_ptr<void> tie_;
};

using ChannelSPtr = std::shared_ptr<Channel>;
using ChannelOPtr = windz::ObserverPtr<Channel>;

}  // namespace windz

#endif //WINDZ_CHANNEL_H
