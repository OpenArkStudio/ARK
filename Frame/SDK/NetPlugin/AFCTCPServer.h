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

#include "SDK/Interface/AFINet.h"
#include "SDK/Core/AFQueue.hpp"
#include "SDK/Core/AFRWLock.hpp"
#include <brynet/net/SocketLibFunction.h>
#include <brynet/net/TCPService.h>
#include <brynet/net/ListenThread.h>
#include <brynet/net/Socket.h>

#pragma pack(push, 1)

namespace ark
{

    class AFCTCPServer : public AFINet
    {
    public:
        using AFTCPEntityPtr = AFTCPEntity*;

        AFCTCPServer();

        template<typename BaseType>
        AFCTCPServer(BaseType* pBaseType, void (BaseType::*handleRecieve)(const ARK_PKG_BASE_HEAD& xHead, const int, const char*, const size_t, const AFGUID&), void (BaseType::*handleEvent)(const NetEventType, const AFGUID&, const int))
        {
            mRecvCB = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
            mEventCB = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            SetWorking(false);

            brynet::net::base::InitSocket();

            m_pTcpService = brynet::net::TcpService::Create();
            m_pListenThread = brynet::net::ListenThread::Create();
        }

        ~AFCTCPServer() override;

        void Update() override;

        //Just for pure virtual function
        bool Start(const int target_busid, const std::string& ip, const int port, bool ip_v6 = false) override
        {
            return false;
        }

        bool Start(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6 = false) override;
        bool Shutdown() override final;
        bool IsServer() override;

        bool SendRawMsg(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID) override;
        bool SendRawMsgToAllClient(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xPlayerID) override;

        bool CloseNetEntity(const AFGUID& xClientID) override;
        bool Log(int severity, const char* msg) override;

    protected:
        bool SendMsgToAllClient(const char* msg, const size_t nLen);
        bool SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient);
        bool AddNetEntity(const AFGUID& xClientID, AFTCPEntityPtr pEntity);
        bool RemoveNetEntity(const AFGUID& xClientID);
        AFTCPEntityPtr GetNetEntity(const AFGUID& xClientID);

        void ProcessMsgLogicThread();
        void ProcessMsgLogicThread(AFTCPEntityPtr pEntity);
        bool CloseSocketAll();
        bool DismantleNet(AFTCPEntityPtr pEntity);

        int DeCode(const char* strData, const size_t len, ARK_PKG_CS_HEAD& xHead);
        int EnCode(const ARK_PKG_CS_HEAD& xHead, const char* strData, const size_t len, std::string& strOutData);

    private:
        std::map<AFGUID, AFTCPEntityPtr> mmObject;
        AFCReaderWriterLock mRWLock;
        int mnMaxConnect{ 0 };
        int mnThreadNum{ 0 };
        int mnServerID{ 0 };

        NET_PKG_RECV_FUNCTOR mRecvCB;
        NET_EVENT_FUNCTOR mEventCB;

        brynet::net::TcpService::PTR m_pTcpService{ nullptr };
        brynet::net::ListenThread::PTR m_pListenThread{ nullptr };
        std::atomic<std::int64_t> mnNextID{ 1 };
    };

}

#pragma pack(pop)