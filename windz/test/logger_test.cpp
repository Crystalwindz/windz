//
// Created by crystalwind on 19-2-12.
//

#include "windz/Logger.h"

#include <limits.h>
#include <float.h>

#include <string>

using std::string;

using namespace windz;

int main(int argc, char **argv) {
    Logger::SetLevel(Logger::TRACE);
    // test LogStream
    LOG_INFO << "bool " << true << false;
    LOG_INFO << "short " << static_cast<short>(INT16_MIN);
    LOG_INFO << "u short " << static_cast<short>(UINT16_MAX);
    LOG_INFO << "int " << INT32_MIN;
    LOG_INFO << "u int " << UINT32_MAX;
    LOG_INFO << "long " << INT64_MIN;
    LOG_INFO << "u long " << UINT64_MAX;
    LOG_INFO << "long long " << static_cast<long long>(INT64_MAX);
    LOG_INFO << "float " << FLT_MAX << " " << FLT_MIN;
    LOG_INFO << "double " << DBL_MAX << " " << DBL_MIN;
    LOG_INFO << "char " << 'd' <<'f';
    LOG_INFO << static_cast<const char *>(nullptr);
    int a;
    LOG_INFO << &a;
    LOG_INFO << string("test std::string");

    // test Logger
    LOG_TRACE << "test trace";
    LOG_DEBUG << "test debug";
    LOG_INFO << "test info";
    LOG_WARN << "test warn";
    LOG_ERROR << "test error";
    errno = 14;
    LOG_SYSERR << "test syserr";
    //LOG_FATAL << "test fatal";
    LOG_SYSFATAL << "test sysfatal";
}
