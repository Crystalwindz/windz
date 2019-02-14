#include "windz/net/Epoller.h"
#include "windz/net/Channel.h"
#include "windz/net/EventLoop.h"

#include <assert.h>
#include <string.h>
#include <sys/epoll.h>

namespace windz {

Epoller::Epoller(ObserverPtr<EventLoop> loop) : loop_(loop) {
    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    assert(epoll_fd_ > 0);
}

Epoller::~Epoller() { /* empty */
}

void Epoller::AddChannel(ChannelOPtr ch) {
    assert(IsInLoopThread());
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->events();
    ev.data.ptr = ch.Get();
    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, ch->fd(), &ev);
}

void Epoller::UpdateChannel(ChannelOPtr ch) {
    assert(IsInLoopThread());
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->events();
    ev.data.ptr = ch.Get();
    epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, ch->fd(), &ev);
}

void Epoller::RemoveChannel(ChannelOPtr ch) {
    assert(IsInLoopThread());
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, ch->fd(), nullptr);
}

bool Epoller::IsInLoopThread() const { return loop_->IsInLoopThread(); }

void Epoller::LoopOnce(int wait_ms, ChannelVector &channel_vector) {
    int event_nums = epoll_wait(epoll_fd_, active_events_, kMaxEvents, wait_ms);
    for (int i = 0; i < event_nums; i++) {
        ChannelOPtr ch = ChannelOPtr(static_cast<Channel *>(active_events_[i].data.ptr));
        uint32_t events = active_events_[i].events;
        ch->set_revents(events);
        channel_vector.push_back(ch);
    }
}

}  // namespace windz