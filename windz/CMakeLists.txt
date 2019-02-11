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
    TcpServer.cpp TcpConnection.cpp TcpConnection.h TcpClient.cpp TcpClient.h FileUtil.cpp FileUtil.h LogFile.cpp LogFile.h AsyncLogging.cpp AsyncLogging.h LogStream.cpp LogStream.h Logger.cpp Logger.h)

set(HEADER
        Acceptor.h
        Atomic.h
        BlockingQueue.h
        Buffer.h
        Channel.h
        Condition.h
        Config.h
        CountDownLatch.h
        CurrentThread.h
        Daemon.h
        Duration.h
        Epoller.h
        EventLoop.h
        EventLoopThread.h
        EventLoopThreadPool.h
        Mutex.h
        Socket.h
        Thread.h
        ThreadPool.h
        Timer.h
        Timestamp.h
        Util.h
        )

add_library(windz ${SRCS})
target_link_libraries(windz pthread)

add_subdirectory(test)