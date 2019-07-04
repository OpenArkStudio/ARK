/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2019 ArkNX authors.
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

#include "base/AFPlatform.hpp"
#include "base/AFMacros.hpp"
#include "base/AFNoncopyable.hpp"

#pragma pack(push, 1)

namespace ark
{
    ARK_CONSTEXPR static const int ARK_TCP_RECV_BUFFER_SIZE = 10 * 1024 * 1024; //10M
    ARK_CONSTEXPR static const int ARK_HTTP_RECV_BUFFER_SIZE = 1024 * 1024; //1M
    ARK_CONSTEXPR static const std::chrono::seconds ARK_CONNECT_TIMEOUT = std::chrono::seconds(5); //5s
    ARK_CONSTEXPR static const std::chrono::seconds ARK_NET_HEART_TIME = std::chrono::seconds(30);//30s
    ARK_CONSTEXPR static const int ARK_PROCESS_NET_MSG_COUNT_ONCE = 100;
    ARK_CONSTEXPR static const int ARK_MSG_MAX_LENGTH = 1024 * 5; //5K

    enum AFHeadLength
    {
        CS_HEAD_LENGTH = 6,  //cs head
        SS_HEAD_LENGTH = 22, //ss head
    };

    class AFMsgHead
    {
    public:
        uint16_t id_{ 0 };      //Msg id
        uint32_t length_{ 0 };  //Msg length(without header length)
        //uint8_t children_{ 0 }; //The number of the children msg(include self)
    };

    /*
    | msg id | msg len |
    |    2   |    4    | = 6
    */
    class AFCSMsgHead : public AFMsgHead
    {
    };

    /*
    | msg id | msg len | actor id | src bus | dst bus |
    |    2   |    4    |     8    |    4    |    4    | = 22
    */
    class AFSSMsgHead : public AFMsgHead
    {
    public:
        int64_t actor_id_{ 0 }; //Actor id
        int32_t src_bus_{ 0 };  //Source bus id
        int32_t dst_bus_{ 0 };  //Destination bus id
    };

    class AFNetMsg : public AFSSMsgHead, public AFNoncopyable
    {
    public:
        static AFNetMsg* AllocMsg(uint32_t len)
        {
            AFNetMsg* msg = ARK_NEW AFNetMsg();
            msg->AllocData(len);
            return msg;
        }

        static void Release(AFNetMsg*& msg)
        {
            if (msg != nullptr)
            {
                msg->DeallocData();
                ARK_DELETE(msg);
            }
        }

        void CopyData(const char* data, const uint32_t len)
        {
            ARK_ASSERT_RET_NONE(data != nullptr && len > 0);
            this->length_ = len;
            memcpy(this->msg_data_, data, len);
        }

        void CopyFrom(AFNetMsg* msg)
        {
            this->id_ = msg->id_;
            //this->children_ = msg->children_;
            this->actor_id_ = msg->actor_id_;
            this->src_bus_ = msg->src_bus_;
            this->dst_bus_ = msg->dst_bus_;
            if (msg->length_ > 0)
            {
                CopyData(msg->msg_data_, msg->length_);
            }
        }

        void CopyFrom(const int64_t actor_id, const uint16_t msg_id, const char* data, const uint32_t len, int src_bus, int dst_bus)
        {
            this->id_ = msg_id;
            //this->children_ = 0;
            this->actor_id_ = actor_id;
            this->src_bus_ = src_bus;
            this->dst_bus_ = dst_bus;
            CopyData(data, len);
        }

    protected:
        void AllocData(uint32_t len)
        {
            if (len > 0)
            {
                msg_data_ = ARK_NEW char[len];
            }
        }

        void DeallocData()
        {
            if (msg_data_ != nullptr)
            {
                ARK_DELETE_ARRAY(char, msg_data_);
            }

            length_ = 0;
        }

    public:
        char* msg_data_{ nullptr };
    };

}

#pragma pack(pop)