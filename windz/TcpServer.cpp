//
// Created by crystalwind on 19-1-31.
//

#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"
#include "Socket.h"
#include "TcpConnection.h"
#include <string>
#include <memory>


namespace windz{

TcpServer::TcpServer(EventLoop *loop, const InetAddr &addr,
                     const std::string &name, bool reuseport)
        : loop_(loop), name_(name),
          ip_port_(addr.IpPortString()),
          acceptor_(new Acceptor(loop, addr, reuseport)),
          next_connid_(1)
{
    acceptor_->SetConnecionCallBack([this](const Socket &socket, const InetAddr &addr) {
        NewTcpConnection(socket, addr);
    });
}

TcpServer::~TcpServer() {
    loop_->AssertInLoopThread();

    for (auto &i: connections_) {
        TcpConnectionPtr conn = i.second;
        i.second.reset();
        conn->loop()->RunInLoop([conn]{ conn->DestroyConnection(); });
    }
}

void TcpServer::Start(size_t thread_num) {
    if (started_.GetAndAdd(1) == 0) {
        if (thread_num > 0) {
            thread_pool_= MakeUnique<EventLoopThreadPool>();
            thread_pool_->Start(thread_num);
        }
        assert(!acceptor_->listenning());
        loop_->RunInLoop([this]{ acceptor_->Listen(); });
    }
}

void TcpServer::NewTcpConnection(const Socket &socket, const InetAddr &peer_addr) {
    loop_->AssertInLoopThread();
    ObserverPtr<EventLoop> ioloop(nullptr);
    if (thread_pool_) {
        ioloop = thread_pool_->Next();
    } else {
        ioloop = loop_;
    }
    std::string conn_name = name_ + '#' + std::to_string(next_connid_);
    next_connid_+=1;
    InetAddr local_addr = socket.LocalAddr();
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(ioloop, conn_name,
                                                            socket, local_addr, peer_addr);
    connections_[conn_name] = conn;
    conn->SetConnectionCallBack(connection_cb_);
    conn->SetMessageCallBack(message_cb_);
    conn->SetCloseCallBack([this](const TcpConnectionPtr &conn) {
        RemoveTcpConnection(conn);
    });
    ioloop->RunInLoop([conn]{
        conn->EstablishConnection();
    });
}

void TcpServer::RemoveTcpConnection(const TcpConnectionPtr &conn) {
    loop_->RunInLoop([this, conn]{
        loop_->AssertInLoopThread();
        connections_.erase(conn->name());
        ObserverPtr<EventLoop> ioloop = conn->loop();
        ioloop->QueueInLoop([conn]{
            conn->DestroyConnection();
        });
    });
}

}  // namespace windz
