set(base_SRCS
        CurrentThread.cpp
        Thread.cpp
        ThreadPool.cpp
        Util.cpp)

add_library(windz_base ${base_SRCS})
target_link_libraries(windz_base pthread)

add_subdirectory(test)