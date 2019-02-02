//
// Created by crystalwind on 19-1-31.
//

#ifndef WINDZ_TCPSERVER_H
#define WINDZ_TCPSERVER_H

#include "Util.h"
#include "Atomic.h"
#include "TcpConnection.h"
#include "CallBack.h"
#include <string>
#include <memory>
#include <map>

namespace windz {

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : private noncopyable {
  public:
    TcpServer(EventLoop *loop, const InetAddr &addr,
    const std::string &name, bool reuseport = false);
    ~TcpServer();

    void Start();

    void SetConnectionCallBack(const ConnectionCallBack &cb) { connection_cb_ = cb; }
    void SetMessageCallBack(const MessageCallBack &cb) { message_cb_ = cb; }
    void SetWriteCompleteCallBack(const WriteCompleteCallBack &cb) { write_complete_cb_ = cb; }

  private:
    using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

    ObserverPtr<EventLoop> loop_;
    std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    std::unique_ptr<EventLoopThreadPool> thread_pool_;
    ConnectionCallBack connection_cb_;
    MessageCallBack message_cb_;
    WriteCompleteCallBack write_complete_cb_;
    AtomicInt32 started_;
    int next_connid_;
    ConnectionMap connections_;
};

}  // namespace windz

#endif //WINDZ_TCPSERVER_H
