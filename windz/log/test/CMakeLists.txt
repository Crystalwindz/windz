add_executable(asynclogging_test asynclogging_test.cpp)
target_link_libraries(asynclogging_test windz_log)

add_executable(logger_test logger_test.cpp)
target_link_libraries(logger_test windz_log)