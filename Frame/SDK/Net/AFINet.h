/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "SDK/Base/AFGUID.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <event2/event_compat.h>
#include "evpp/tcp_callbacks.h"
#include "evpp/buffer.h"
#include "SDK/Base/AFLockFreeQueue.h"

#pragma pack(push, 1)

struct  AFIMsgHead
{
    enum AF_Head
    {
        NF_HEAD_LENGTH = 22,
    };

    virtual int EnCode(char* strData) const = 0;
    virtual int DeCode(const char* strData)  = 0;

    virtual uint16_t GetMsgID() const = 0;
    virtual void SetMsgID(uint16_t nMsgID) = 0;

    virtual uint32_t GetBodyLength() const = 0;
    virtual void SetBodyLength(uint32_t nLength) = 0;

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

class AFCMsgHead : public AFIMsgHead
{
public:
    AFCMsgHead(): munSize(0), munMsgID(0), mxPlayerID(0)
    {
    }

    // Message Head[ MsgID(2) | MsgSize(4) | PlayerID(16) ]
    virtual int EnCode(char* strData) const
    {
        uint32_t nOffset(0);

        uint16_t nMsgID = ARK_HTONS(munMsgID);
        memcpy(strData + nOffset, (void*)(&nMsgID), sizeof(munMsgID));
        nOffset += sizeof(munMsgID);

        uint32_t nPackSize = munSize + NF_HEAD_LENGTH;
        uint32_t nSize = ARK_HTONL(nPackSize);
        memcpy(strData + nOffset, (void*)(&nSize), sizeof(munSize));
        nOffset += sizeof(munSize);

        uint64_t nHightData = ARK_HTONLL(mxPlayerID.nHigh);
        memcpy(strData + nOffset, (void*)(&nHightData), sizeof(nHightData));
        nOffset += sizeof(nHightData);

        uint64_t nLowData = ARK_HTONLL(mxPlayerID.nLow);
        memcpy(strData + nOffset, (void*)(&nLowData), sizeof(nLowData));
        nOffset += sizeof(nLowData);

        if(nOffset != NF_HEAD_LENGTH)
        {
            assert(0);
        }

        return nOffset;
    }

    // Message Head[ MsgID(2) | MsgSize(4) | PlayerID(16) ]
    virtual int DeCode(const char* strData)
    {
        uint32_t nOffset(0);

        uint16_t nMsgID(0);
        memcpy(&nMsgID, strData + nOffset, sizeof(munMsgID));
        munMsgID = ARK_NTOHS(nMsgID);
        nOffset += sizeof(munMsgID);

        uint32_t nPackSize(0);
        memcpy(&nPackSize, strData + nOffset, sizeof(munSize));
        munSize = ARK_NTOHL(nPackSize) - NF_HEAD_LENGTH;
        nOffset += sizeof(munSize);


        uint64_t nHighData(0);
        memcpy(&nHighData, strData + nOffset, sizeof(nHighData));
        mxPlayerID.nHigh = ARK_NTOHLL(nHighData);
        nOffset += sizeof(nHighData);

        uint64_t nLowData(0);
        memcpy(&nLowData, strData + nOffset, sizeof(nLowData));
        mxPlayerID.nLow = ARK_NTOHLL(nLowData);
        nOffset += sizeof(nLowData);

        if(nOffset != NF_HEAD_LENGTH)
        {
            assert(0);
        }

        return nOffset;
    }

    virtual uint16_t GetMsgID() const
    {
        return munMsgID;
    }
    virtual void SetMsgID(uint16_t nMsgID)
    {
        munMsgID = nMsgID;
    }

    virtual uint32_t GetBodyLength() const
    {
        return munSize;
    }
    virtual void SetBodyLength(uint32_t nLength)
    {
        munSize = nLength;
    }

    virtual const AFGUID& GetPlayerID() const
    {
        return mxPlayerID;
    }

