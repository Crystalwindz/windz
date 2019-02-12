#include "Logger.h"
#include "CurrentThread.h"

#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

namespace windz {

namespace {

__thread char t_errnobuf[512];
__thread char t_timebuf[64];

const char *strerror_t(int saved_errno) {
    return strerror_r(saved_errno, t_errnobuf, sizeof(t_errnobuf));
}

const char *LogLevelName[Logger::LOG_LEVELS_NUM] = {
        "TRACE ",
        "DEBUG ",
        "INFO  ",
        "WARN  ",
        "ERROR ",
        "FATAL ",
};

class T {
  public:
    T(const char *str, size_t len)
            : str_(str), len_(len) {
        assert(strlen(str) == len);
    }

    const char *str_;
    const size_t len_;
};

LogStream &operator<<(LogStream &s, const T &v) {
    s.Append(v.str_, v.len_);
    return s;
}

void DefaultOutput(const char *msg, size_t len) {
    fwrite(msg, 1, len, stdout);
}

void DefaultFlush() {
    fflush(stdout);
}

}  // namespace

Logger::LogLevel g_loglevel = Logger::INFO;
Logger::OutputFunc g_output = DefaultOutput;
Logger::FlushFunc g_flush = DefaultFlush;

Logger::Logger(const char *filename, int line, LogLevel level, const char *func, int saved_errno)
        : stream_(),
          filename_(filename),
          line_(line),
          level_(level) {
    const char *p = strrchr(filename, '/');
    if (p) {
        filename_ = p + 1;
    }
    FormatTime();
    currentthread::tid();
    stream_ << T(currentthread::tid_str(), currentthread::tid_str_len());
    stream_ << T(LogLevelName[level_], 6);
    if (saved_errno != 0) {
        stream_ << strerror_t(saved_errno) << " (errno=" << saved_errno << ") ";
    }
    if (func != nullptr) {
        stream_ << func << " ";
    }
}

Logger::Logger(const char *filename, int line, LogLevel level, const char *func)
        : Logger(filename, line, level, func, 0) { }

Logger::Logger(const char *filename, int line, LogLevel level, int saved_errno)
        : Logger(filename, line, level, nullptr, saved_errno) { }

Logger::Logger(const char *filename, int line, LogLevel level)
        : Logger(filename, line, level, nullptr, 0) { }

Logger::Logger(const char *filename, int line)
        : Logger(filename, line, INFO) { }

Logger::~Logger() {
    stream_ << " - " << filename_ << ':' << line_ << '\n';
    const LogStream::Buffer &buffer(stream().buffer());
    g_output(buffer.data(), buffer.Length());
    if (level_ == LogLevel::FATAL) {
        g_flush();
        abort();
    }
}

void Logger::FormatTime() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    time_t seconds = static_cast<time_t>(tv.tv_sec);
    int microseconds = static_cast<int>(tv.tv_usec);
    struct tm tm;
    localtime_r(&seconds, &tm);
    int len = snprintf(t_timebuf, sizeof(t_timebuf),
                       "%4d%02d%02d %02d:%02d:%02d.%06d ",
                       tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
                       tm.tm_hour, tm.tm_min, tm.tm_sec, microseconds);
    assert(len == 25); static_cast<void>(len);
    stream_ << T(t_timebuf, 25);
}

Logger::LogLevel Logger::Level() {
    return g_loglevel;
}

void Logger::SetLevel(LogLevel level) {
    g_loglevel = level;
}

void Logger::SetOutput(OutputFunc output) {
    g_output = output;
}

void Logger::SetFlush(FlushFunc flush) {
    g_flush = flush;
}


}  // namespace windz
