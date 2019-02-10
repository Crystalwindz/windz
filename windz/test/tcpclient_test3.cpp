//
// Created by crystalwind on 19-2-10.
//

#include "windz/EventLoop.h"
#include "windz/EventLoopThread.h"
#include "windz/TcpClient.h"
#include "windz/Thread.h"

using namespace windz;

int main(int argc, char **argv) {
    EventLoopThread loop;
    ObserverPtr<EventLoop> p_loop = loop.Start();
    InetAddr addr("127.0.0.1", 2019);
    {
        TcpClient client(p_loop.Get(), addr, "client");
        client.Connect();
        sleep(5);
        client.Disconnect();
    }
    sleep(4);
}
