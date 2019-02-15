#include "windz/util/Config.h"
#include "windz/util/Daemon.h"
#include "windz/log/AsyncLogging.h"
#include "windz/log/Logger.h"
#include "windz/net/EventLoop.h"
#include "windz/net/TcpServer.h"

#include <string>

using namespace windz;

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s [ start | stop | restart ]\n", basename(argv[0]));
        exit(1);
    }
    daemon::Process(argv[1]);

    Config config;
    int r = config.Parse("daemon.conf");
    if (r != 0) {
        LOG_FATAL << "parse daemon.conf error, return "<<r;
    }
    std::string logfile = config.GetString("", "logfile", basename(argv[0]));
    std::string loglevel = config.GetString("", "loglevel", "INFO");
    off_t rollsize = config.GetInt("", "rollsize", 1*1024*1024*1024);

    Logger::LogLevel level;
    if (loglevel == "TRACE") {
        level = Logger::TRACE;
    } else if (loglevel == "DEBUG") {
        level = Logger::DEBUG;
    } else if (loglevel == "INFO") {
        level = Logger::INFO;
    } else if (loglevel == "WARN") {
        level = Logger::WARN;
    } else if (loglevel == "ERROR") {
        level = Logger::ERROR;
    } else if (loglevel == "FATAL") {
        level = Logger::FATAL;
    } else {
        LOG_WARN << "unknown loglevel, use INFO";
    }
    Logger::SetLevel(level);

    AsyncLogging asynclogging(logfile, rollsize);
    Logger::SetOutput(
            [&asynclogging](const char *msg, size_t len) {
        asynclogging.Append(msg, len);
    });
    asynclogging.Start();

    EventLoop loop;
    InetAddr listenaddr(2019);
    TcpServer tcpserver(&loop, listenaddr, "echo");
    tcpserver.SetConnectionCallBack(
            [](const TcpConnectionPtr &conn) {
        LOG_INFO << conn->peer_addr().IpPortString() << " -> "
                 << conn->local_addr().IpPortString()
                    << (conn->connected() ? "connect" : "disconnect");
    });
    tcpserver.SetMessageCallBack(
            [](const TcpConnectionPtr &conn, Buffer &buf) {
                conn->Send(buf);
    });
    tcpserver.Start(4);
    loop.Loop();
}