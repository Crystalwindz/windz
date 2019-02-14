#include "windz/net/Acceptor.h"
#include "windz/net/Channel.h"
#include "windz/net/EventLoop.h"
#include "windz/net/Socket.h"
#include "windz/log/Logger.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <memory>

namespace windz {

Acceptor::Acceptor(ObserverPtr<EventLoop> loop, const InetAddr &addr, bool reuseport)
    : loop_(loop),
      socket_(Socket::CreateNonblockSocket()),
      channel_(std::make_shared<Channel>(loop, socket_.sockfd())),
      listenning_(false),
      idle_fd_(open("/dev/null", O_RDONLY | O_CLOEXEC)) {
    assert(idle_fd_ > 0);
    socket_.SetReuseAddr(true);
    socket_.SetReusePort(reuseport);
    socket_.Bind(addr);
    channel_->SetReadHandler([this] { HandleRead(); });
}

Acceptor::~Acceptor() {
    channel_->DisableReadWrite();
    channel_->Close();
    socket_.Close();
    close(idle_fd_);
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
    Socket socket = socket_.Accept(&peer_addr);
    if (socket.Valid()) {
        if (conn_cb_) {
            conn_cb_(socket, peer_addr);
        } else {
            socket.Close();
        }
    } else {
        LOG_SYSERR << "Acceptor::HandleRead() - accept error";
        // See the section named "The special problem of accept()ing
        // when you can't" in https://linux.die.net/man/3/ev.
        if (errno == EMFILE || errno == ENFILE) {
            close(idle_fd_);
            socket_.Accept().Close();
            idle_fd_ = open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}

}  // namespace windz
