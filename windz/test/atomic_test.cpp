//
// Created by crystalwind on 19-1-10.
//

#include "windz/Atomic.h"
#include "windz/Thread.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>

#define PRINTLN(s) cout << #s" = " << (s) << endl

using namespace windz;
using namespace std;

AtomicInt64 a64(4);
int64_t b = 0;

int main()
{
    PRINTLN(a64.Get());
    PRINTLN(a64.Set(1));
    PRINTLN(a64.Get());
    PRINTLN(a64.AddAndGet(5));
    PRINTLN(a64.GetAndAdd(-6));
    PRINTLN(a64.Get());

    vector<shared_ptr<Thread>> thread;
    for (int i = 0; i < 8; i++) {
        thread.push_back(make_shared<Thread>(
                []{
                    for (int i = 0; i < 1000000; i++) {
                        a64++;
                        b++;
                    }
                }));
    }

    for_each(thread.begin(), thread.end(), [](shared_ptr<Thread> &t){
       t->Start();
    });
    for_each(thread.begin(), thread.end(), [](shared_ptr<Thread> &t){
        t->Join();
    });

    PRINTLN(a64.Get());
    PRINTLN(b);
}