    virtual void SetPlayerID(const AFGUID& xPlayerID)
    {
        mxPlayerID = xPlayerID;
    }

protected:
    uint32_t munSize;
    uint16_t munMsgID;
    AFGUID mxPlayerID;
};
enum NetEventType
{
    None = 0,
    CONNECTED = 1,
    DISCONNECTED = 2,
    RECIVEDATA = 3,
};

class AFINet;

typedef std::function<void(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nClientID)> NET_RECEIVE_FUNCTOR;
typedef std::shared_ptr<NET_RECEIVE_FUNCTOR> NET_RECEIVE_FUNCTOR_PTR;

typedef std::function<void(const NetEventType nEvent, const AFGUID& nClientID, const int nServerID)> NET_EVENT_FUNCTOR;
typedef std::shared_ptr<NET_EVENT_FUNCTOR> NET_EVENT_FUNCTOR_PTR;

typedef std::function<void(int severity, const char* msg)> NET_EVENT_LOG_FUNCTOR;
typedef std::shared_ptr<NET_EVENT_LOG_FUNCTOR> NET_EVENT_LOG_FUNCTOR_PTR;

struct MsgFromNetInfo;

class NetObject
{
public:
    NetObject(AFINet* pNet, const AFGUID& xClientID, const evpp::TCPConnPtr& conn): mConnPtr(conn)
    {
        bNeedRemove = false;
        m_pNet = pNet;
        mnClientID = xClientID;
        memset(&sin, 0, sizeof(sin));
    }

    virtual ~NetObject()
    {
    }

    int AddBuff(const char* str, uint32_t nLen)
    {
        mstrBuff.Write(str, nLen);
        return (int)mstrBuff.length();
    }

    int RemoveBuff(uint32_t nLen)
    {
        if(nLen > mstrBuff.length())
        {
            return 0;
        }

        mstrBuff.Next(nLen);

        return mstrBuff.length();
    }

    const char* GetBuff()
    {
        return mstrBuff.data();
    }

    int GetBuffLen() const
    {
        return mstrBuff.length();
    }

    AFINet* GetNet()
    {
        return m_pNet;
    }
    bool NeedRemove()
    {
        return bNeedRemove;
    }
    void SetNeedRemove(bool b)
    {
        bNeedRemove = b;
    }
    const std::string& GetAccount() const
    {
        return mstrUserData;
    }

    void SetAccount(const std::string& strData)
    {
        mstrUserData = strData;
    }

    const AFGUID& GetClientID()
    {
        return mnClientID;
    }

    void SetClientID(const AFGUID& xClientID)
    {
        mnClientID = xClientID;
    }
    const evpp::TCPConnPtr& GetConnPtr()
    {
        return mConnPtr;
    }
public:
    AFLockFreeQueue<MsgFromNetInfo*> mqMsgFromNet;
    evpp::Buffer mstrNetBuff;

private:
    sockaddr_in sin;
    const evpp::TCPConnPtr mConnPtr;
    evpp::Buffer mstrBuff;
    std::string mstrUserData;
    AFGUID mnClientID;//temporary client id

    AFINet* m_pNet;
    mutable bool bNeedRemove;

};

struct MsgFromNetInfo
{
    MsgFromNetInfo(const evpp::TCPConnPtr TCPConPtr) : mTCPConPtr(TCPConPtr)
    {
        nType = None;
    }

    NetEventType nType;
    AFGUID xClientID;
    evpp::TCPConnPtr mTCPConPtr;
    std::string strMsg;
    AFCMsgHead xHead;
};

class AFINet
{
public:
    //need to call this function every frame to drive network library
    virtual bool Execute() = 0;

    virtual void Initialization(const std::string& strAddrPort, const int nServerID) {};
    virtual int Initialization(const unsigned int nMaxClient, const std::string& strAddrPort, const int nServerID, const int nCpuCount)
    {
        return -1;
    };

    virtual bool Final() = 0;

    //send a message with out msg-head[auto add msg-head in this function]
    virtual bool SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID) = 0;

    //send a message to all client[need to add msg-head for this message by yourself]
    virtual bool SendMsgToAllClient(const char* msg, const uint32_t nLen, const AFGUID& xPlayerID)
    {
        return false;
    }

    //send a message with out msg-head to all client[auto add msg-head in this function]
    virtual bool SendMsgToAllClientWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xPlayerID)
    {
        return false;
    }

    virtual bool CloseNetObject(const AFGUID& xClientID) = 0;

    virtual bool IsServer() = 0;

    virtual bool Log(int severity, const char* msg) = 0;
    bool IsStop()
    {
        return  !bWorking;
    };
    virtual bool StopAfter(double dTime)
    {
        return false;
    };

protected:
    bool bWorking;

public:
    int nReceiverSize;
    int nSendSize;
};

#pragma pack(pop)


