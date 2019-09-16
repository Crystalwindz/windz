#include "windz/net/TcpConnection.h"
#include "windz/net/Buffer.h"
#include "windz/net/Channel.h"
#include "windz/net/EventLoop.h"
#include "windz/net/Socket.h"
#include "windz/net/Timer.h"
#include "windz/log/Logger.h"

#include <memory>
#include <string>

namespace windz {

TcpConnection::TcpConnection(ObserverPtr<EventLoop> loop, const std::string &name,
                             const Socket &sockfd, const InetAddr &local, const InetAddr &peer)
    : loop_(loop),
      name_(name),
      state_(kConnecting),
      socket_(sockfd),
      channel_(std::make_shared<Channel>(loop, sockfd.sockfd())),
      local_addr_(local),
      peer_addr_(peer),
      high_watermark_(64 * 1024 * 1024) {
    channel_->SetReadHandler([this] { HandleRead(); });
    channel_->SetWriteHandler([this] { HandleWrite(); });
    channel_->SetErrorHandler([this] { HandleError(); });
    socket_.SetKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    channel_->DisableReadWrite();
    channel_->Close();
    socket_.Close();
}

void TcpConnection::EstablishConnection() {
    loop_->AssertInLoopThread();
    assert(state_ == kConnecting);
    state_ = kConnected;
    channel_->Tie(shared_from_this());
    channel_->EnableRead();
    if (connection_cb_) {
        connection_cb_(shared_from_this());
    }
}

void TcpConnection::DestroyConnection() {
    loop_->AssertInLoopThread();
    state_ = kDisconnected;
    channel_->DisableReadWrite();
    if (connection_cb_) {
        connection_cb_(shared_from_this());
    }
    channel_->Close();
}

void TcpConnection::Send(const void *message, int len) {
    Send(std::string(static_cast<const char *>(message), len));
}

void TcpConnection::Send(const std::string &message) {
    if (state_ == kConnected) {
        if (loop_->IsInLoopThread()) {
            SendInLoop(message.c_str(), message.length());
        } else {
            auto self = shared_from_this();
            loop_->RunInLoop(
                [this, message, self] { SendInLoop(message.c_str(), message.length()); });
        }
    }
}

void TcpConnection::Send(Buffer &message) {
    Send(message.Peek(), message.ReadableBytes());
    message.ReleaseAll();
}

void TcpConnection::SendInLoop(const void *message, size_t len) {
    loop_->AssertInLoopThread();
    ssize_t nwrite = 0;
    size_t remain = len;
    bool err = false;
    if (state_ == kDisconnected) {
        LOG_WARN << "TcpConnection::SendInLoop() - disconnected, give up writing";
        return;
    }

    if (!channel_->WriteEnabled() && output_buf_.ReadableBytes() == 0) {
        nwrite = write(channel_->fd(), message, len);
        if (nwrite >= 0) {
            remain = len - nwrite;
            if (remain == 0 && low_watermark_cb_) {
                auto self = shared_from_this();
                loop_->QueueInLoop([this, self] { low_watermark_cb_(shared_from_this()); });
            }
        } else {
            nwrite = 0;
            if (errno != EWOULDBLOCK) {
                LOG_SYSERR << "TcpConnection::SendInLoop() - write error";
                if (errno == EPIPE || errno == ECONNRESET) {
                    err = true;
                }
            }
        }
    }

    assert(remain <= len);
    if (!err && remain > 0) {
        size_t oldlen = output_buf_.ReadableBytes();
        if (oldlen <= high_watermark_ && oldlen + remain > high_watermark_ && high_watermark_cb_) {
            auto self = shared_from_this();
            loop_->RunInLoop([this, oldlen, remain, self] {
                high_watermark_cb_(shared_from_this(), oldlen + remain);
            });
        }
        output_buf_.Write(static_cast<const char *>(message) + nwrite, remain);
        if (!channel_->WriteEnabled()) {
            channel_->EnableWrite();
        }
    }
}

void TcpConnection::Shutdown() {
    if (state_ == kConnected) {
        state_ = kDisconnecting;
        auto self = shared_from_this();
        loop_->RunInLoop([this, self] { ShutdownInLoop(); });
    }
}

void TcpConnection::ShutdownInLoop() {
    loop_->AssertInLoopThread();
    if (!channel_->WriteEnabled()) {
        socket_.Shutdown(SHUT_WR);
    }
}

void TcpConnection::ForceClose() {
    if (state_ == kConnected || state_ == kDisconnecting) {
        state_ = kDisconnected;
        auto self = shared_from_this();
        loop_->QueueInLoop([this, self] { ForceCloseInLoop(); });
    }
}

void TcpConnection::ForceCloseAfter(const Duration &delay) {
    auto self = shared_from_this();
    loop_->RunAfter(delay, [this, self] { ForceCloseInLoop(); });
}

void TcpConnection::ForceCloseInLoop() {
    loop_->AssertInLoopThread();
    if (state_ == kConnected || state_ == kDisconnecting) {
        HandleClose();
    }
}

void TcpConnection::SetTcpNoDelay(bool flag) { socket_.SetTcpNoDelay(flag); }

void TcpConnection::HandleRead() {
    loop_->AssertInLoopThread();
    ssize_t n = input_buf_.ReadSocket(socket_);
    if (n > 0) {
        message_cb_(shared_from_this(), input_buf_);
    } else if (n == 0) {
        HandleClose();
    } else {
        LOG_SYSERR << "TcpConnection::HandleRead() - read error";
        HandleError();
    }
}

void TcpConnection::HandleWrite() {
    loop_->AssertInLoopThread();
    if (channel_->WriteEnabled()) {
        ssize_t n = write(channel_->fd(), output_buf_.Peek(), output_buf_.ReadableBytes());
        if (n > 0) {
            output_buf_.Release(n);
            if (output_buf_.ReadableBytes() == 0) {
                channel_->DisableWrite();
                if (low_watermark_cb_) {
                    auto self = shared_from_this();
                    loop_->QueueInLoop([this, self] { low_watermark_cb_(shared_from_this()); });
                }
                if (state_ == kDisconnecting) {
                    ShutdownInLoop();
                }
                if (state_ == kDisconnected) {
                    HandleClose();
                }
            }
        } else {
            LOG_SYSERR << "TcpConnection::HandleWrite() - write error";
        }
    }
}

void TcpConnection::HandleClose() {
    loop_->AssertInLoopThread();
    assert(state_ == kConnected || state_ == kDisconnecting || state_ == kDisconnected);
    state_ = kDisconnected;
    channel_->DisableRead();
    if (channel_->WriteEnabled()) {
        return;
    }
    channel_->DisableWrite();
    close_cb_(shared_from_this());
}

void TcpConnection::HandleError() {
    LOG_ERROR << "TcpConnection::HandleError() [" << name_
              << "] - SO_ERROR = " << socket_.SocketError();
}

}  // namespace windz
