#include "Daemon.h"
#include "Memory.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <signal.h>
#include <string.h>

#include <string>

namespace windz {

namespace {

bool WritePidFile(const char *pidfile) {
    char pid[16];
    int fd = open(pidfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0 || lockf(fd, F_TLOCK, 0) < 0) {
        fprintf(stderr, "can't write pidfile: %s\n", pidfile);
        return false;
    }
    ExitCaller close_fd([fd]{ close(fd); });
    snprintf(pid, sizeof(pid), "%d", getpid());
    ssize_t len = strlen(pid);
    if (write(fd, pid, len) != len) {
        fprintf(stderr, "can't write pidfile: %s\n", pidfile);
        return false;
    }
    return true;
}

int GetPidFromFile(const char *pidfile) {
    char buf[16];
    int fd = open(pidfile, O_RDONLY, 0);
    if (fd < 0) {
        return -1;
    }
    ExitCaller close_fd([fd]{ close(fd); });
    if (read(fd, buf, sizeof(buf)) <= 0) {
        return -1;
    }
    buf[15] = '\0';
    return atoi(buf);
}

}  // namespace

bool daemon::Start(const char *pidfile) {
    int pid = GetPidFromFile(pidfile);
    if (pid > 0) {
        if (kill(pid, 0) == 0 || errno == EPERM) {
            fprintf(stderr, "program %d already exist.\n", pid);
            return false;
        }
    }

    if (getppid() == 1) {
        fprintf(stderr, "already daemon.\n");
        return false;
    }

    umask(0);
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "1st fork error: %d\n", pid);
        return false;
    }
    if (pid > 0) {
        exit(0);
    }
    setsid();
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "2nd fork error: %d\n", pid);
        return false;
    }
    if (pid > 0) {
        exit(0);
    }
    if (!WritePidFile(pidfile)) {
        return false;
    }
    int fd = open("/dev/null", 0);
    ExitCaller close_fd([fd]{ close(fd); });
    if (fd >= 0) {
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        std::string file(pidfile);
        static ExitCaller unlink_file([file]{ unlink(file.c_str()); });
        return true;
    }
    return false;
}

bool daemon::Stop(const char *pidfile) {
    int pid = GetPidFromFile(pidfile);
    if (pid <= 0) {
        fprintf(stderr, "%s not exist or not valid.\n", pidfile);
        return false;
    }
    if (kill(pid, SIGQUIT) != 0) {
        if (errno == EPERM) {
            fprintf(stderr, "program %d permission denied.\n", pid);
        } else {
            fprintf(stderr, "program %d not exist.\n", pid);
        }
        return false;
    }
    for (int i = 0; i < 500; ++i) {
        usleep(10 * 1000);
        if (kill(pid, SIGQUIT) != 0) {
            unlink(pidfile);
            return true;
        }
    }
    fprintf(stderr, "program %d still exists after 5 seconds (500 SIGQUIT sended).\n", pid);
    return false;
}

bool daemon::Restart(const char *pidfile) {
    int pid = GetPidFromFile(pidfile);
    if (pid > 0) {
        if (kill(pid, 0) == 0) {
            if (!daemon::Stop(pidfile)) {
                return false;
            }
        } else if (errno == EPERM) {
            fprintf(stderr, "program %d permission denied.\n", pid);
            return false;
        } else {
            fprintf(stderr, "program %d has exited, just start.\n", pid);
        }
    } else {
        fprintf(stderr, "%s not valid, ignore and start.\n", pidfile);
    }
    return daemon::Start(pidfile);
}

void daemon::Process(const char *cmd) {
    std::string program = basename(getenv("_"));
    std::string pidfile = program + ".pid";

    if (cmd == nullptr || strcmp(cmd, "start") == 0) {
        if (!daemon::Start(pidfile.c_str())) {
            exit(1);
        }
    } else if (strcmp(cmd, "stop") == 0) {
        if (daemon::Stop(pidfile.c_str())) {
            exit(0);
        } else {
            exit(1);
        }
    } else if (strcmp(cmd, "restart") == 0) {
        if (!daemon::Restart(pidfile.c_str())) {
            exit(1);
        }
    } else {
        fprintf(stderr, "Usage: %s [ start | stop | restart ]\n", program.c_str());
        exit(1);
    }
}

}  // namespace windz