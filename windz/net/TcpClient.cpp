#include "windz/net/TcpClient.h"
#include "windz/net/Connector.h"
#include "windz/net/EventLoop.h"

#include <memory>
#include <string>

namespace windz {

TcpClient::TcpClient(ObserverPtr<EventLoop> loop, const InetAddr &addr, const std::string &name)
    : loop_(loop),
      name_(name),
      connector_(std::make_shared<Connector>(loop, addr)),
      retry_(false),
      start_(false),
      next_conn_id_(1) {
    connector_->SetConnectionCallBack([this](const Socket &socket) { NewTcpConnection(socket); });
}

TcpClient::~TcpClient() {
    TcpConnectionPtr conn;
    bool unique = false;
    {
        LockGuard lock(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }
    if (conn) {
        loop_->RunInLoop([this, conn] {
            conn->SetCloseCallBack([this](const TcpConnectionPtr &conn) {
                loop_->QueueInLoop([conn] { conn->DestroyConnection(); });
            });
        });
        if (unique) {
            conn->ForceClose();
        }
    } else {
        connector_->Stop();
    }
}

void TcpClient::Connect() {
    assert(!start_);
    start_.SetTrue();
    connector_->Start();
}

void TcpClient::Disconnect() {
    start_.SetFalse();
    {
        LockGuard lock(mutex_);
        if (connection_) {
            connection_->Shutdown();
        }
    }
}

void TcpClient::Stop() {
    start_.SetFalse();
    connector_->Stop();
}

void TcpClient::NewTcpConnection(const Socket &socket) {
    loop_->AssertInLoopThread();
    InetAddr peer_addr = socket.PeerAddr();
    std::string conn_name =
        name_ + ":" + peer_addr.IpPortString() + "#" + std::to_string(next_conn_id_);
    InetAddr local_addr = socket.LocalAddr();
    TcpConnectionPtr conn =
        std::make_shared<TcpConnection>(loop_, conn_name, socket, local_addr, peer_addr);
    conn->SetConnectionCallBack(connection_cb_);
    conn->SetMessageCallBack(message_cb_);
    conn->SetCloseCallBack([this](const TcpConnectionPtr &conn) { RemoveTcpConnection(conn); });
    {
        LockGuard lock(mutex_);
        connection_ = conn;
    }
    conn->EstablishConnection();
}

void TcpClient::RemoveTcpConnection(const TcpConnectionPtr &conn) {
    loop_->AssertInLoopThread();
    {
        LockGuard lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }
    loop_->QueueInLoop([conn] { conn->DestroyConnection(); });
    if (retry_ && start_) {
        connector_->Restart();
    }
}

}  // namespace windz