set(util_SRCS
        Config.cpp
        Daemon.cpp)

add_library(windz_util ${util_SRCS})
target_link_libraries(windz_util windz_base)

add_subdirectory(test)