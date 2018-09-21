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

#include "brynet/net/TCPService.h"
#include "brynet/net/http/HttpService.h"
#include "SDK/Core/AFMacros.hpp"
#include "SDK/Core/AFGUID.hpp"
#include "SDK/Core/AFLockFreeQueue.hpp"
#include "SDK/Core/AFBuffer.hpp"
#include "SDK/Interface/AFNetMsg.h"
#include "SDK/Interface/AFNetDefine.h"

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
    AFINet() = default;
    virtual ~AFINet() = default;

    //need to call this function every frame to drive network library
    virtual void Update() = 0;
    virtual bool Start(const int target_busid, const std::string& ip, const int port, bool ip_v6 = false) = 0;
    virtual bool Start(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6 = false) = 0;

    virtual bool Shutdown() = 0;

    //send a message with out msg-head[auto add msg-head in this function]
    virtual bool SendRawMsg(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID) = 0;

    //send a message to all client[need to add msg-head for this message by yourself]
    virtual bool SendMsgToAllClient(const char* msg, const uint32_t nLen, const AFGUID& xPlayerID)
    {
        return false;
    }

    //send a message with out msg-head to all client[auto add msg-head in this function]
    virtual bool SendRawMsgToAllClient(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xPlayerID)
    {
        return false;
    }

    virtual bool CloseNetEntity(const AFGUID& xClientID) = 0;

    virtual bool IsServer() = 0;

    virtual bool Log(int severity, const char* msg) = 0;

    bool IsStop()
    {
        return !bWorking;
    }

    virtual bool StopAfter(double dTime)
    {
        return false;
    }

    void SetWorking(bool value)
    {
        bWorking = value;
    }

private:
    bool bWorking{ false };

public:
    size_t nRecvSize{ 0 };
    size_t nSendSize{ 0 };
};

//////////////////////////////////////////////////////////////////////////

template <typename SessionPTR>
class AFNetMsg
{
public:
    AFNetMsg(const SessionPTR session_ptr) : mxSession(session_ptr) {}

    NetEventType nType{ NONE };
    AFGUID xClientID{ 0 };
    SessionPTR mxSession{nullptr};
    std::string strMsg{};
    ARK_PKG_CS_HEAD xHead;
};

using AFTCPMsg = AFNetMsg<brynet::net::DataSocket::PTR>;
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

    AFGUID xHttpClientID;
    AFLockFreeQueue<AFNetMsg<SessionPTR>*> mxNetMsgMQ;

private:
    const SessionPTR mxSession;
};

using AFTCPEntity = AFNetEntity<brynet::net::DataSocket::PTR>;
using AFHttpEntity = AFNetEntity<brynet::net::HttpSession::PTR>;