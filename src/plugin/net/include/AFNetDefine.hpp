/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once

#include "base/AFMacros.hpp"

// open kcp console log
// #define KCP_OPEN_LOG

struct IKCPCB;
using ikcpcb = struct IKCPCB;

namespace ark {

class AFINet;
class AFNetMsg;
class AFNetEvent;

ARK_CONSTEXPR static const int ARK_SOCKET_SEND_SIZE = 32 * 1024; // 32K
ARK_CONSTEXPR static const int ARK_SOCKET_RECV_SIZE = 32 * 1024; // 32K

ARK_CONSTEXPR static const int ARK_TCP_RECV_BUFFER_SIZE = 1024 * 1024;                         // 1M
ARK_CONSTEXPR static const int ARK_HTTP_RECV_BUFFER_SIZE = 32 * 1024;                          // 32K
ARK_CONSTEXPR static const std::chrono::seconds ARK_CONNECT_TIMEOUT = std::chrono::seconds(5); // 5s
ARK_CONSTEXPR static const std::chrono::seconds ARK_NET_HEART_TIME = std::chrono::seconds(30); // 30s
ARK_CONSTEXPR static const std::chrono::seconds ARK_RECONNECT_TIME = std::chrono::seconds(5);  // 5s
ARK_CONSTEXPR static const int ARK_PROCESS_NET_MSG_COUNT_ONCE = 100;
ARK_CONSTEXPR static const int ARK_MSG_MAX_LENGTH = 4 * 1024;              // 4K
ARK_CONSTEXPR static const int ARK_CONV_START_ID = 1000;                   // start conv_id
ARK_CONSTEXPR static const int ARK_KCP_CONNECTION_TIMEOUT_TIME = 5 * 1000; // default is 5 seconds

ARK_CONSTEXPR static const int ARK_KCP_RECV_BUFFER_SIZE = 4 * 1024;        // 4K
ARK_CONSTEXPR static const int ARK_KCP_UPDATE_INTERVAL_TIME = 10;          // 10 ms
ARK_CONSTEXPR static const int ARK_KCP_RESEND_CONNECT_MSG_INTERVAL = 3000; // milliseconds
ARK_CONSTEXPR static const int ARK_KCP_CONNECT_TIMEOUT_TIME = 60000;       // milliseconds

enum class AFNetEventType : uint8_t
{
    NONE = 0,
    CONNECT,
    DISCONNECT,
    RECV_DATA,
    CONNECT_FAILED,
    LAG_NOTIFY,
};

enum class AFKcpProtocalType : uint8_t
{
    NONE = 0,
    SYN, // connect
    ACK, // connected
    FIN, // disconnect
    MSG, // msg
};

enum class AFKcpConnectStatus : uint8_t
{
    STATUS_NONE = 0,
    STATUS_CONNECTING,
    STATUS_CONNECTED,
};

} // namespace ark