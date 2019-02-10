//
// Created by crystalwind on 19-1-31.
//

#ifndef WINDZ_TCPSERVER_H
#define WINDZ_TCPSERVER_H

#include "Noncopyable.h"
#include "Atomic.h"
#include "TcpConnection.h"
#include "Socket.h"
#include "CallBack.h"
#include <string>
#include <memory>
#include <map>

namespace windz {

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : private Noncopyable {
  public:
    TcpServer(EventLoop *loop, const InetAddr &addr,
    const std::string &name, bool reuseport = false);
    ~TcpServer();

    void Start(size_t thread_num = 0);

    void SetConnectionCallBack(const ConnectionCallBack &cb) { connection_cb_ = cb; }
    void SetMessageCallBack(const MessageCallBack &cb) { message_cb_ = cb; }

    std::string name() { return name_; };
    std::string ip_port()  { return ip_port_; }

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

#endif //WINDZ_TCPSERVER_H
