#include "windz/base/Duration.h"
#include "windz/base/Timestamp.h"
#include "windz/net/EventLoop.h"

#include <iostream>

using namespace windz;
using namespace std;

int main(int argc, char **argv) {
    EventLoop loop;
    Timestamp now = Timestamp::Now();
    loop.RunAt(now + Duration(6.6), [] {
        cout << "RunAt: now + 6.6s.\n";
    });
    loop.RunAfter(Duration(10.0), [] {
        cout << "RunAfter: after 10s.\n";
    });
    loop.RunEvery(Duration(3.3), [] {
        cout << "RunEvery: every 3.3s\n";
    });
    loop.Loop();
}