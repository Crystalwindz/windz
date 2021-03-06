#ifndef WINDZ_TCPSERVER_H
#define WINDZ_TCPSERVER_H

#include "windz/base/Atomic.h"
#include "windz/base/Noncopyable.h"
#include "windz/net/Acceptor.h"
#include "windz/net/CallBack.h"
#include "windz/net/EventLoop.h"
#include "windz/net/EventLoopThreadPool.h"
#include "windz/net/Socket.h"
#include "windz/net/TcpConnection.h"

#include <map>
#include <memory>
#include <string>

namespace windz {

class TcpServer : Noncopyable {
  public:
    TcpServer(ObserverPtr<EventLoop> loop, const InetAddr &addr, const std::string &name,
              bool reuseport = false);
    ~TcpServer();

    void Start(size_t thread_num = 0);

    void SetConnectionCallBack(const ConnectionCallBack &cb) { connection_cb_ = cb; }
    void SetMessageCallBack(const MessageCallBack &cb) { message_cb_ = cb; }

    std::string name() { return name_; };
    std::string ip_port() { return ip_port_; }

  private:
    void NewTcpConnection(const Socket &socket, const InetAddr &peer_addr);
    void RemoveTcpConnection(const TcpConnectionPtr &conn);
    using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

    ObserverPtr<EventLoop> loop_;
    std::string name_;
    std::string ip_port_;
    std::unique_ptr<Acceptor> acceptor_;
    std::unique_ptr<EventLoopThreadPool> thread_pool_;
    ConnectionCallBack connection_cb_;
    MessageCallBack message_cb_;
    AtomicInt32 started_;
    int next_connid_;
    ConnectionMap connections_;
};

}  // namespace windz

#endif  // WINDZ_TCPSERVER_H
