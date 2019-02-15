#include "windz/base/Thread.h"
#include "windz/base/CurrentThread.h"
#include "windz/net/EventLoop.h"

#include <iostream>

using namespace windz;
using namespace std;

int main(int argc, char **argv) {
    EventLoop loop;
    Thread thread([&loop] {
        cout << "tid: " << currentthread::tid()
             << " invoke RunInLoop()\n";
        loop.RunInLoop([] {
            cout << "functor run in tid: " << currentthread::tid() << endl;
        });
    });
    thread.Start();
    loop.Loop();
}

