set(net_SRCS
        Acceptor.cpp
        Buffer.cpp
        Channel.cpp
        Connector.cpp
        Epoller.cpp
        EventLoop.cpp
        EventLoopThread.cpp
        EventLoopThreadPool.cpp
        Socket.cpp
        TcpClient.cpp
        TcpConnection.cpp
        TcpServer.cpp
        Timer.cpp)

add_library(windz_net ${net_SRCS})
target_link_libraries(windz_net windz_base windz_log)

add_subdirectory(test)