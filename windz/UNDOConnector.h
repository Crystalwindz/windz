//
// Created by crystalwind on 19-1-31.
//

#ifndef WINDZ_CONNECTOR_H
#define WINDZ_CONNECTOR_H

#include "Util.h"
#include "Socket.h"
#include "Channel.h"
#include <memory>
#include <functional>

namespace windz {

class EventLoop;

class UNDOConnector : private noncopyable,
                  public std::enable_shared_from_this<UNDOConnector> {
  public:
    using ConnCallBack = std::function<void (const Socket &)>;

    UNDOConnector(EventLoop *loop, const InetAddr &addr);
    ~UNDOConnector();

    void Start();
    void Stop();

    const InetAddr &addr() { return addr_; }
    void SetConnCallBack(const ConnCallBack &cb) { conncb_ = cb; }

  private:
    enum Status { kDisconnected, kConnecting, kConnected };
    static const int kMaxRetryTimeMs = 30*1000;
    static const int kInitRetryTimeMs = 500;

    void Connect();
    void Connecting(const Socket &sockfd);
    void Retry(const Socket &sockfd);
    void HandleWrite();
    void HandleError();

    ObserverPtr<EventLoop> loop_;
    InetAddr addr_;
    bool connect_;
    Status status_;
    ChannelSPtr channel_;
    ConnCallBack conncb_;
    int retry_timems_;
};

}  // namespace windz

#endif //WINDZ_CONNECTOR_H
