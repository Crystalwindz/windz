//
// Created by crystalwind on 19-2-9.
//

#include "windz/TcpClient.h"
#include "windz/EventLoop.h"
#include "windz/Thread.h"
#include "windz/Channel.h"
#include "windz/Util.h"

#include <vector>
#include <memory>
#include <iostream>

using namespace windz;

int main(int argc, char **argv) {
    EventLoop loop;
    InetAddr addr("127.0.0.1", 2019);
    TcpClient client(&loop, addr, "client");
    auto channel = std::make_shared<Channel>(&loop, STDIN_FILENO);

    client.SetConnectionCallBack([&loop, channel](const TcpConnectionPtr &conn) {
        std::cout << conn->local_addr().IpPortString()
                  << " -> "<< conn->peer_addr().IpPortString()
                  <<  (conn->connected() ? " Connect.\n" : " Disconnect.\n");
        if (conn->connected()) {
            std::weak_ptr<TcpConnection> weak_conn(conn);
            channel->SetReadHandler([weak_conn, channel] {
                auto conn = weak_conn.lock();
                if (conn) {
                    std::string msg;
                    util::SetNonBlockAndCloseOnExec(STDIN_FILENO);
                    net::ReadFd(STDIN_FILENO, msg);
                    conn->Send(msg);
                }
            });
            channel->SetErrorHandler([conn] {
                conn->Send("STDIN EOF ERROR\n");
                sleep(60);
            });
            channel->EnableRead();
        } else {
            channel->DisableRead();
        }
    });
    client.SetMessageCallBack([](const TcpConnectionPtr& conn, Buffer &buffer) {
        std::string msg(buffer.ReadAll());
        std::cout << msg << std::flush;
    });
    client.EnableRetry();
    client.Connect();
    loop.Loop();
}
