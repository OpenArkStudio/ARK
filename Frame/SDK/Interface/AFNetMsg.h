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

    class  ARK_PKG_BASE_HEAD
    {
    public:
        enum AF_Head
        {
            ARK_CS_HEADER_LENGTH = 22,
            ARK_SS_HEADER_LENGTH = 32,
        };

        virtual ~ARK_PKG_BASE_HEAD() {}

        virtual int EnCode(char* strData) const = 0;
        virtual int DeCode(const char* strData) = 0;

        virtual uint16_t GetMsgID() const = 0;
        virtual void SetMsgID(uint16_t nMsgID) = 0;

        virtual uint32_t GetBodyLength() const = 0;
        virtual void SetBodyLength(size_t nLength) = 0;

        virtual const AFGUID& GetUID() const = 0;
        virtual void SetUID(const AFGUID& xPlayerID) = 0;

        int64_t ARK_HTONLL(const int64_t nData) const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return htonll(nData);
#elif ARK_PLATFORM == PLATFORM_APPLE
            return OSSwapHostToBigInt64(nData);
#else
            return htobe64(nData);
#endif
        }

        int64_t ARK_NTOHLL(const int64_t nData)const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return ntohll(nData);
#elif ARK_PLATFORM == PLATFORM_APPLE
            return OSSwapBigToHostInt64(nData);
#else
            return be64toh(nData);
#endif
        }

        int32_t ARK_HTONL(const int32_t nData)const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return htonl(nData);
#elif ARK_PLATFORM == PLATFORM_APPLE
            return OSSwapHostToBigInt32(nData);
#else
            return htobe32(nData);
#endif
        }

        int32_t ARK_NTOHL(const int32_t nData)const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return ntohl(nData);
#elif ARK_PLATFORM == PLATFORM_APPLE
            return OSSwapBigToHostInt32(nData);
#else
            return be32toh(nData);
#endif
        }

        int16_t ARK_HTONS(const int16_t nData)const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return htons(nData);
#elif ARK_PLATFORM == PLATFORM_APPLE
            return OSSwapHostToBigInt16(nData);
#else
            return htobe16(nData);
#endif
        }

        int16_t ARK_NTOHS(const int16_t nData)const
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return ntohs(nData);
#elif ARK_PLATFORM == PLATFORM_APPLE
            return OSSwapBigToHostInt16(nData);
#else
            return be16toh(nData);
#endif
        }
    };

    class ARK_PKG_CS_HEAD : public ARK_PKG_BASE_HEAD
    {
    public:
        ARK_PKG_CS_HEAD() = default;
        virtual ~ARK_PKG_CS_HEAD() = default;

        // Message Head[ MsgID(2) | MsgSize(4) | PlayerID(16) ]
        int EnCode(char* buffer) const override
        {
            uint32_t offset(0);

            uint16_t msg_id = ARK_HTONS(msg_id_);
            memcpy(buffer + offset, (void*)(&msg_id), sizeof(msg_id_));
            offset += sizeof(msg_id_);

            uint32_t pack_size = msg_len_ + ARK_CS_HEADER_LENGTH;
            uint32_t size = ARK_HTONL(pack_size);
            memcpy(buffer + offset, (void*)(&size), sizeof(msg_len_));
            offset += sizeof(msg_len_);

            uint64_t high_value = ARK_HTONLL(uid_.nHigh);
            memcpy(buffer + offset, (void*)(&high_value), sizeof(high_value));
            offset += sizeof(high_value);

            uint64_t low_value = ARK_HTONLL(uid_.nLow);
            memcpy(buffer + offset, (void*)(&low_value), sizeof(low_value));
            offset += sizeof(low_value);

            if (offset != ARK_CS_HEADER_LENGTH)
            {
                assert(0);
            }

            return offset;
        }

        // Message Head[ MsgID(2) | MsgSize(4) | PlayerID(16) ]
        int DeCode(const char* buffer) override
        {
            uint32_t offset(0);

            uint16_t msg_id(0);
            memcpy(&msg_id, buffer + offset, sizeof(msg_id_));
            msg_id_ = ARK_NTOHS(msg_id);
            offset += sizeof(msg_id_);

            uint32_t pack_size(0);
            memcpy(&pack_size, buffer + offset, sizeof(msg_len_));
            msg_len_ = ARK_NTOHL(pack_size) - ARK_CS_HEADER_LENGTH;
            offset += sizeof(msg_len_);


            uint64_t high_value(0);
            memcpy(&high_value, buffer + offset, sizeof(high_value));
            uid_.nHigh = ARK_NTOHLL(high_value);
            offset += sizeof(high_value);

            uint64_t low_value(0);
            memcpy(&low_value, buffer + offset, sizeof(low_value));
            uid_.nLow = ARK_NTOHLL(low_value);
            offset += sizeof(low_value);

            if (offset != ARK_CS_HEADER_LENGTH)
            {
                assert(0);
            }

            return offset;
        }

        uint16_t GetMsgID() const override
        {
            return msg_id_;
        }

        void SetMsgID(uint16_t nMsgID) override
        {
            msg_id_ = nMsgID;
        }

        uint32_t GetBodyLength() const override
        {
            return msg_len_;
        }

        void SetBodyLength(size_t nLength) override
        {
            msg_len_ = (uint32_t)nLength;
        }

        const AFGUID& GetUID() const override
        {
            return uid_;
        }

        void SetUID(const AFGUID& uid) override
        {
            uid_ = uid;
        }

    private:
        uint32_t msg_len_{ 0 };
        uint16_t msg_id_{ 0 };
        AFGUID uid_{ 0 }; //进游戏前是conn_id, 进入游戏后是actor_rid
        //TODO:打包拆小包时候用到 child count
    };

    //TODO:
    class ARK_PKG_SS_HEAD : public ARK_PKG_BASE_HEAD
    {
    public:
    protected:
    private:
        uint32_t msg_len_{ 0 };
        uint16_t msg_id_{ 0 };
        AFGUID actor_id_{ 0 };
        int32_t bus_src_{ 0 };
        int32_t bus_dst_{ 0 };
        uint8_t child_pkg_count_{ 0 };
    };

    enum NetEventType
    {
        NONE = 0,
        CONNECTED = 1,
        DISCONNECTED = 2,
        RECVDATA = 3,
    };

    typedef std::function<void(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len, const AFGUID& conn_id)> NET_PKG_RECV_FUNCTOR;
    typedef std::shared_ptr<NET_PKG_RECV_FUNCTOR> NET_PKG_RECV_FUNCTOR_PTR;

    typedef std::function<void(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id)> NET_EVENT_FUNCTOR;
    typedef std::shared_ptr<NET_EVENT_FUNCTOR> NET_EVENT_FUNCTOR_PTR;

}

#pragma pack(pop)