/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) AFHttpEntity ArkGame authors.
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

#include "SDK/Core/AFMacros.hpp"
#include "SDK/Core/AFGUID.hpp"
#include "SDK/Core/AFLockFreeQueue.hpp"
#include "SDK/Core/AFBuffer.hpp"
#include "brynet/net/WrapTCPService.h"
#include "brynet/net/http/HttpService.h"

#pragma pack(push, 1)

class  AFIMsgHead
{
public:

    enum AF_Head
    {
        ARK_MSG_HEAD_LENGTH = 22,
    };

    virtual ~AFIMsgHead() {}

    virtual int EnCode(char* strData) const = 0;
    virtual int DeCode(const char* strData)  = 0;

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

class AFCMsgHead : public AFIMsgHead
{
public:
    AFCMsgHead(): munSize(0), munMsgID(0), mxPlayerID(0)
    {
    }

    virtual ~AFCMsgHead() {}

    // Message Head[ MsgID(2) | MsgSize(4) | PlayerID(16) ]
    virtual int EnCode(char* strData) const
    {
        uint32_t nOffset(0);

        uint16_t nMsgID = ARK_HTONS(munMsgID);
        memcpy(strData + nOffset, (void*)(&nMsgID), sizeof(munMsgID));
        nOffset += sizeof(munMsgID);

        uint32_t nPackSize = munSize + ARK_MSG_HEAD_LENGTH;
        uint32_t nSize = ARK_HTONL(nPackSize);
        memcpy(strData + nOffset, (void*)(&nSize), sizeof(munSize));
        nOffset += sizeof(munSize);

        uint64_t nHightData = ARK_HTONLL(mxPlayerID.nHigh);
        memcpy(strData + nOffset, (void*)(&nHightData), sizeof(nHightData));
        nOffset += sizeof(nHightData);

        uint64_t nLowData = ARK_HTONLL(mxPlayerID.nLow);
        memcpy(strData + nOffset, (void*)(&nLowData), sizeof(nLowData));
        nOffset += sizeof(nLowData);

        if (nOffset != ARK_MSG_HEAD_LENGTH)
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
        munSize = ARK_NTOHL(nPackSize) - ARK_MSG_HEAD_LENGTH;
        nOffset += sizeof(munSize);


        uint64_t nHighData(0);
        memcpy(&nHighData, strData + nOffset, sizeof(nHighData));
        mxPlayerID.nHigh = ARK_NTOHLL(nHighData);
        nOffset += sizeof(nHighData);

        uint64_t nLowData(0);
        memcpy(&nLowData, strData + nOffset, sizeof(nLowData));
        mxPlayerID.nLow = ARK_NTOHLL(nLowData);
        nOffset += sizeof(nLowData);

        if (nOffset != ARK_MSG_HEAD_LENGTH)
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
    virtual void SetBodyLength(size_t nLength)
    {
        munSize = (uint32_t)nLength;
    }

    virtual const AFGUID& GetPlayerID() const
    {
        return mxPlayerID;
    }

    virtual void SetPlayerID(const AFGUID& xPlayerID)
    {
        mxPlayerID = xPlayerID;
    }

private:
    uint32_t munSize;
    uint16_t munMsgID;
    AFGUID mxPlayerID;
};
enum NetEventType
{
    NONE = 0,
    CONNECTED = 1,
    DISCONNECTED = 2,
    RECIVEDATA = 3,
};

typedef std::function<void(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const size_t nLen, const AFGUID& nClientID)> NET_RECEIVE_FUNCTOR;
typedef std::shared_ptr<NET_RECEIVE_FUNCTOR> NET_RECEIVE_FUNCTOR_PTR;

typedef std::function<void(const NetEventType nEvent, const AFGUID& nClientID, const int nServerID)> NET_EVENT_FUNCTOR;
typedef std::shared_ptr<NET_EVENT_FUNCTOR> NET_EVENT_FUNCTOR_PTR;

//For now, unused
//typedef std::function<void(int severity, const char* msg)> NET_EVENT_LOG_FUNCTOR;
//typedef std::shared_ptr<NET_EVENT_LOG_FUNCTOR> NET_EVENT_LOG_FUNCTOR_PTR;

class AFINet;

class AFBaseNetEntity
{
public:
    AFBaseNetEntity(AFINet* pNet, const AFGUID& xClientID) :
        mnClientID(xClientID),
        m_pNet(pNet),
        bNeedRemove(false)
    {
    }

    virtual ~AFBaseNetEntity()
    {
    }

