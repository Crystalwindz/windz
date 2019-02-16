# windz [![Build Status](https://travis-ci.org/Crystalwindz/windz.svg?branch=master)](https://travis-ci.org/Crystalwindz/windz) [![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT)

## Introduction

本项目是一个基于Reactor模式、使用C++11编写的网络库，既提供了锁、条件变量、阻塞队列、线程池、日志等基础设施，也提供了事件循环、Tcp服务器、Tcp客户端、定时器、缓冲区等网络设施，高度借鉴了muduo的实现。

## Build

~~~
./build.sh
~~~

## Technical points

* 使用Epoll进行IO复用，非阻塞IO，使用Reactor模式
* 使用基于最小堆的定时器
* 使用智能指针等RAII机制
* 利用eventfd实现线程的异步唤醒
* 使用双缓冲区实现了异步日志
* 使用one loop per thread作为线程模型
* Tcp客户端支持自动重连
* 支持优雅关闭连接

## Examples

example文件夹下：

* asynclog：异步日志的使用方法
* daemon：程序以daemon方式启动，从conf文件中获取日志参数
* echo：简单的echo服务
* runinloop：如何在指定的IO线程中执行用户回调
* timer：定时器的使用

## Directory Structure

* windz:
  * base: 基础库
    * Atomic.h 原子类型
    * BlockingQueue.h 有界阻塞队列
    * Condition.h 条件变量
    * CountDownLatch.h 倒数门闩，用来同步线程
    * CurrentThread.{h,cpp} 当前线程信息
    * Duration.h 时间段
    * Memory.h RAII的一些设施
    * Mutex.h 锁
    * Noncopyable.h 不可拷贝的工具类
    * Thread.{h,cpp} 线程对象
    * ThreadPool.{h,cpp} 线程池
    * Timestamp.h 时间戳
    * Util.{h,cpp} 其他一些实用设施
  * log:
    * AsyncLogging.{h,cpp} 异步日志
    * FileUtil.{h,cpp} 封装底层文件操作
    * LogFile.{h,cpp} 日志文件操作
    * Logger.{h,cpp} 日志器
    * LogStream.{h,cpp} 日志流
  * net:
    * Acceptor.{h,cpp} 接收器，用于TcpServer接受连接
    * Buffer.{h,cpp} 缓冲区
    * CallBack.h 回调类型声明
    * Channel.{h,cpp} 通道，用于fd事件分发
    * Connector.{h,cpp} 连接器，用于TcpClient建立连接
    * Epoller.{h,cpp} IO复用器
    * EventLoop.{h,cpp} 事件分发器
    * EventLoopThread.{h,cpp} 新建一个专门用于事件分发的线程
    * EventLoopThreadPool.{h,cpp} one loop per thread线程模型
    * Socket.{h,cpp} 封装底层socket操作
    * TcpClient.{h,cpp} Tcp客户端
    * TcpConnection.{h,cpp} Tcp连接
    * TcpServer.{h,cpp} Tcp服务器
    * Timer.{h,cpp} 定时器
  * util:
    * Config.{h,cpp} 配置文件解析器
    * Daemon.{h,cpp} 守护进程