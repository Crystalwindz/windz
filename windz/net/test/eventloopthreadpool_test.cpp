#include "windz/net/Channel.h"
#include "windz/net/EventLoop.h"
#include "windz/net/EventLoopThreadPool.h"

#include <iostream>

using namespace windz;
using namespace std;

int main(int argc, char **argv) {
    EventLoop loop;
    EventLoopThreadPool evpoll;
    loop.RunEvery(Duration(2.0), [&evpoll] {
        auto p = evpoll.Next();
        p->RunInLoop([] { printf("tid: %d\n", currentthread::tid()); });
    });
    evpoll.Start(6);
    loop.Loop();
}