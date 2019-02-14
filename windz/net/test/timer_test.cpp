#include "windz/base/Duration.h"
#include "windz/base/Util.h"
#include "windz/net/EventLoop.h"

#include <iostream>

using namespace windz;
using namespace std;

int main(int argc, char **argv) {
    EventLoop loop;
    Timestamp now = Timestamp::Now();
    loop.RunAt(now + Duration(10 * Duration::kSecond), [] {
        cout << Timer::timer_num() << " timer | ";
        cout << "10s test RunAt\n";
    });
    TimerId id1 = loop.RunEvery(Duration(5.0), [] {
        cout << Timer::timer_num() << " timer | ";
        cout << "Ping!\n";
    });
    loop.RunAfter(Duration(13.0), [&loop, id1] {
        cout << Timer::timer_num() << " timer | ";
        cout << "13s cancel 5s every timer" << endl;
        loop.CancelTimer(id1);
    });
    loop.RunAfter(Duration(16.0), [] {
        cout << Timer::timer_num() << " timer | ";
        cout << "16s" << endl;
    });
    loop.RunAfter(Duration(7.0), [] {
        cout << Timer::timer_num() << " timer | ";
        cout << "7s" << endl;
    });
    TimerId id2 = loop.RunAfter(Duration(4.0), [] {
        cout << Timer::timer_num() << " timer | ";
        cout << "4s" << endl;
    });
    loop.RunAfter(Duration(3.0), [&loop, id2] {
        cout << Timer::timer_num() << " timer | ";
        cout << "3s cancel 4s timer" << endl;
        loop.CancelTimer(id2);
    });
    loop.RunAfter(Duration(5.0), [&loop, id2] {
        cout << Timer::timer_num() << " timer | ";
        cout << "5s cancel 4s timer" << endl;
        loop.CancelTimer(id2);
    });
    loop.RunAfter(Duration(1.0), [] {
        cout << Timer::timer_num() << " timer | ";
        cout << "1s" << endl;
    });
    loop.RunAfter(Duration(4.5), [] {
        cout << Timer::timer_num() << " timer | ";
        cout << "4.5s" << endl;
    });
    loop.RunAfter(Duration(2.5), [] {
        cout << Timer::timer_num() << " timer | ";
        cout << "2.5s" << endl;
    });
    loop.RunAfter(Duration(6.5), [] {
        cout << Timer::timer_num() << " timer | ";
        cout << "6.5s" << endl;
    });
    loop.Loop();
}