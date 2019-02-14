#include "windz/base/ThreadPool.h"

#include <iostream>

using namespace windz;
using namespace std;

int main(int argc, char **argv) {
    auto task = [] {
        printf("tid: %d get a task\n", currentthread::tid());
        sleep(3);
    };

    ThreadPool threadpool;
    threadpool.SetQueueMaxSize(1);
    threadpool.Start(2);
    for (int i = 0; i < 15; i++) {
        threadpool.AddTask(task);
        printf("Add task %d\n", i);
    }
    //    sleep(2);
    //    threadpool.Stop();
    //    threadpool.Start(3);
    sleep(60);
}