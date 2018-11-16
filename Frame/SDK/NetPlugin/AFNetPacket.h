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

    enum AFHeadLength
    {
        CS_HEAD_LENGTH = 14, //cs head
        SS_HEAD_LENGTH = 24, //ss head
    };

    class  AFIMsgHead
    {
    public:
        virtual ~AFIMsgHead() = default;

        virtual int encode(char* strData) const = 0;
        virtual int decode(const char* strData) = 0;

        virtual uint16_t msg_id() const = 0;
        virtual void set_msg_id(uint16_t nMsgID) = 0;

        virtual uint32_t body_length() const = 0;
        virtual void set_body_length(size_t nLength) = 0;

        virtual const AFGUID& uid() const = 0;
        virtual void set_uid(const AFGUID& xPlayerID) = 0;

        int64_t ark_htonll(const int64_t nData) const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return htonll(nData);
#elif ARK_PLATFORM == PLATFORM_LINUX
            return htobe64(nData);
#else
#error Not supported OS.
#endif
        }

        int64_t ark_ntohll(const int64_t nData)const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return ntohll(nData);
#elif ARK_PLATFORM == PLATFORM_LINUX
            return be64toh(nData);
#else
#error Not supported OS.
#endif
        }

        int32_t ark_htonl(const int32_t nData)const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return htonl(nData);
#elif ARK_PLATFORM == PLATFORM_LINUX
            return htobe32(nData);
#else
#error Not supported OS.
#endif
        }

        int32_t ark_ntohl(const int32_t nData)const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return ntohl(nData);
#elif ARK_PLATFORM == PLATFORM_LINUX
            return be32toh(nData);
#else
#error Not supported OS.
#endif
        }

        int16_t ark_htons(const int16_t nData)const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return htons(nData);
#elif ARK_PLATFORM == PLATFORM_LINUX
            return htobe16(nData);
#else
#error Not supported OS.
#endif
        }

        int16_t ark_ntohs(const int16_t nData)const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return ntohs(nData);
#elif ARK_PLATFORM == PLATFORM_LINUX
            return be16toh(nData);
