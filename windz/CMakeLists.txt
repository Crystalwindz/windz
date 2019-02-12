set(SRCS
    Buffer.cpp
    Channel.cpp
    Config.cpp
    CurrentThread.cpp
    Daemon.cpp
    EventLoop.cpp
    Epoller.cpp
    Socket.cpp
    Thread.cpp
    ThreadPool.cpp
    Util.cpp
    Timer.cpp
    EventLoopThread.cpp
    EventLoopThreadPool.cpp
    Acceptor.cpp
    Connector.cpp
    TcpServer.cpp
    TcpConnection.cpp
    TcpClient.cpp
    FileUtil.cpp
    LogFile.cpp
    AsyncLogging.cpp
    LogStream.cpp
    Logger.cpp)

add_library(windz ${SRCS})
target_link_libraries(windz pthread)

add_subdirectory(test)