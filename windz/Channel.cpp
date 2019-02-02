//
// Created by crystalwind on 18-12-26.
//

#include "Channel.h"
#include "EventLoop.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>

namespace windz {

Channel::Channel(EventLoop *loop, int fd)
        : loop_(loop), fd_(fd), events_(0), revents_(0),
          event_handling_(false) {
    /* empty */
}

Channel::~Channel() {
    assert(!event_handling_);
}

void Channel::HandleEvents() {
    event_handling_ = true;
    if ((revents_ & EPOLLHUP) && !(revents_& EPOLLIN)) {
        if (closecb_) {
            closecb_();
        }
    }

    if (revents_ & EPOLLERR) {
        if (errorcb_) {
            errorcb_();
        }
    }

    if (revents_ & (EPOLLIN | EPOLLRDHUP)) {
        if (readcb_) {
            readcb_();
        }
    }

    if (revents_ & EPOLLOUT) {
        if (writecb_) {
            writecb_();
        }
    }
    event_handling_ = false;
}

void Channel::Close() {
    assert(events_ == 0);
    readcb_ = writecb_ = errorcb_ = closecb_ = nullptr;
    loop_->RemoveChannel(shared_from_this());
}

void Channel::Update() {
    loop_->UpdateChannel(shared_from_this());
}

}  // namespace windz