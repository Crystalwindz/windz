#ifndef WINDZ_EPOLLER_H
#define WINDZ_EPOLLER_H

#include "windz/base/Memory.h"
#include "windz/base/Noncopyable.h"
#include "windz/net/Channel.h"

#include <sys/epoll.h>

#include <memory>
#include <set>
#include <vector>

namespace windz {

class EventLoop;

class Epoller : Noncopyable {
  public:
    using ChannelOPtr = windz::ObserverPtr<Channel>;
    using ChannelVector = std::vector<ChannelOPtr>;

    explicit Epoller(ObserverPtr<EventLoop> loop);
    ~Epoller();

    void AddChannel(ChannelOPtr ch);
    void UpdateChannel(ChannelOPtr ch);
    void RemoveChannel(ChannelOPtr ch);
    bool IsInLoopThread() const;

    void LoopOnce(int wait_ms, ChannelVector &channel_vector);

  private:
    static const int kMaxEvents = 2000;

    int epoll_fd_;
    ObserverPtr<EventLoop> loop_;
    struct epoll_event active_events_[kMaxEvents];
};

}  // namespace windz

#endif  // WINDZ_EPOLLER_H
