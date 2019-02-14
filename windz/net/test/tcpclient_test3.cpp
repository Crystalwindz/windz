#include "windz/base/Thread.h"
#include "windz/net/EventLoop.h"
#include "windz/net/EventLoopThread.h"
#include "windz/net/TcpClient.h"

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
