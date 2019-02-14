#ifndef WINDZ_LOGGER_H
#define WINDZ_LOGGER_H

#include "windz/log/LogStream.h"

#include <errno.h>

#include <functional>

namespace windz {

class Logger {
  public:
    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        LOG_LEVELS_NUM,
    };
    using OutputFunc = std::function<void(const char *msg, size_t len)>;
    using FlushFunc = std::function<void()>;

    Logger(const char *filename, int line, LogLevel level, const char *func, int saved_errno);
    Logger(const char *filename, int line, LogLevel level, const char *func);
    Logger(const char *filename, int line, LogLevel level, int saved_errno);
    Logger(const char *filename, int line, LogLevel level);
    Logger(const char *filename, int line);
    ~Logger();

    LogStream &stream() { return stream_; }

    static LogLevel Level();
    static void SetLevel(LogLevel level);
    static void SetOutput(OutputFunc);
    static void SetFlush(FlushFunc);

  private:
    void FormatTime();

    LogStream stream_;
    const char *filename_;
    int line_;
    LogLevel level_;
};

// From muduo
//
// CAUTION: do not write:
//
// if (good)
//   LOG_INFO << "Good news";
// else
//   LOG_WARN << "Bad news";
//
// this expends to
//
// if (good)
//   if (logging_INFO)
//     logInfoStream << "Good news";
//   else
//     logWarnStream << "Bad news";
//
#define LOG_TRACE if (windz::Logger::Level() <= windz::Logger::LogLevel::TRACE) \
    windz::Logger(__FILE__, __LINE__, windz::Logger::LogLevel::TRACE, __func__).stream()
#define LOG_DEBUG if (windz::Logger::Level() <= windz::Logger::LogLevel::DEBUG) \
    windz::Logger(__FILE__, __LINE__, windz::Logger::LogLevel::DEBUG, __func__).stream()
#define LOG_INFO if (windz::Logger::Level() <= windz::Logger::LogLevel::INFO) \
    windz::Logger(__FILE__, __LINE__, windz::Logger::LogLevel::INFO, __func__).stream()
#define LOG_WARN windz::Logger(__FILE__, __LINE__, windz::Logger::LogLevel::WARN).stream()
#define LOG_ERROR windz::Logger(__FILE__, __LINE__, windz::Logger::LogLevel::ERROR).stream()
#define LOG_FATAL windz::Logger(__FILE__, __LINE__, windz::Logger::LogLevel::FATAL).stream()
#define LOG_SYSERR windz::Logger(__FILE__, __LINE__, windz::Logger::LogLevel::ERROR, errno).stream()
#define LOG_SYSFATAL windz::Logger(__FILE__, __LINE__, windz::Logger::LogLevel::FATAL, errno).stream()

}  // namespace windz

#endif  // WINDZ_LOGGER_H
