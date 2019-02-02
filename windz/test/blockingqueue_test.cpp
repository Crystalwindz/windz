//
// Created by crystalwind on 19-1-15.
//

#include "windz/BlockingQueue.h"
#include "windz/Thread.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <sstream>

using namespace windz;
using namespace std;

BlockingQueue<string> blkqueue;
BlockingQueue<string> limitqueue(3);

void TestBlockQueue() {
    cout<<"test blkqueue:\n";

    vector<shared_ptr<Thread>> producer1;
    vector<shared_ptr<Thread>> consumer1;
    for (int i = 0; i < 4; i++) {
        producer1.push_back(make_shared<Thread>(
                [i]{
                    for (int j = 0; j < 100; j++) {
                        stringstream str;
                        str << "Producer "<< i+1 << " " << j+1 << "th thing";
                        blkqueue.Push(str.str());
                    }
                }
        ));
        consumer1.push_back(make_shared<Thread>(
                [i]{
                    while (true) {
                        stringstream str;
                        str << "Consumer " << i+1 << ": " << blkqueue.Pop() << endl;
                        cout<<str.str();
                    }
                }
        ));
    }

    // start produce
    for_each(producer1.begin(), producer1.end(), [](shared_ptr<Thread> &t){
        t->Start();
    });
    for_each(producer1.begin(), producer1.end(), [](shared_ptr<Thread> &t){
        t->Join();
    });

    cout << "Now blkqueue size is: "<<blkqueue.Size()<<endl;
    for (size_t i = 0; i <blkqueue.Size(); ++i) {
        string s = blkqueue.Pop();
        cout << s <<endl;
        blkqueue.Push(s);
    }

    for_each(consumer1.begin(), consumer1.end(), [](shared_ptr<Thread> &t){
        t->Start();
    });

    sleep(5);
}

void TestLimitQueue() {
    cout << "\n\ntest limitqueue:\n";
    vector<shared_ptr<Thread>> producer2;
    vector<shared_ptr<Thread>> consumer2;
    for (int i = 0; i < 4; i++) {
        producer2.push_back(make_shared<Thread>(
                [i]{
                    for (int j = 0; j < 10; j++) {
                        stringstream str;
                        str << "Producer "<< i+1 << " " << j+1 << "th thing\n";
                        if (limitqueue.wakeup()) {
                            sleep(3);
                        }
                        limitqueue.Push(str.str());
                        str << "limitqueue size is " <<limitqueue.Size()<<endl;
                        cout << str.str();
                    }
                }
        ));
        consumer2.push_back(make_shared<Thread>(
                [i]{
                    while (true) {
                        stringstream str;
                        sleep(5);
                        str << "Consumer " << i+1 << ": " << limitqueue.Pop();
                        cout<<str.str();
                    }
                }
        ));
    }

    for_each(producer2.begin(), producer2.end(), [](shared_ptr<Thread> &t){
        t->Start();
    });
//    for_each(consumer2.begin(), consumer2.end(), [](shared_ptr<Thread> &t){
//        t->Start();
//    });
    Thread exiter([]{
        sleep(7);
        limitqueue.WakeUp();
        cout << "\nWakeUp\n\n";
        sleep(1);
        limitqueue.Restore();
    });
    exiter.Start();

    sleep(60);
}

int main()
{
    //TestBlockQueue();
    TestLimitQueue();
}