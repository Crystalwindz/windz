//
// Created by crystalwind on 19-1-31.
//

#include "UNDOConnector.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include <assert.h>

namespace windz {

UNDOConnector::UNDOConnector(EventLoop *loop, const InetAddr &addr)
: loop_(loop), addr_(addr), connect_(false),
  status_(kDisconnected), retry_timems_(kInitRetryTimeMs) {
    /*empty*/
}

UNDOConnector::~UNDOConnector() {
    assert(!channel_);
}

void UNDOConnector::Start() {
    connect_ = true;
    loop_->RunInLoop([this]{
        loop_->AssertInLoopThread();
        assert(status_ == kDisconnected);
        if (connect_) {
            Connect();
        }
    });
}

void UNDOConnector::Stop() {
    connect_ = false;
    loop_->RunInLoop([this]{
        loop_->AssertInLoopThread();
        if (status_ == kConnecting) {
            status_ = kDisconnected;
            channel_->DisableReadWrite();
            channel_->Close();
            Socket sockfd(channel_->fd());
            channel_.reset();
            Retry(sockfd);
        }
    });
}

void UNDOConnector::Connect() {
    Socket sockfd = Socket::CreateNonblockSocket();
    bool r = sockfd.Connect(addr_);
    int save_errno = r ? 0 : errno;
    switch (save_errno) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:

    }
}

void UNDOConnector::Connecting(const Socket &sockfd) {
    status_ = kConnecting;
    assert(!channel_);
    channel_ = std::make_shared<Channel>(loop_, sockfd.sockfd());
    channel_->SetWriteHandler()
}

void UNDOConnector::Retry(const Socket &sockfd) {
    sockfd.Close();
    status_ = kDisconnected;
    if (connect_) {
        loop_
    }
}

void UNDOConnector::HandleWrite() {
    if (status_ == kConnecting) {
        
    }
}

void UNDOConnector::HandleError() {
    
}

}  // namespace windz
