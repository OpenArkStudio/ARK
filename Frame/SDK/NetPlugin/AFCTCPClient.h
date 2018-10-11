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
#include <brynet/net/Connector.h>
#include <brynet/net/SyncConnector.h>

#pragma pack(push, 1)

namespace ark
{

    class AFCTCPClient : public AFINet
    {
    public:
        using AFTCPEntityPtr = AFTCPEntity*;

        AFCTCPClient(const brynet::net::TcpService::PTR& service = nullptr, const brynet::net::AsyncConnector::PTR& connector = nullptr);

        template<typename BaseType>
        AFCTCPClient(BaseType* pBaseType, void (BaseType::*handleRecieve)(const ARK_PKG_BASE_HEAD&, const int, const char*, const size_t, const AFGUID&), void (BaseType::*handleEvent)(const NetEventType, const AFGUID&, const std::string&, const int))
        {
            net_recv_cb_ = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
            net_event_cb_ = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

            brynet::net::base::InitSocket();
            tcp_service_ptr_ = brynet::net::TcpService::Create();
            //虽然SyncConnectSocket可以自动创建connector,但是当多个client同时共用connector的时候，就用的上了
            connector_ptr_ = brynet::net::AsyncConnector::Create();
        }

        ~AFCTCPClient() override;

        void Update() override;

        bool Start(const int dst_busid, const std::string& ip, const int port, bool ip_v6 = false) override;
        bool Start(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6 = false) override
        {
            return false;
        }

        bool Shutdown() override final;
        bool SendRawMsg(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id = 0, const AFGUID& actor_rid = 0) override;

        bool CloseNetEntity(const AFGUID& conn_id) override;
        bool IsServer() override;
        bool Log(int severity, const char* msg) override;

    protected:

        bool SendMsg(const char* msg, const size_t msg_len, const AFGUID& conn_id = 0);

        bool DismantleNet(AFTCPEntityPtr entity_ptr);
        void ProcessMsgLogicThread();
        void ProcessMsgLogicThread(AFTCPEntityPtr entity_ptr);
        bool CloseSocketAll();

        int EnCode(const ARK_PKG_CS_HEAD& head, const char* msg, const size_t len, OUT std::string& out_data);
        int DeCode(const char* data, const size_t len, ARK_PKG_CS_HEAD& head);

    private:
        std::unique_ptr<AFTCPEntity> client_entity_ptr_{ nullptr };
        int dst_bus_id_{ 0 };
        std::atomic<uint64_t> conn_id_{ 1 }; //do nothing

        NET_PKG_RECV_FUNCTOR net_recv_cb_;
        NET_EVENT_FUNCTOR net_event_cb_;
        AFCReaderWriterLock rw_lock_;

        brynet::net::TcpService::PTR tcp_service_ptr_{ nullptr };
        brynet::net::AsyncConnector::PTR connector_ptr_{ nullptr };
    };

}

#pragma pack(pop)