#else
#error Not supported OS.
#endif
        }
    };

    class AFCSMsgHead : public AFIMsgHead
    {
    public:
        AFCSMsgHead() = default;
        virtual ~AFCSMsgHead() = default;

        int encode(char* buffer) const override
        {
            uint32_t offset(0);

            uint16_t msg_id = ark_htons(msg_id_);
            memcpy(buffer + offset, (void*)(&msg_id), sizeof(msg_id_));
            offset += sizeof(msg_id_);

            uint32_t pack_size = msg_len_ + AFHeadLength::CS_HEAD_LENGTH;
            uint32_t size = ark_htonl(pack_size);
            memcpy(buffer + offset, (void*)(&size), sizeof(msg_len_));
            offset += sizeof(msg_len_);

            uint64_t uid_value = ark_htonll(uid_);
            memcpy(buffer + offset, (void*)(&uid_value), sizeof(uid_value));
            offset += sizeof(uid_value);

            if (offset != AFHeadLength::CS_HEAD_LENGTH)
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
            msg_id_ = ark_ntohs(msg_id);
            offset += sizeof(msg_id_);

            uint32_t pack_size(0);
            memcpy(&pack_size, buffer + offset, sizeof(msg_len_));
            msg_len_ = ark_ntohl(pack_size) - AFHeadLength::CS_HEAD_LENGTH;
            offset += sizeof(msg_len_);

            uint64_t uid_value(0);
            memcpy(&uid_value, buffer + offset, sizeof(uid_value));
            uid_ = ark_ntohll(uid_value);
            offset += sizeof(uid_value);

            if (offset != AFHeadLength::CS_HEAD_LENGTH)
            {
                assert(0);
            }

            return offset;
        }

        uint16_t msg_id() const override
        {
            return msg_id_;
        }

        void set_msg_id(uint16_t id) override
        {
            msg_id_ = id;
        }

        uint32_t body_length() const override
        {
            return msg_len_;
        }

        void set_body_length(size_t length) override
        {
            msg_len_ = (uint32_t)length;
        }

        const AFGUID& uid() const
        {
            return uid_;
        }

        void set_uid(const int64_t& id)
        {
            uid_ = id;
        }

    private:
        uint32_t msg_len_{ 0 };
        uint16_t msg_id_{ 0 };
        //AFGUID uid_{ 0 }; //进游戏前是conn_id, 进入游戏后是actor_rid
        int64_t uid_{ 0 }; //进游戏前是conn_id, 进入游戏后是actor_rid.low, actor_rid.high可以通过busid获取
        //uint8_t seq_{0};
        //uint32_t verify_code_{0};
        //uint8_t child_count_{0};
    };

    class AFSSMsgHead : public AFIMsgHead
    {
    public:
        AFSSMsgHead() = default;
        virtual ~AFSSMsgHead() = default;

        int encode(char* buffer) const override
        {
            uint32_t offset(0);

            uint16_t msg_id = ark_htons(msg_id_);
            memcpy(buffer + offset, (void*)(&msg_id), sizeof(msg_id_));
            offset += sizeof(msg_id_);

            uint32_t pack_size = msg_len_ + AFHeadLength::SS_HEAD_LENGTH;
            uint32_t size = ark_htonl(pack_size);
            memcpy(buffer + offset, (void*)(&size), sizeof(msg_len_));
            offset += sizeof(msg_len_);

            uint64_t uid_value = ark_htonll(uid_);
            memcpy(buffer + offset, (void*)(&uid_value), sizeof(uid_value));
            offset += sizeof(uid_value);

            uint32_t bus_src = ark_htonl(bus_src_);
            memcpy(buffer + offset, (void*)(&bus_src), sizeof(bus_src));
            offset += sizeof(bus_src);

            uint32_t bus_dst = ark_htonl(bus_dst_);
            memcpy(buffer + offset, (void*)(&bus_dst), sizeof(bus_dst));
            offset += sizeof(bus_dst);

            if (offset != AFHeadLength::SS_HEAD_LENGTH)
            {
                assert(0);
            }

            return offset;
        }

        // Message Head[ MsgID(2) | MsgSize(4) | PlayerID(16) ]
        int decode(const char* buffer) override
        {
            uint32_t offset(0);

            uint16_t msg_id(0);
            memcpy(&msg_id, buffer + offset, sizeof(msg_id_));
            msg_id_ = ark_ntohs(msg_id);
            offset += sizeof(msg_id_);

            uint32_t pack_size(0);
            memcpy(&pack_size, buffer + offset, sizeof(msg_len_));
            msg_len_ = ark_ntohl(pack_size) - SS_HEAD_LENGTH;
            offset += sizeof(msg_len_);

            uint64_t uid_value(0);
            memcpy(&uid_value, buffer + offset, sizeof(uid_value));
            uid_ = ark_ntohll(uid_value);
            offset += sizeof(uid_value);

            uint32_t bus_src(0);
            memcpy(&bus_src, buffer + offset, sizeof(bus_src_));
            bus_src_ = ark_ntohl(bus_src);
            offset += sizeof(bus_src_);

            uint32_t bus_dst(0);
            memcpy(&bus_dst, buffer + offset, sizeof(bus_dst_));
            bus_dst_ = ark_ntohl(bus_dst);
            offset += sizeof(bus_dst_);

            if (offset != SS_HEAD_LENGTH)
            {
                assert(0);
            }

            return offset;
        }

        uint16_t msg_id() const override
        {
            return msg_id_;
        }

        void set_msg_id(uint16_t id) override
        {
            msg_id_ = id;
        }

        uint32_t body_length() const override
        {
            return msg_len_;
        }

        void set_body_length(size_t length) override
        {
            msg_len_ = (uint32_t)length;
        }

        const int64_t& actor_id() const
        {
            return uid_;
        }

        void set_actor_id(const int64_t& id)
        {
            uid_ = id;
        }

    private:
        uint32_t msg_len_{ 0 };
        uint16_t msg_id_{ 0 };
        int64_t actor_id_{ 0 }; //actor_rid.low, actor_rid.high可以通过busid获取
        int32_t bus_src_{ 0 };
        int32_t bus_dst_{ 0 };
        //uint8_t child_pkg_count_{ 0 };
    };

    enum AFNetEvent
    {
        NONE            = 0,
        CONNECTED       = 1,
        DISCONNECTED    = 2,
        RECV_DATA       = 3,
    };

    using NET_PKG_RECV_FUNCTOR = std::function<void(const AFIMsgHead& head, const int msg_id, const char* msg, const uint32_t msg_len, const AFGUID& conn_id)>;
    using NET_PKG_RECV_FUNCTOR_PTR = std::shared_ptr<NET_PKG_RECV_FUNCTOR> ;

    using NET_EVENT_FUNCTOR = std::function<void(const AFNetEvent event, const AFGUID& conn_id, const std::string& ip, const int bus_id)> ;
    using NET_EVENT_FUNCTOR_PTR = std::shared_ptr<NET_EVENT_FUNCTOR>;

}

#pragma pack(pop)