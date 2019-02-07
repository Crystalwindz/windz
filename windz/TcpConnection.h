//
// Created by crystalwind on 19-1-31.
//

#ifndef WINDZ_TCPCONNECTION_H
#define WINDZ_TCPCONNECTION_H

#include "Util.h"
#include "Socket.h"
#include "Buffer.h"
#include "CallBack.h"
#include "Duration.h"
#include <string>
#include <memory>
#include <functional>

namespace windz {

class Channel;
class EventLoop;

class TcpConnection : private noncopyable,
                      public std::enable_shared_from_this<TcpConnection> {
  public:
    TcpConnection(ObserverPtr<EventLoop> loop, const std::string &name,
                  const Socket &sockfd, const InetAddr &local, const InetAddr &peer);
    ~TcpConnection();

    void EstablishConnection();
    void DestroyConnection();

    void Send(const void *message, int len);
    void Send(const std::string &message);
    void Send(const Buffer &message);
    void Shutdown();
    void ForceClose();
    void ForceCloseAfter(const Duration &delay);
    void SetTcpNoDelay(bool flag);

    void SetConnectionCallBack(const ConnectionCallBack &cb) {
        connection_cb_ = cb;
    }
    void SetMessageCallBack(const MessageCallBack &cb) {
        message_cb_ = cb;
    }
    void SetLowWaterMarkCallBack(const LowWaterMarkCallBack &cb) {
        low_watermark_cb_ = cb;
    }
    void SetHighWaterMarkCallBack(const HighWaterMarkCallBack &cb) {
        high_watermark_cb_ = cb;
    }
    void SetCloseCallBack(const CloseCallBack &cb) {
        close_cb_ = cb;
    }

    bool connected() const { return state_ == kConnected; }
    ObserverPtr<EventLoop> loop() const { return loop_; }
    const std::string &name() const { return name_; }
    const InetAddr &local_addr() const { return local_addr_; }
    const InetAddr &peer_addr() const { return peer_addr_; }

  private:
    enum State { kConnecting, kConnected, kDisconnected, kDisconnecting };

    void HandleRead();
    void HandleWrite();
    void HandleClose();
    void HandleError();

    void SendInLoop(const void *message, size_t len);
    void ShutdownInLoop();
    void ForceCloseInLoop();

    ObserverPtr<EventLoop> loop_;
    std::string name_;
    State state_;
    Socket socket_;
    std::shared_ptr<Channel> channel_;
    InetAddr local_addr_;
    InetAddr peer_addr_;
    ConnectionCallBack connection_cb_;
    MessageCallBack message_cb_;
    LowWaterMarkCallBack low_watermark_cb_;
    HighWaterMarkCallBack high_watermark_cb_;
    CloseCallBack close_cb_;
    size_t high_watermark_;
    Buffer input_buf_;
    Buffer output_buf_;
};

}  // namespace windz

#endif //WINDZ_TCPCONNECTION_H
