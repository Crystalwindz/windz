//
// Created by crystalwind on 19-1-15.
//

#include "windz/CountDownLatch.h"
#include "windz/Thread.h"
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

using namespace windz;
using namespace std;

CountDownLatch latch(5);

int main()
{
    vector<shared_ptr<Thread>> thread;

    for (int i = 0; i < 5; ++i) {
        thread.push_back(make_shared<Thread>(
                [i]{
                    sleep(i);
                    cout<<4-i<<endl;
                    latch.CountDown();
                }));
    }

    for_each(thread.begin(), thread.end(), [](shared_ptr<Thread> &t){
        t->Start();
    });
    latch.Wait();
    cout << "boom!";
}
