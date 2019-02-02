//
// Created by crystalwind on 19-1-30.
//

#include "Acceptor.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory>

namespace windz {

Acceptor::Acceptor(EventLoop *loop, const InetAddr &addr, bool reuseport)
        : loop_(loop), socket_(Socket::CreateNonblockSocket()),
          channel_(std::make_shared<Channel>(loop, socket_.sockfd())),
          listenning_(false),
          idle_fd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
    assert(idle_fd_ > 0);
    socket_.SetReuseAddr(true);
    socket_.SetReusePort(reuseport);
    socket_.Bind(addr);
    channel_->SetReadHandler([this]{
        HandleRead();
    });
}

Acceptor::~Acceptor() {
    channel_->DisableReadWrite();
    channel_->Close();
    socket_.Close();
    ::close(idle_fd_);
}

void Acceptor::Listen() {
    loop_->AssertInLoopThread();
    listenning_ = true;
    socket_.Listen();
    channel_->EnableRead();
}

void Acceptor::HandleRead() {
    loop_->AssertInLoopThread();
    InetAddr peer_addr;
    Socket conn = socket_.Accept(&peer_addr);
    if (conn.Valid()) {
        if (conncb_) {
            conncb_(conn, peer_addr);
        } else {
            conn.Close();
        }
    } else {
        // See the section named "The special problem of accept()ing
        // when you can't" in https://linux.die.net/man/3/ev.
        if (errno == EMFILE || errno == ENFILE) {
            ::close(idle_fd_);
            socket_.Accept().Close();
            idle_fd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}

}  // namespace windz
