#include "windz/base/Util.h"
#include "windz/util/Daemon.h"

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace windz;

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s [ start | stop | restart ]\n", basename(argv[0]));
        exit(1);
    }
    daemon::Process(argv[1]);
    // system::Signal(SIGQUIT, SIG_IGN);
    sleep(10);
}