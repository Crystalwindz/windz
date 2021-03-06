#include "windz/base/Thread.h"
#include "windz/net/EventLoop.h"

#include <assert.h>

using namespace windz;
using namespace std;

int main(int argc, char **argv) {
    EventLoop loop;
    // EventLoop loop2; // abort
    printf("main(): pid = %d, tid = %d\n", getpid(), windz::currentthread::tid());

    Thread thread1([] {
        printf("thread1(): pid = %d, tid = %d\n", getpid(), windz::currentthread::tid());
        EventLoop loop;
        loop.Loop();
    });

    Thread thread2([&loop] {
        sleep(3);
        printf("thread2(): pid = %d, tid = %d\n", getpid(), windz::currentthread::tid());
        loop.RunInLoop([] {
            printf("RunInLoop(): pid = %d, tid = %d\n", getpid(), windz::currentthread::tid());
        });
        loop.RunAfter(Duration(3.0), [&loop] {
            printf("loop Quit\n");
            loop.Quit();
        });
    });

    thread1.Start();
    thread2.Start();

    loop.Loop();

    return 0;
}
