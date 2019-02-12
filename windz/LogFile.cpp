#include "LogFile.h"
#include "Noncopyable.h"
#include "Mutex.h"
#include "Memory.h"
#include "FileUtil.h"

#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

namespace windz {

namespace {

std::string MakeLogFileName(const std::string &basename, time_t *now) {
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(nullptr);
    localtime_r(now, &tm);
    strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S", &tm);
    filename += timebuf;

    char pidbuf[32];
    snprintf(pidbuf, sizeof(pidbuf), ".%d", getpid());
    filename += pidbuf;

    filename += ".log";

    return filename;
}

}  // namespace

LogFile::LogFile(const std::string &basename,
                 off_t roll_size,
                 bool thread_safe,
                 int flush_interval,
                 int check_every_n)
        : basename_(basename),
          roll_size_(roll_size),
          flush_interval_(flush_interval),
          check_every_n_(check_every_n),
          count_(0),
          mutex_(thread_safe ? MakeUnique<Mutex>() : nullptr),
          start_of_day_(0),
          last_roll_(0),
          last_flush_(0) {
    RollFile();
}

LogFile::~LogFile() = default;

void LogFile::Append(const char *logline, size_t len) {
    if (mutex_){
        LockGuard lock(*mutex_);
        AppendUnlocked(logline, len);
    } else {
        AppendUnlocked(logline, len);
    }

}

void LogFile::AppendUnlocked(const char *logline, size_t len) {
    file_->Append(logline, len);

    if (file_->written_byte() > roll_size_) {
        RollFile();
    } else {
        ++count_;
        if (count_ >= check_every_n_) {
            count_ = 0;
            time_t now = time(nullptr);
            time_t start_of_day = now / kRollPerSeconds * kRollPerSeconds;
            if (start_of_day != start_of_day_) {
                RollFile();
            } else if (now - last_flush_ >= flush_interval_) {
                last_flush_ = now;
                file_->Flush();
            }
        }
    }
}

void LogFile::Flush() {
    if (mutex_){
        LockGuard lock(*mutex_);
        file_->Flush();
    } else {
        file_->Flush();
    }
}

bool LogFile::RollFile() {
    time_t now = 0;
    std::string filename = MakeLogFileName(basename_, &now);

    if (now > last_roll_) {
        last_roll_ = now;
        last_flush_ = now;
        start_of_day_ = now / kRollPerSeconds * kRollPerSeconds;
        file_ = MakeUnique<AppendFile>(filename);
        return true;
    }
    return false;
}

}  // namespace windz
