#include "windz/base/CurrentThread.h"
#include "windz/net/EventLoop.h"
#include "windz/net/TcpServer.h"

#include <string>
#include <iostream>

int main(int argc, char **argv) {
    windz::EventLoop loop;
    windz::InetAddr addr(2019);
    windz::TcpServer echo(&loop, addr, "echo");

    echo.SetConnectionCallBack([](const windz::TcpConnectionPtr &conn) {
        std::cout<<"echo - " << windz::currentthread::tid()
                 << " " << conn->peer_addr().IpPortString()
                 << " -> "<< conn->local_addr().IpPortString()
                 <<  (conn->connected() ? " Connect.\n" : " Disconnect.\n");
    });

    echo.SetMessageCallBack([](const windz::TcpConnectionPtr &conn, windz::Buffer &buffer) {
        std::string msg = buffer.ReadAll();
        printf("%s echo %lu bytes: %s", conn->name().c_str(), msg.length(), msg.c_str());
        conn->Send(msg);
    });

    echo.Start(8);
    loop.Loop();
}
