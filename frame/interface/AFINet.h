/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2019 ArkNX authors.
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

#include "brynet/net/TCPService.h"
#include "brynet/net/http/HttpService.h"
#include "base/AFMacros.hpp"
#include "base/AFLockFreeQueue.hpp"
#include "base/AFBuffer.hpp"
#include "base/AFNetMsg.hpp"
#include "base/AFNetEvent.hpp"

namespace ark
{

    class AFINet;

    using NET_MSG_FUNCTOR = std::function<void(const AFNetMsg*, const int64_t)>;
    using NET_MSG_FUNCTOR_PTR = std::shared_ptr<NET_MSG_FUNCTOR>;

    using NET_EVENT_FUNCTOR = std::function<void(const AFNetEvent*)>;
    using NET_EVENT_FUNCTOR_PTR = std::shared_ptr<NET_EVENT_FUNCTOR>;

    class AFINet
    {
    public:
        AFINet() = default;
        virtual ~AFINet() = default;

        //need to call this function every frame to drive network library
        virtual void Update() = 0;
        virtual bool StartClient(AFHeadLength head_len, const int dst_busid, const std::string& ip, const int port, bool ip_v6 = false)
        {
            return false;
        }

        virtual bool StartServer(AFHeadLength head_len, const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6 = false)
        {
            return false;
        }

        virtual bool Shutdown() = 0;

        virtual bool SendMsg(AFMsgHead* head, const char* msg_data, const int64_t session_id) = 0;
        virtual bool BroadcastMsg(AFMsgHead* head, const char* msg_data)
        {
            return false;
        }

        virtual bool CloseSession(const int64_t& session_id) = 0;

        bool IsWorking() const
        {
            return working_;
        }

        void SetWorking(bool value)
        {
            working_ = value;
        }

    private:
        bool working_{ false };

    public:
        size_t statistic_recv_size_{ 0 };
        size_t statistic_send_size_{ 0 };
    };

}