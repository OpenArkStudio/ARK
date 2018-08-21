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

#include "AFINet.h"
#include "SDK/Core/AFQueue.hpp"
#include "SDK/Core/AFRWLock.hpp"
#include <brynet/net/SocketLibFunction.h>
#include <brynet/net/EventLoop.h>
#include <brynet/net/WrapTCPService.h>
#include <brynet/net/ListenThread.h>
#include <brynet/net/Socket.h>

#pragma pack(push, 1)

class AFCNetServer : public AFINet
{
public:
    using AFTCPEntityPtr = AFTCPEntity*;

    AFCNetServer()
        : mnMaxConnect(0)
        , mnCpuCount(0)
        , mnServerID(0)
        , mnNextID(1)
    {

        m_pServer = std::make_shared<brynet::net::WrapTcpService>();
        m_plistenThread = brynet::net::ListenThread::Create();
    }

    template<typename BaseType>
    AFCNetServer(BaseType* pBaseType, void (BaseType::*handleRecieve)(const AFIMsgHead& xHead, const int, const char*, const size_t, const AFGUID&), void (BaseType::*handleEvent)(const NetEventType, const AFGUID&, const int))
        : mnMaxConnect(0)
        , mnCpuCount(0)
        , mnServerID(0)
        , mnNextID(1)
    {
        mRecvCB = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        mEventCB = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        SetWorking(false);

        m_pServer = std::make_shared<brynet::net::WrapTcpService>();
        m_plistenThread = brynet::net::ListenThread::Create();
    }

    virtual ~AFCNetServer()
    {
        Final();
    };

    virtual void Update();

    virtual int Start(const unsigned int nMaxClient, const std::string& strAddrPort, const int nServerID, const int nThreadCount);
    virtual bool Final() final;
    virtual bool IsServer()
    {
        return true;
    }

    virtual bool SendMsgWithOutHead(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID);
    virtual bool SendMsgToAllClientWithOutHead(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xPlayerID);

    virtual bool CloseNetEntity(const AFGUID& xClientID);
    virtual bool Log(int severity, const char* msg)
    {
        return true;
    };

    //From Worker Thread
    size_t OnMessageInner(const brynet::net::TCPSession::PTR& session, const char* buffer, size_t len);

    //From ListenThread
    void OnAcceptConnectionInner(brynet::net::TcpSocket::PTR session);
    void OnClientConnectionInner(const brynet::net::TCPSession::PTR& session);
    void OnClientDisConnectionInner(const brynet::net::TCPSession::PTR& session);

private:
    bool SendMsgToAllClient(const char* msg, const size_t nLen);
    bool SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient);
    bool AddNetEntity(const AFGUID& xClientID, AFTCPEntityPtr pEntity);
    bool RemoveNetEntity(const AFGUID& xClientID);
    AFTCPEntityPtr GetNetEntity(const AFGUID& xClientID);

    void ProcessMsgLogicThread();
    void ProcessMsgLogicThread(AFTCPEntityPtr pEntity);
    bool CloseSocketAll();
    bool DismantleNet(AFTCPEntityPtr pEntity);

protected:
    int DeCode(const char* strData, const size_t len, AFCMsgHead& xHead);
    int EnCode(const AFCMsgHead& xHead, const char* strData, const size_t len, std::string& strOutData);

private:
    std::map<AFGUID, AFTCPEntityPtr> mmObject;
    AFCReaderWriterLock mRWLock;
    int mnMaxConnect;
    std::string mstrIPPort;
    int mnCpuCount;
    int mnServerID;

    NET_RECEIVE_FUNCTOR mRecvCB;
    NET_EVENT_FUNCTOR mEventCB;

    brynet::net::WrapTcpService::PTR m_pServer;
    brynet::net::ListenThread::PTR m_plistenThread;
    std::atomic<std::int64_t> mnNextID;
};

#pragma pack(pop)