//
// Created by crystalwind on 19-1-19.
//

#include "windz/Daemon.h"
#include "windz/Util.h"

#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

using namespace windz;

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s [ start | stop | restart ]\n", basename(argv[0]));
        exit(1);
    }
    daemon::Process(argv[1]);
    //system::Signal(SIGQUIT, SIG_IGN);
    sleep(10);
}