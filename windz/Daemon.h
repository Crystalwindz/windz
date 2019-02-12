#ifndef WINDZ_DAEMON_H
#define WINDZ_DAEMON_H

namespace windz {

namespace daemon {

bool Start(const char *pidfile);

bool Stop(const char *pidfile);

bool Restart(const char *pidfile);

void Process(const char *cmd);

};

}  // namespace windz

#endif //WINDZ_DAEMON_H
