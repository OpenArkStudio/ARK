/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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
#include "SDK/Core/AFMacros.hpp"
#include "SDK/Core/AFLockFreeQueue.hpp"
#include "SDK/Core/AFBuffer.hpp"
#include "AFNetMsg.h"
#include "AFNetEvent.h"

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

        //send a message with out msg-head[auto add msg-head in this function]
        virtual bool SendRawMsg(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id, const AFGUID& actor_id) = 0;

        //send a message to all client[need to add msg-head for this message by yourself]
        virtual bool SendMsgToAllClient(const char* msg, const uint32_t msg_len, const AFGUID& actor_id)
        {
            return false;
        }

        //send a message with out msg-head to all client[auto add msg-head in this function]
        virtual bool SendRawMsgToAllClient(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& actor_id)
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

    //////////////////////////////////////////////////////////////////////////

}