set(SRCS Acceptor.cpp
        AsyncLogging.cpp
        Buffer.cpp
        Channel.cpp
        Config.cpp
        Connector.cpp
        CurrentThread.cpp
        Daemon.cpp
        Epoller.cpp
        EventLoop.cpp
        EventLoopThread.cpp
        EventLoopThreadPool.cpp
        FileUtil.cpp
        LogFile.cpp
        Logger.cpp
        LogStream.cpp
        Socket.cpp
        TcpClient.cpp
        TcpConnection.cpp
        TcpServer.cpp
        Thread.cpp
        ThreadPool.cpp
        Timer.cpp
        Util.cpp)

add_library(windz ${SRCS})
target_link_libraries(windz pthread)

add_subdirectory(test)