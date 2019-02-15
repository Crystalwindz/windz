add_executable(daemon daemon.cpp)
target_link_libraries(daemon windz_util windz_base windz_log windz_net)