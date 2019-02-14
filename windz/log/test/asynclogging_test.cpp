#include "windz/base/Thread.h"
#include "windz/log/AsyncLogging.h"
#include "windz/log/Logger.h"

#include <algorithm>
#include <memory>
#include <vector>

using namespace windz;
using namespace std;

int main(int argc, char **argv) {
    Logger::SetLevel(Logger::TRACE);
    AsyncLogging async_logging("test", 10 * 1024 * 1024);
    async_logging.Start();
    Logger::SetOutput(
        [&async_logging](const char *msg, size_t len) { async_logging.Append(msg, len); });

    vector<shared_ptr<Thread>> thread;
    for (int i = 0; i < 8; i++) {
        thread.push_back(make_shared<Thread>([] {
            for (int i = 0; i < 100; i++) {
                LOG_INFO << i;
            }
        }));
    }
    for_each(thread.begin(), thread.end(), [](shared_ptr<Thread> &t) { t->Start(); });
    for_each(thread.begin(), thread.end(), [](shared_ptr<Thread> &t) { t->Join(); });
    sleep(1);
    async_logging.Stop();
}