//
// Created by crystalwind on 19-1-16.
//

#include "windz/EventLoopThreadPool.h"
#include "windz/EventLoop.h"
#include "windz/Channel.h"
#include <iostream>

using namespace windz;
using namespace std;

int main()
{
    EventLoop loop;
    EventLoopThreadPool evpoll;
    loop.RunEvery(Duration(2.0), [&evpoll] {
        auto p = evpoll.Next();
        p->RunInLoop([]{
            printf("tid: %d\n", currentthread::tid());
        });
    });
    evpoll.Start(6);
    loop.Loop();
}