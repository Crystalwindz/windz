#ifndef WINDZ_TCPCLIENT_H
#define WINDZ_TCPCLIENT_H

#include "windz/base/Atomic.h"
#include "windz/base/Mutex.h"
#include "windz/base/Noncopyable.h"
#include "windz/net/Connector.h"
#include "windz/net/TcpConnection.h"

namespace windz {

class TcpClient : Noncopyable {
  public:
    TcpClient(ObserverPtr<EventLoop> loop, const InetAddr &addr, const std::string &name);
    ~TcpClient();

    void Connect();
    void Disconnect();
    void Stop();

    void EnableRetry() { retry_.SetTrue(); }
    void SetConnectionCallBack(const ConnectionCallBack &cb) { connection_cb_ = cb; }
    void SetMessageCallBack(const MessageCallBack &cb) { message_cb_ = cb; }

    ObserverPtr<EventLoop> loop() const { return loop_; }
    const std::string &name() const { return name_; }
    bool retry() const { return retry_.Get(); }

  private:
    void NewTcpConnection(const Socket &socket);
    void RemoveTcpConnection(const TcpConnectionPtr &conn);

    ObserverPtr<EventLoop> loop_;
    std::string name_;
    std::shared_ptr<Connector> connector_;
    ConnectionCallBack connection_cb_;
    MessageCallBack message_cb_;
    AtomicBool retry_;
    AtomicBool start_;

    int next_conn_id_;
    mutable Mutex mutex_;
    TcpConnectionPtr connection_;
};

}  // namespace windz

#endif  // WINDZ_TCPCLIENT_H
