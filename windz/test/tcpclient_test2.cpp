//
// Created by crystalwind on 19-2-10.
//

#include "windz/TcpClient.h"
#include "windz/EventLoop.h"
#include "windz/Duration.h"
#include <iostream>

using namespace windz;

int main(int argc, char **argv) {
    EventLoop loop;
    InetAddr unreach_addr("127.0.0.1", 32);
    TcpClient client(&loop, unreach_addr, "client");
    loop.RunAfter(Duration(2.0),[&client] {
        std::cout << "Stop\n";
        client.Stop();
    });
    loop.RunAfter(Duration(4.0), [&loop]{
        loop.Quit();
    });
    client.Connect();
    loop.Loop();
}