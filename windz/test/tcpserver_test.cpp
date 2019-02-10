//
// Created by crystalwind on 19-2-9.
//

#include "windz/EventLoop.h"
#include "windz/TcpServer.h"
#include "windz/CurrentThread.h"
#include <string>
#include <iostream>

using namespace windz;

int main(int argc, char **argv) {
    EventLoop loop;
    InetAddr addr(2019);
    TcpServer echo(&loop, addr, "echo");

    echo.SetConnectionCallBack([](const TcpConnectionPtr &conn) {
        std::cout<<"echo - " << currentthread::tid()
                 << " " << conn->peer_addr().IpPortString()
                 << " -> "<< conn->local_addr().IpPortString()
                 <<  (conn->connected() ? " Connect.\n" : " Disconnect.\n");
    });

    echo.SetMessageCallBack([&loop](const TcpConnectionPtr &conn, Buffer &buffer) {
        std::string msg(buffer.ReadAll());
        printf("%s recv %lu bytes: %s", conn->name().c_str(), msg.length(), msg.c_str());
        if (msg == "exit\n") {
            conn->Send("bye\n");
            conn->Shutdown();
        }
        else if (msg == "quit") {
            conn->Send("Server exiting...");
            loop.Quit();
        } else {
            conn->Send(msg);
        }
    });

    echo.Start();
    loop.Loop();
}

