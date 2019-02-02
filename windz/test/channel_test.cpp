//
// Created by crystalwind on 18-12-31.
//

#include "windz/EventLoop.h"
#include "windz/Channel.h"
#include <sys/timerfd.h>
#include <string.h>
#include <memory>

using namespace windz;
using namespace std;

int main()
{
    {
        EventLoop loop;
        int timefd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

        {
            ChannelSPtr ch = make_shared<Channel>(&loop, timefd);
            ch->SetReadHandler([ch] {
                printf("TIMEOUT!\n");
                ch->DisableRead();
            });
            ch->EnableRead();

            loop.RunAfter(2, [ch] {
                printf("%ld\n", ch.use_count());
                ch->DisableReadWrite();
                ch->loop()->Quit();
                ch->Close();
                printf("%ld\n", ch.use_count());
            });
        }

        struct itimerspec howlong;
        bzero(&howlong, sizeof(howlong));
        howlong.it_value.tv_sec = 1;
        ::timerfd_settime(timefd, 0, &howlong, NULL);

        loop.Loop();
        ::close(timefd);
    }

    sleep(4);
}