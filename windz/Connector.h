//
// Created by crystalwind on 19-1-31.
//

#ifndef WINDZ_CONNECTOR_H
#define WINDZ_CONNECTOR_H

#include "Util.h"
#include "Socket.h"
#include "Channel.h"
#include "Duration.h"
#include "Atomic.h"
#include <memory>
#include <functional>

namespace windz {

class EventLoop;

class Connector : private noncopyable,
                  public std::enable_shared_from_this<Connector> {
  public:
    using ConnectionCallBack = std::function<void (const Socket &)>;

    Connector(EventLoop *loop, const InetAddr &addr);
    ~Connector();

    void Start();
    void Stop();
    //void Restart();

    const InetAddr &addr() { return addr_; }
    void SetConnectionCallBack(const ConnectionCallBack &cb) { conncb_ = cb; }

  private:
    enum State { kDisconnected, kConnecting, kConnected };
    static const Duration kMaxRetryDuration;
    static const Duration kInitRetryDuration;

    void Connect();
    void Connecting(const Socket &sockfd);
    void Retry(const Socket &sockfd);
    void HandleWrite();
    void HandleError();
    Socket ResetChannel();

    ObserverPtr<EventLoop> loop_;
    InetAddr addr_;
    AtomicInt32 start_;
    State state_;
    ChannelSPtr channel_;
    ConnectionCallBack conncb_;
    Duration retry_duration_;
};

}  // namespace windz

#endif //WINDZ_CONNECTOR_H
