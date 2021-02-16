/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
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

namespace ark {

enum class AFErrorCode : uint32_t
{
    SUCCESS = 0,

    KCP_SERVER_DISCONNECT = 1000,   // kcp server disconnect when dtor
    KCP_TIME_OUT_DISCONNECT = 1001, // kcp timeout disconnect
    KCP_FORCE_DISCONNECT = 1002,    // force disconnect
    KCP_CONNECT_TIMEOUT = 1003,     // connect timeout
    PEER_DISCONNECT = 1004,         // peer(client/server) disconnect

    // will add error code
    LOGIC_ERROR_CODE_START = 10000,
};
}