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

#include "SDK/Core/AFPlatform.hpp"
#include "SDK/Core/AFMacros.hpp"

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
        SS_HEAD_LENGTH = 22, //ss head
    };

    class AFIMsgHead
    {
    public:
        virtual ~AFIMsgHead() = default;
        virtual uint32_t get_body_length() const = 0;
        virtual int encode(char* buffer) = 0;
        virtual int decode(const char* buffer) = 0;
    };
    /*
    +--------+---------+----------+
    | msg_id | msg_len | actor_id |
    +--------+---------+----------+
    | 2bytes |  4bytes |  8bytes  |
    |           14bytes           |
    */
    class AFCSMsgHead : public AFIMsgHead
    {
    public:
        uint16_t msg_id_{ 0 };
        uint32_t msg_len_{ 0 };
        int64_t actor_id_{ 0 }; //进游戏前是session_id, 进入游戏后是actor_id
        //uint8_t seq_{0};
        //uint32_t verify_code_{0};
        //uint8_t child_count_{0};

        uint32_t get_body_length() const override
        {
            return msg_len_;
        }

        int encode(char* buffer) override
        {
            uint32_t offset(0);

            uint16_t msg_id = ARK_HTONS(msg_id_);
            memcpy(buffer + offset, (void*)(&msg_id), sizeof(msg_id_));
            offset += sizeof(msg_id_);

            uint32_t msg_len = msg_len_ + CS_HEAD_LENGTH;
            uint32_t size = ARK_HTONL(msg_len);
            memcpy(buffer + offset, (void*)(&size), sizeof(msg_len_));
            offset += sizeof(msg_len_);

            uint64_t actor_id = ARK_HTONLL(actor_id_);
            memcpy(buffer + offset, (void*)(&actor_id), sizeof(actor_id));
            offset += sizeof(actor_id);

            if (offset != CS_HEAD_LENGTH)
            {
                assert(0);
            }

            return offset;
        }

        int decode(const char* buffer) override
        {
            uint32_t offset(0);

            uint16_t msg_id(0);
            memcpy(&msg_id, buffer + offset, sizeof(msg_id_));
            msg_id_ = ARK_NTOHS(msg_id);
            offset += sizeof(msg_id_);

            uint32_t msg_len(0);
            memcpy(&msg_len, buffer + offset, sizeof(msg_len_));
            msg_len_ = ARK_NTOHL(msg_len) - CS_HEAD_LENGTH;
            offset += sizeof(msg_len_);

            uint64_t actor_id(0);
            memcpy(&actor_id, buffer + offset, sizeof(actor_id));
            actor_id_ = ARK_NTOHLL(actor_id);
            offset += sizeof(actor_id);

            if (offset != CS_HEAD_LENGTH)
            {
                assert(0);
            }

            return offset;
        }
    };

    /*
    +--------+---------+----------+-----------+-----------+
    | msg_id | msg_len | actor_id |  src_bus  |  dst_bus  |
    +--------+---------+----------+-----------+-----------+
    | 2bytes |  4bytes |  8bytes  |  4bytes   |  4bytes   |
    |                      22bytes                        |
    */
    class AFSSMsgHead : public AFIMsgHead
    {
    public:
        uint16_t msg_id_{ 0 };
        uint32_t msg_len_{ 0 };
        int64_t actor_id_{ 0 };
        int32_t src_bus_{ 0 };
        int32_t dst_bus_{ 0 };
        //uint8_t child_pkg_count_{ 0 };

        uint32_t get_body_length() const override
        {
            return msg_len_;
        }

        int encode(char* buffer) override
        {
            uint32_t offset(0);

            uint16_t msg_id = ARK_HTONS(msg_id_);
            memcpy(buffer + offset, (void*)(&msg_id), sizeof(msg_id_));
            offset += sizeof(msg_id_);

            uint32_t msg_len = msg_len_ + SS_HEAD_LENGTH;
            uint32_t size = ARK_HTONL(msg_len);
            memcpy(buffer + offset, (void*)(&size), sizeof(msg_len_));
            offset += sizeof(msg_len_);

            uint64_t actor_id = ARK_HTONLL(actor_id_);
            memcpy(buffer + offset, (void*)(&actor_id), sizeof(actor_id));
            offset += sizeof(actor_id);

            uint32_t src_bus = ARK_HTONL(src_bus_);
            memcpy(buffer + offset, (void*)(&src_bus), sizeof(src_bus_));
            offset += sizeof(src_bus_);

            uint32_t dst_bus = ARK_HTONL(dst_bus_);
            memcpy(buffer + offset, (void*)(&dst_bus), sizeof(dst_bus_));
            offset += sizeof(dst_bus_);

            if (offset != SS_HEAD_LENGTH)
            {
                assert(0);
            }

            return offset;
        }

        int decode(const char* buffer) override
        {
            uint32_t offset(0);

            uint16_t msg_id(0);
            memcpy(&msg_id, buffer + offset, sizeof(msg_id_));
            msg_id_ = ARK_NTOHS(msg_id);
            offset += sizeof(msg_id_);

            uint32_t msg_len(0);
            memcpy(&msg_len, buffer + offset, sizeof(msg_len_));
            msg_len_ = ARK_NTOHL(msg_len) - SS_HEAD_LENGTH;
            offset += sizeof(msg_len_);

            uint64_t actor_id(0);
            memcpy(&actor_id, buffer + offset, sizeof(actor_id));
            actor_id_ = ARK_NTOHLL(actor_id);
            offset += sizeof(actor_id);

            uint32_t src_bus(0);
            memcpy(&src_bus, buffer + offset, sizeof(src_bus_));
            src_bus_ = ARK_NTOHL(msg_len);
            offset += sizeof(src_bus_);

            uint32_t dst_bus(0);
            memcpy(&dst_bus, buffer + offset, sizeof(dst_bus_));
            dst_bus_ = ARK_NTOHL(dst_bus);
            offset += sizeof(dst_bus_);

            if (offset != SS_HEAD_LENGTH)
            {
                assert(0);
            }

            return offset;
        }
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