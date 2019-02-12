#ifndef WINDZ_CALLBACK_H
#define WINDZ_CALLBACK_H

#include "Timestamp.h"

#include <memory>
#include <functional>

namespace windz {

class Buffer;
class TcpConnection;

using TimerCallBack = std::function<void()>;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallBack = std::function<void (const TcpConnectionPtr &)>;
using MessageCallBack = std::function<void (const TcpConnectionPtr &, Buffer &)>;
using CloseCallBack = std::function<void (const TcpConnectionPtr &)>;
using LowWaterMarkCallBack = std::function<void (const TcpConnectionPtr &)>;
using HighWaterMarkCallBack = std::function<void (const TcpConnectionPtr &, size_t)>;

}  // namespace windz

#endif //WINDZ_CALLBACK_H
