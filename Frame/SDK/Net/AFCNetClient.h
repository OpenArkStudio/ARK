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

#include "AFINet.h"
#include "SDK/Core/Base/AFQueue.h"
#include "SDK/Core/Base/AFRWLock.hpp"
#include <evpp/libevent.h>
#include <evpp/event_watcher.h>
#include <evpp/event_loop.h>
#include <evpp/event_loop_thread.h>
#include <evpp/tcp_server.h>
#include <evpp/buffer.h>
#include <evpp/tcp_conn.h>
#include <evpp/tcp_client.h>

#pragma pack(push, 1)

class AFCNetClient : public AFINet
{
public:
    AFCNetClient()
    {
        mnServerID = 0;
        bWorking = false;
        nReceiverSize = 0;
        nSendSize = 0;
    }

    template<typename BaseType>
    AFCNetClient(BaseType* pBaseType, void (BaseType::*handleRecieve)(const AFIMsgHead& xHead, const int, const char*, const uint32_t, const AFGUID&), void (BaseType::*handleEvent)(const NetEventType, const AFGUID&, const int))
    {
        mRecvCB = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        mEventCB = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        mnServerID = 0;
        nReceiverSize = 0;
        nSendSize = 0;
    }

    virtual ~AFCNetClient()
    {
        bool bRet = Final();
    };

public:
    virtual void Update();
    virtual void Initialization(const std::string& strAddrPort, const int nServerID);
    virtual bool Final();
    virtual bool SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID = 0, const AFGUID& xPlayerID = 0);

    virtual bool CloseNetObject(const AFGUID& xClient);

    virtual bool IsServer();
    virtual bool Log(int severity, const char* msg);
    virtual bool StopAfter(double dTime);

public:
    static void OnClientConnection(const evpp::TCPConnPtr& conn, void* pData);
    void OnClientConnectionInner(const evpp::TCPConnPtr& conn);

    static void OnMessage(const evpp::TCPConnPtr& conn,
                          evpp::Buffer* msg, void* pData);
    void OnMessageInner(const evpp::TCPConnPtr& conn,
                        evpp::Buffer* msg);

private:
    bool SendMsg(const char* msg, const uint32_t nLen, const AFGUID& xClient = 0);

    bool DismantleNet(NetObject* pEntity);
    void ProcessMsgLogicThread();
    void ProcessMsgLogicThread(NetObject* pEntity);
    bool CloseSocketAll();

    static void log_cb(int severity, const char* msg);

protected:
    int DeCode(const char* strData, const uint32_t unLen, AFCMsgHead& xHead);
    int EnCode(const AFCMsgHead& xHead, const char* strData, const uint32_t unDataLen, std::string& strOutData);

private:
    std::unique_ptr<evpp::EventLoopThread> m_pThread;
    std::unique_ptr<evpp::TCPClient> m_pClient;
    std::unique_ptr<NetObject> m_pClientObject;
    std::string mstrIPPort;
    int mnServerID;
    NET_RECEIVE_FUNCTOR mRecvCB;
    NET_EVENT_FUNCTOR mEventCB;
    AFCReaderWriterLock mRWLock;
};

#pragma pack(pop)