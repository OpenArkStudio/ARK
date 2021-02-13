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
#include "base/container/AFLockFreeQueue.hpp"
#include "base/AFBuffer.hpp"
#include "base/AFDefine.hpp"
#include "net/include/AFNetMsg.hpp"
#include "net/include/AFNetEvent.hpp"
#include "proto/AFProtoCPP.hpp"

namespace ark {

class AFINet;

using NET_MSG_FUNCTOR = std::function<void(const AFNetMsg*, conv_id_t)>;
using NET_MSG_SESSION_FUNCTOR = std::function<void(AFNetMsg const*, conv_id_t)>;
using NET_EVENT_FUNCTOR = std::function<void(AFNetEvent const*)>;

class AFINet
{
public:
    virtual ~AFINet() = default;

    // need to call this function every frame to drive network frame
    virtual void Update() = 0;
    virtual bool StartClient(std::shared_ptr<const AFIMsgHeader> head, bus_id_t dst_bus_id, const std::string& host, uint16_t port)
    {
        return false;
    }

    virtual bool StartServer(std::shared_ptr<const AFIMsgHeader> head, bus_id_t bus_id, const std::string& host, uint16_t port,
        uint8_t thread_num, uint32_t max_client, const size_t silent_timeout)
    {
        return false;
    }

    virtual void Shutdown() = 0;

    virtual bool SendMsg(AFIMsgHeader* head, const char* msg_data, conv_id_t session_id) = 0;
    virtual void BroadcastMsg(AFIMsgHeader* head, const char* msg_data) {}

    virtual void CloseSession(conv_id_t session_id) = 0;

    bool IsWorking() const
    {
        return working_;
    }

    void SetWorking(bool value)
    {
        working_ = value;
    }

    virtual std::string GetIP(const conv_id_t session_id)
    {
        return NULL_STR;
    };

private:
    bool working_{false};

public:
    // TODO: use this to statistic net traffic.
    size_t statistic_recv_size_{0};
    size_t statistic_send_size_{0};
};

} // namespace ark