    int AddBuff(const char* str, size_t nLen)
    {
        mstrBuff.write(str, nLen);
        return (int)mstrBuff.getlength();
    }

    size_t RemoveBuff(size_t nLen)
    {
        if (nLen > mstrBuff.getlength())
        {
            return 0;
        }

        mstrBuff.removedata(nLen);

        return mstrBuff.getlength();
    }

    const char* GetBuff()
    {
        return mstrBuff.getdata();
    }

    size_t GetBuffLen()
    {
        return mstrBuff.getlength();
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

private:
    AFBuffer mstrBuff;
    std::string mstrUserData;
    AFGUID mnClientID;//temporary client id

    AFINet* m_pNet;
    mutable bool bNeedRemove;
};

class AFINet
{
public:
    AFINet() : bWorking(false), nReceiverSize(0), nSendSize(0) {}

    virtual ~AFINet() = default;

    //need to call this function every frame to drive network library
    virtual void Update() = 0;
    virtual void Start(const std::string& strAddrPort, const int nServerID) {/*Just a base class function*/}
    virtual int Start(const unsigned int nMaxClient, const std::string& strAddrPort, const int nServerID, const int nThreadCount)
    {
        return -1;
    }

    virtual bool Final() = 0;

    //send a message with out msg-head[auto add msg-head in this function]
    virtual bool SendMsgWithOutHead(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID) = 0;

    //send a message to all client[need to add msg-head for this message by yourself]
    virtual bool SendMsgToAllClient(const char* msg, const uint32_t nLen, const AFGUID& xPlayerID)
    {
        return false;
    }

    //send a message with out msg-head to all client[auto add msg-head in this function]
    virtual bool SendMsgToAllClientWithOutHead(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xPlayerID)
    {
        return false;
    }

    virtual bool CloseNetEntity(const AFGUID& xClientID) = 0;

    virtual bool IsServer() = 0;

    virtual bool Log(int severity, const char* msg) = 0;

    bool IsStop()
    {
        return  !bWorking;
    }

    virtual bool StopAfter(double dTime)
    {
        return false;
    }

    bool SplitHostPort(const std::string& strIpPort, std::string& host, int& port)
    {
        std::string a = strIpPort;

        if (a.empty())
        {
            return false;
        }

        size_t index = a.rfind(':');

        if (index == std::string::npos)
        {
            return false;
        }

        if (index == a.size() - 1)
        {
            return false;
        }

        port = std::atoi(&a[index + 1]);

        host = std::string(strIpPort, 0, index);

        if (host[0] == '[')
        {
            if (*host.rbegin() != ']')
            {
                return false;
            }

            // trim the leading '[' and trail ']'
            host = std::string(host.data() + 1, host.size() - 2);
        }

        // Compatible with "fe80::886a:49f3:20f3:add2]:80"
        if (*host.rbegin() == ']')
        {
            // trim the trail ']'
            host = std::string(host.data(), host.size() - 1);
        }

        return true;
    }

    void SetWorking(bool value)
    {
        bWorking = value;
    }

private:
    bool bWorking;

public:
    size_t nReceiverSize;
    size_t nSendSize;
};

//////////////////////////////////////////////////////////////////////////

template <typename SessionPTR>
class AFNetMsg
{
public:
    AFNetMsg(const SessionPTR session_ptr) : nType(NONE), mxSession(session_ptr) {}

    NetEventType nType;
    AFGUID xClientID = {};
    SessionPTR mxSession;
    std::string strMsg = {};
    AFCMsgHead xHead;
};

using AFTCPMsg = AFNetMsg<brynet::net::TCPSession::PTR>;
using AFHttpMsg = AFNetMsg<brynet::net::HttpSession::PTR>;

template <typename SessionPTR>
class AFNetEntity : public AFBaseNetEntity
{
public:
    AFNetEntity(AFINet* pNet, const AFGUID& xClientID, const SessionPTR session) : AFBaseNetEntity(pNet, xClientID), mxSession(session)
    {
    }

    virtual ~AFNetEntity()
    {
    }

    const SessionPTR& GetSession()
    {
        return mxSession;
    }

    SessionPTR mBryNetHttpConnPtr;
    AFGUID xHttpClientID;

    AFLockFreeQueue<AFNetMsg<SessionPTR>*> mxNetMsgMQ;
private:
    const SessionPTR mxSession;
};

using AFTCPEntity = AFNetEntity<brynet::net::TCPSession::PTR>;
using AFHttpEntity = AFNetEntity<brynet::net::HttpSession::PTR>;

#pragma pack(pop)