//
// Created by crystalwind on 19-1-21.
//

#include "Socket.h"
#include "Noncopyable.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <assert.h>
#include <string>

namespace windz {

InetAddr::InetAddr(in_port_t port, bool loopback) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = loopback ? INADDR_LOOPBACK : INADDR_ANY;
    addr_.sin_port = htons(port);
}

InetAddr::InetAddr(const std::string &ip, in_port_t port) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    ::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
}

std::string InetAddr::IpString() const {
    char buf[16];
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    return std::string(buf);
}

uint16_t InetAddr::PortUint16() const {
    return ntohs(addr_.sin_port);
}

std::string InetAddr::IpPortString() const {
    std::string ip = IpString();
    std::string port = std::to_string(PortUint16());
    return ip + ':' + port;
}

const struct sockaddr *InetAddr::SockAddr() const {
    return (struct sockaddr *)&addr_;
}

Socket Socket::CreateNonblockSocket() {
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    return Socket(sockfd);
}

bool Socket::Bind(const InetAddr &local_addr) const {
    int r = ::bind(sockfd_, local_addr.SockAddr(), sizeof(struct sockaddr_in));
    return r == 0;
}

bool Socket::Listen() const {
    int r = ::listen(sockfd_, 128);
    return r == 0;
}

Socket Socket::Accept(InetAddr *peer_addr) const {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    int connfd = ::accept4(sockfd_, (struct sockaddr *)&addr,
                           &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (peer_addr != nullptr && connfd >= 0) {
        peer_addr->SetAddr(addr);
    }
    //TODO: Error Check
    return Socket(connfd);
}

bool Socket::Connect(const InetAddr &server_addr) const {
    int r = ::connect(sockfd_, server_addr.SockAddr(), sizeof(struct sockaddr_in));
    return r == 0;
}

bool Socket::Shutdown(int how) const {
    assert(how == SHUT_RD || how == SHUT_WR || how == SHUT_RDWR);
    int r = ::shutdown(sockfd_, how);
    return r == 0;
}

bool Socket::Close() const {
    int r = ::close(sockfd_);
    return r == 0;
}

InetAddr Socket::LocalAddr() const {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    ::getsockname(sockfd_, (struct sockaddr *)&addr, &len);
    return InetAddr(addr);
}

InetAddr Socket::PeerAddr() const {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    ::getpeername(sockfd_, (struct sockaddr *)&addr, &len);
    return InetAddr(addr);
}

int Socket::SocketError() const {
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof(optval));

    if (::getsockopt(sockfd_, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        return errno;
    } else {
        return optval;
    }
}

bool Socket::SetTcpNoDelay(bool flag) const {
    int optval = flag ? 1 : 0;
    int r = ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                         &optval, sizeof(optval));
    return r == 0;
}

bool Socket::SetReuseAddr(bool flag) const {
    int optval = flag ? 1 : 0;
    int r = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                         &optval, sizeof(optval));
    return r == 0;
}

bool Socket::SetReusePort(bool flag) const {
    int optval = flag ? 1 : 0;
    int r = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                         &optval, sizeof(optval));
    return r == 0;
}

bool Socket::SetKeepAlive(bool flag) const {
    int optval = flag ? 1 : 0;
    int r = ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
                         &optval, sizeof(optval));
    return r == 0;
}

bool Socket::SetLinger(bool flag, int linger) const {
    assert(!flag || linger >= 0);
    struct linger optval;
    optval.l_onoff = flag ? 1 : 0;
    optval.l_linger = linger;
    int r = ::setsockopt(sockfd_, SOL_SOCKET, SO_LINGER,
                         &optval, sizeof(optval));
    return r == 0;
}

}  // namespace windz