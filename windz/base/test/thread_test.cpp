#include "windz/base/Thread.h"

#include <unistd.h>

#include <functional>
#include <iostream>

using namespace std;
using namespace windz;

int main(int argc, char **argv) {
    int a = 4;
    Thread t1([&a]() {
        cout << currentthread::tid() << " " << currentthread::name() << " "
             << currentthread::tid_str() << " " << currentthread::tid_str_len() << " " << boolalpha
             << currentthread::IsMainThread() << endl;
        for (int i = 0; i < 5; i++) {
            cout << "hello!\n";
            ++a;
            sleep(1);
        }
    });
    cout << currentthread::tid() << " " << boolalpha << currentthread::IsMainThread() << endl;
    t1.Start();
    t1.Join();
    cout << a << endl;
    return 0;
}