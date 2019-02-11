//
// Created by crystalwind on 19-2-10.
//

#ifndef WINDZ_LOGFILE_H
#define WINDZ_LOGFILE_H

#include "Noncopyable.h"
#include "Mutex.h"
#include "FileUtil.h"
#include <time.h>
#include <string>
#include <memory>

namespace windz {

class LogFile : private Noncopyable {
  public:
    LogFile(const std::string &basename,
            off_t roll_size,
            bool thread_safe = true,
            int flush_interval = 3,
            int check_every_n = 1024);
    ~LogFile();

    void Append(const char *logline, size_t len);
    void Flush();
    bool RollFile();

  private:
    void AppendUnlocked(const char *logline, size_t len);

    const static int kRollPerSeconds = 60*60*24;

    const std::string basename_;
    const off_t roll_size_;
    const int flush_interval_;
    const int check_every_n_;
    int count_;

    std::unique_ptr<Mutex> mutex_;
    time_t start_of_day_;
    time_t last_roll_;
    time_t last_flush_;
    std::unique_ptr<AppendFile> file_;
};

}  // namespace windz

#endif //WINDZ_LOGFILE_H
