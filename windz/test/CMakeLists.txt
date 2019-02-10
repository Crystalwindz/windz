add_executable(atomic_test atomic_test.cpp)
target_link_libraries(atomic_test windz)

add_executable(blockingqueue_test blockingqueue_test.cpp)
target_link_libraries(blockingqueue_test windz)

add_executable(buffer_test buffer_test.cpp)
target_link_libraries(buffer_test windz)

add_executable(channel_test channel_test.cpp)
target_link_libraries(channel_test windz)

add_executable(config_test config_test.cpp)
target_link_libraries(config_test windz)

add_executable(countdownlatch_test countdownlatch_test.cpp)
target_link_libraries(countdownlatch_test windz)

add_executable(daemon_test daemon_test.cpp)
target_link_libraries(daemon_test windz)

add_executable(eventloop_test eventloop_test.cpp)
target_link_libraries(eventloop_test windz)

add_executable(eventloopthreadpool_test eventloopthreadpool_test.cpp)
target_link_libraries(eventloopthreadpool_test windz)

add_executable(tcpserver_test tcpserver_test.cpp)
target_link_libraries(tcpserver_test windz)

add_executable(tcpclient_test1 tcpclient_test1.cpp)
target_link_libraries(tcpclient_test1 windz)

add_executable(tcpclient_test2 tcpclient_test2.cpp)
target_link_libraries(tcpclient_test2 windz)

add_executable(tcpclient_test3 tcpclient_test3.cpp)
target_link_libraries(tcpclient_test3 windz)

add_executable(thread_test thread_test.cpp)
target_link_libraries(thread_test windz)

add_executable(threadpool_test threadpool_test.cpp)
target_link_libraries(threadpool_test windz)

add_executable(timer_test timer_test.cpp)
target_link_libraries(timer_test windz)
