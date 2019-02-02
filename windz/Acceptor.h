//
// Created by crystalwind on 19-1-30.
//

#ifndef WINDZ_ACCEPTOR_H
#define WINDZ_ACCEPTOR_H

#include "Util.h"
#include "Channel.h"
#include "Socket.h"
#include <functional>

namespace windz {

class EventLoop;

class Acceptor : private noncopyable {
  public:
    using ConnCallBack = std::function<void(const Socket &, const InetAddr &)>;

    Acceptor(EventLoop *loop, const InetAddr &addr, bool reuseport = false);
    ~Acceptor();

    void Listen();

    bool listenning() { return listenning_; }
    void SetConnCallBack(const ConnCallBack &cb) { conncb_ = cb; }

  private:
    void HandleRead();

    ObserverPtr<EventLoop> loop_;
    Socket socket_;
    ChannelSPtr channel_;
    ConnCallBack conncb_;
    bool listenning_;
    int idle_fd_;
};

}  // namespace windz

#endif //WINDZ_ACCEPTOR_H
