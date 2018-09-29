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

        virtual const AFGUID& GetPlayerID() const = 0;
        virtual void SetPlayerID(const AFGUID& xPlayerID) = 0;

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

            uint16_t nMsgID = ARK_HTONS(munMsgID);
            memcpy(buffer + offset, (void*)(&nMsgID), sizeof(munMsgID));
            offset += sizeof(munMsgID);

            uint32_t nPackSize = munSize + ARK_CS_HEADER_LENGTH;
            uint32_t nSize = ARK_HTONL(nPackSize);
            memcpy(buffer + offset, (void*)(&nSize), sizeof(munSize));
            offset += sizeof(munSize);

            uint64_t nHightData = ARK_HTONLL(mxPlayerID.nHigh);
            memcpy(buffer + offset, (void*)(&nHightData), sizeof(nHightData));
            offset += sizeof(nHightData);

            uint64_t nLowData = ARK_HTONLL(mxPlayerID.nLow);
            memcpy(buffer + offset, (void*)(&nLowData), sizeof(nLowData));
            offset += sizeof(nLowData);

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

            uint16_t nMsgID(0);
            memcpy(&nMsgID, buffer + offset, sizeof(munMsgID));
            munMsgID = ARK_NTOHS(nMsgID);
            offset += sizeof(munMsgID);

            uint32_t nPackSize(0);
            memcpy(&nPackSize, buffer + offset, sizeof(munSize));
            munSize = ARK_NTOHL(nPackSize) - ARK_CS_HEADER_LENGTH;
            offset += sizeof(munSize);


            uint64_t nHighData(0);
            memcpy(&nHighData, buffer + offset, sizeof(nHighData));
            mxPlayerID.nHigh = ARK_NTOHLL(nHighData);
            offset += sizeof(nHighData);

            uint64_t nLowData(0);
            memcpy(&nLowData, buffer + offset, sizeof(nLowData));
            mxPlayerID.nLow = ARK_NTOHLL(nLowData);
            offset += sizeof(nLowData);

            if (offset != ARK_CS_HEADER_LENGTH)
            {
                assert(0);
            }

            return offset;
        }

        uint16_t GetMsgID() const override
        {
            return munMsgID;
        }

        void SetMsgID(uint16_t nMsgID) override
        {
            munMsgID = nMsgID;
        }

        uint32_t GetBodyLength() const override
        {
            return munSize;
        }

        void SetBodyLength(size_t nLength) override
        {
            munSize = (uint32_t)nLength;
        }

        const AFGUID& GetPlayerID() const override
        {
            return mxPlayerID;
        }

        void SetPlayerID(const AFGUID& xPlayerID) override
        {
            mxPlayerID = xPlayerID;
        }

    private:
        uint32_t munSize{ 0 };
        uint16_t munMsgID{ 0 };
        AFGUID mxPlayerID{ 0 };
        //TODO:打包拆小包时候用到 child count
    };

    //TODO:
    class ARK_PKG_SS_HEAD : public ARK_PKG_BASE_HEAD
    {
    public:
    protected:
    private:
        uint32_t _msg_size{ 0 };
        uint16_t _msg_id{ 0 };
        AFGUID _player_id{ 0 };
        int32_t _bus_from{ 0 };
        int32_t _bus_to{ 0 };
        uint8_t _child_pkg_count{ 0 };
    };

    enum NetEventType
    {
        NONE = 0,
        CONNECTED = 1,
        DISCONNECTED = 2,
        RECVDATA = 3,
    };

    typedef std::function<void(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nClientID)> NET_PKG_RECV_FUNCTOR;
    typedef std::shared_ptr<NET_PKG_RECV_FUNCTOR> NET_PKG_RECV_FUNCTOR_PTR;

    typedef std::function<void(const NetEventType nEvent, const AFGUID& nClientID, const int nServerID)> NET_EVENT_FUNCTOR;
    typedef std::shared_ptr<NET_EVENT_FUNCTOR> NET_EVENT_FUNCTOR_PTR;

}

#pragma pack(pop)