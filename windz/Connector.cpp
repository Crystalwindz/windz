//
// Created by crystalwind on 19-1-31.
//

#include "Connector.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include "Duration.h"
#include <assert.h>

namespace windz {

const Duration Connector::kMaxRetryDuration(30.0);
const Duration Connector::kInitRetryDuration(0.1);

Connector::Connector(EventLoop *loop, const InetAddr &addr)
: loop_(loop), addr_(addr), start_(false),
  state_(kDisconnected), retry_duration_(kInitRetryDuration) {
    /*empty*/
}

Connector::~Connector() {
    assert(!channel_);
}

void Connector::Start() {
    start_.SetTrue();
    auto self = shared_from_this();
    loop_->RunInLoop([this, self] {
        loop_->AssertInLoopThread();
        assert(state_ == kDisconnected);
        if (start_) {
            Connect();
        }
    });
}

void Connector::Stop() {
    start_.SetFalse();
    auto self = shared_from_this();
    loop_->RunInLoop([this, self] {
        loop_->AssertInLoopThread();
        if (state_ == kConnecting) {
            state_ = kDisconnected;
            Socket socket = ResetChannel();
            socket.Close();
        }
    });
}

void Connector::Restart() {
    loop_->AssertInLoopThread();
    state_ = kDisconnected;
    retry_duration_ = kInitRetryDuration;
    start_.SetTrue();
    if (start_) {
        Connect();
    }
}

void Connector::Connect() {
    Socket sockfd = Socket::CreateNonblockSocket();
    bool r = sockfd.Connect(addr_);
    int save_errno = r ? 0 : errno;
    switch (save_errno) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            Connecting(sockfd);
            break;
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case EAGAIN:
        case ECONNREFUSED:
        case ENETUNREACH:
        case ETIMEDOUT:
            Retry(sockfd);
            break;
        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
        case EPROTOTYPE:
            // TODO:LOGSYSERR
            sockfd.Close();
            break;
        default:
            break;
    }
}

void Connector::Connecting(const Socket &sockfd) {
    state_ = kConnecting;
    assert(!channel_);
    channel_ = std::make_shared<Channel>(loop_.Get(), sockfd.sockfd());
    channel_->SetWriteHandler([this]{ HandleWrite(); });
    channel_->SetErrorHandler([this]{ HandleError(); });
    channel_->Tie(shared_from_this());
    channel_->EnableWrite();
}

void Connector::Retry(const Socket &sockfd) {
    sockfd.Close();
    state_ = kDisconnected;
    if (start_) {
        auto self(shared_from_this());
        loop_->RunAfter(retry_duration_, [this, self]{
            loop_->AssertInLoopThread();
            assert(state_ == kDisconnected);
            if (start_) {
                Connect();
            }
        });
        retry_duration_*=2;
        if (retry_duration_ > kMaxRetryDuration) {
            retry_duration_ = kMaxRetryDuration;
        }
    } else {
        // TODO:LOG
    }
}

void Connector::HandleWrite() {
    Socket socket = ResetChannel();
    int err = socket.SocketError();
    if (err) {
        Retry(socket);
    } else if (socket.LocalAddr().IpPortString()
               == socket.PeerAddr().IpPortString()) {
        Retry(socket);
    } else {
        state_ = kConnected;
        if (start_) {
            conncb_(socket);
        } else {
            socket.Close();
        }
    }
}

void Connector::HandleError() {
    // TODO:LOGERROR
    if (state_ == kConnecting) {
        Socket socket = ResetChannel();
        int err = socket.SocketError();
        static_cast<void>(err);
        // TODO:LOGTRACE
        Retry(socket);
    }
}

Socket Connector::ResetChannel() {
    channel_->DisableReadWrite();
    channel_->Close();
    Socket socket(channel_->fd());
    loop_->QueueInLoop([this] {
        // FIXME: Retry() may cause Connecting(): assert(!channel_) fail.
        channel_.reset();
    });
    return socket;
}

}  // namespace windz
