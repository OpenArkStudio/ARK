/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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

#include "SDK/Core/AFPlatform.hpp"
#include "SDK/Core/AFMacros.hpp"
#include "SDK/Core/AFGUID.hpp"

#pragma pack(push, 1)

namespace ark
{
    ARK_CONSTEXPR static const int ARK_TCP_RECV_BUFFER_SIZE = 10 * 1024 * 1024; //10M
    ARK_CONSTEXPR static const int ARK_HTTP_RECV_BUFFER_SIZE = 1024 * 1024; //1M
    ARK_CONSTEXPR static const std::chrono::seconds ARK_CONNECT_TIMEOUT = std::chrono::seconds(5); //5s
    ARK_CONSTEXPR static const std::chrono::seconds ARK_NET_HEART_TIME = std::chrono::seconds(30);//30s
    ARK_CONSTEXPR static const int ARK_PROCESS_NET_MSG_COUNT_ONCE = 100;

    enum AFHeadLength
    {
        CS_HEAD_LENGTH = 14, //cs head
        SS_HEAD_LENGTH = 24, //ss head
    };

    class  AFIMsgHead
    {
    public:
        virtual ~AFIMsgHead() = default;
    };

    class AFCSMsgHead : public AFIMsgHead
    {
    public:
        uint32_t msg_len_{ 0 };
        uint16_t msg_id_{ 0 };
        int64_t ident_{ 0 }; //进游戏前是conn_id, 进入游戏后是actor_rid.low, actor_rid.high可以通过busid获取
        //uint8_t seq_{0};
        //uint32_t verify_code_{0};
        //uint8_t child_count_{0};
    };

    class AFSSMsgHead : public AFIMsgHead
    {
    public:
        uint32_t msg_len_{ 0 };
        uint16_t msg_id_{ 0 };
        int64_t uid_{ 0 }; //actor_rid.low, actor_rid.high可以通过busid获取
        int32_t bus_src_{ 0 };
        int32_t bus_dst_{ 0 };
        //uint8_t child_pkg_count_{ 0 };
    };

    class AFNetMsg
    {
    public:
        AFNetMsg(const AFHeadLength head_len)
        {
            switch (head_len)
            {
            case AFHeadLength::CS_HEAD_LENGTH:
                head_ptr_ = ARK_NEW AFCSMsgHead();
                break;
            case AFHeadLength::SS_HEAD_LENGTH:
                head_ptr_ = ARK_NEW AFSSMsgHead();
                break;
            default:
                ARK_ASSERT_NO_EFFECT(0);
                break;
            }
        }

        ~AFNetMsg()
        {
            ARK_DELETE(head_ptr_);
        }

        AFNetEvent event_{ NONE };
        int64_t conn_id_{ 0 };
        std::string msg_data_{};
        AFIMsgHead* head_ptr_{ nullptr };
    };

}

#pragma pack(pop)