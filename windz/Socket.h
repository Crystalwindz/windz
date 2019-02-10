//
// Created by crystalwind on 19-1-21.
//

#ifndef WINDZ_SOCKET_H
#define WINDZ_SOCKET_H

#include "Noncopyable.h"
#include <netinet/in.h>
#include <unistd.h>
#include <string>

namespace windz {

class InetAddr {
  public:
    explicit InetAddr(in_port_t port = 0, bool loopback = false);
    InetAddr(const std::string &ip, in_port_t port);
    explicit InetAddr(const struct sockaddr_in &addr) : addr_(addr) {}

    std::string IpString() const;
    uint16_t PortUint16() const;
    std::string IpPortString() const;
    const struct sockaddr *SockAddr() const;
    void SetAddr(const struct sockaddr_in &addr) { addr_ = addr; }

  private:
    struct sockaddr_in addr_;
};

class Socket {
  public:
    static Socket CreateNonblockSocket();

    explicit Socket(int sockfd) : sockfd_(sockfd) {}

    bool Bind(const InetAddr &local_addr) const;
    bool Listen() const;
    Socket Accept(InetAddr *peer_addr = nullptr) const;
    bool Connect(const InetAddr &server_addr) const;
    bool Shutdown(int how) const;
    bool Close() const;

    InetAddr LocalAddr() const;
    InetAddr PeerAddr() const;
    int SocketError() const;

    bool SetTcpNoDelay(bool flag) const;
    bool SetReuseAddr(bool flag) const;
    bool SetReusePort(bool flag) const;
    bool SetKeepAlive(bool flag) const;
    bool SetLinger(bool flag, int linger = -1) const;

    bool Valid() const { return sockfd_ >= 0; }
    int sockfd() const { return sockfd_; }

  private:
    int sockfd_;
};

}  // namespace windz

#endif //WINDZ_SOCKET_H
