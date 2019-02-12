#ifndef WINDZ_ACCEPTOR_H
#define WINDZ_ACCEPTOR_H

#include "Noncopyable.h"
#include "Channel.h"
#include "Socket.h"

#include <functional>

namespace windz {

class EventLoop;

class Acceptor : Noncopyable {
  public:
    using ConnectionCallBack = std::function<void(const Socket &, const InetAddr &)>;

    Acceptor(ObserverPtr<EventLoop> loop, const InetAddr &addr, bool reuseport = false);
    ~Acceptor();

    void Listen();

    bool listenning() { return listenning_; }
    void SetConnecionCallBack(const ConnectionCallBack &cb) { conn_cb_ = cb; }

  private:
    void HandleRead();

    ObserverPtr<EventLoop> loop_;
    Socket socket_;
    ChannelSPtr channel_;
    ConnectionCallBack conn_cb_;
    bool listenning_;
    int idle_fd_;
};

}  // namespace windz

#endif //WINDZ_ACCEPTOR_H
