set(log_SRCS
        AsyncLogging.cpp
        FileUtil.cpp
        LogFile.cpp
        Logger.cpp
        LogStream.cpp)

add_library(windz_log ${log_SRCS})
target_link_libraries(windz_log windz_base)

add_subdirectory(test)