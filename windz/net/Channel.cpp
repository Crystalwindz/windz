#include "windz/net/Channel.h"
#include "windz/net/EventLoop.h"

#include <assert.h>
#include <sys/epoll.h>
#include <unistd.h>

namespace windz {

Channel::Channel(ObserverPtr<EventLoop> loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      event_handling_(false),
      tied_(false)
{}

Channel::~Channel() { assert(!event_handling_); }

void Channel::HandleEvents() {
    std::shared_ptr<void> guard;
    if (tied_) {
        guard = tie_.lock();
        if (!guard) {
            return;
        }
    }

    event_handling_ = true;
    if (revents_ & EPOLLERR) {
        if (error_cb_) {
            error_cb_();
        }
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (read_cb_) {
            read_cb_();
        }
    }
    if (revents_ & EPOLLOUT) {
        if (write_cb_) {
            write_cb_();
        }
    }
    event_handling_ = false;
}

void Channel::Close() {
    assert(events_ == 0);
    read_cb_ = write_cb_ = error_cb_ = nullptr;
    loop_->RemoveChannel(shared_from_this());
}

void Channel::Tie(const std::shared_ptr<void> &p) {
    tie_ = p;
    tied_ = true;
}

void Channel::Update() { loop_->UpdateChannel(shared_from_this()); }

}  // namespace windz