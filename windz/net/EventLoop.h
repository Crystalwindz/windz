#ifndef WINDZ_EVENTLOOP_H
#define WINDZ_EVENTLOOP_H

#include "windz/base/BlockingQueue.h"
#include "windz/base/Memory.h"
#include "windz/base/Mutex.h"
#include "windz/base/Noncopyable.h"
#include "windz/base/Thread.h"
#include "windz/base/Timestamp.h"
#include "windz/net/Channel.h"
#include "windz/net/Epoller.h"
#include "windz/net/Timer.h"

#include <unistd.h>

#include <functional>
#include <memory>
#include <set>
#include <vector>

namespace windz {

class EventLoop : Noncopyable {
  public:
    using Functor = std::function<void()>;
    using ChannelSPtr = std::shared_ptr<Channel>;
    using ChannelOPtr = windz::ObserverPtr<Channel>;
    using ChannelSet = std::set<ChannelSPtr>;
    using ChannelVector = std::vector<ChannelOPtr>;

    EventLoop();
    ~EventLoop();

    void Loop();
    void Quit();

    void RunInLoop(Functor func);
    void QueueInLoop(Functor func);

    TimerId RunAt(const Timestamp &when, Functor func);
    TimerId RunAfter(const Duration &delay, Functor func);
    TimerId RunEvery(const Duration &interval, Functor func);
    void CancelTimer(const TimerId &timerid);

    void WakeUp();
    void AddChannel(const ChannelSPtr &ch);
    void UpdateChannel(const ChannelSPtr &ch);
    void RemoveChannel(const ChannelSPtr &ch);
    bool HasChannel(const ChannelSPtr &ch);

    bool IsInLoopThread() const { return tid_ == currentthread::tid(); }
    void AssertInLoopThread() const { assert(IsInLoopThread()); }

    static ObserverPtr<EventLoop> GetThisThreadEventLoopPtr();

  private:
    void HandleRead();
    void CallPendingFunctors();

    bool looping_;
    bool quit_;
    bool event_handling_;
    bool pending_functors_calling_;
    const pid_t tid_;

    std::unique_ptr<Epoller> epoller_;
    std::unique_ptr<TimerManager> timer_manager_;
    ChannelSet channels_;
    ChannelVector active_channels_;

    int wakeup_fd_;
    ChannelSPtr wakeup_channel_;

    BlockingQueue<Functor> pending_functors_;
};

}  // namespace windz

#endif  // WINDZ_EVENTLOOP_H
