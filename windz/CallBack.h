//
// Created by crystalwind on 19-1-31.
//

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
using CloseCallBack = std::function<void (const TcpConnectionPtr &)>;
using WriteCompleteCallBack = std::function<void (const TcpConnectionPtr &)>;
using HighWaterMarkCallBack = std::function<void (const TcpConnectionPtr &, size_t)>;
using MessageCallBack = std::function<void (const TcpConnectionPtr &,
                                            Buffer &, Timestamp)>;

}  // namespace windz

#endif //WINDZ_CALLBACK_H
