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
#include <brynet/net/EventLoop.h>
#include <brynet/net/TCPService.h>
#include <brynet/net/ListenThread.h>
#include <brynet/net/Socket.h>
#include <brynet/net/http/HttpService.h>
#include <brynet/net/http/HttpFormat.h>

#pragma pack(push, 1)

namespace ark
{

    class AFCWebSocktServer : public AFINet
    {
    public:
        using AFHttpEntityPtr = AFHttpEntity*;

        AFCWebSocktServer();

        template<typename BaseType>
        AFCWebSocktServer(BaseType* pBaseType, void (BaseType::*handleRecieve)(const ARK_PKG_BASE_HEAD&, const int, const char*, const size_t, const AFGUID&), void (BaseType::*handleEvent)(const NetEventType, const AFGUID&, const std::string&, int))
        {
            net_recv_cb_ = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
            net_event_cb_ = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

            brynet::net::base::InitSocket();
            tcp_service_ptr_ = brynet::net::TcpService::Create();
            listen_thread_ptr_ = brynet::net::ListenThread::Create();
        }

        ~AFCWebSocktServer() override;

        void Update() override;

        //Just for pure virtual function
        bool Start(const int dst_busid, const std::string& ip, const int port, bool ip_v6 = false) override
        {
            return false;
        }

        bool Start(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6 = false) override;
        bool Shutdown() override final;
        bool IsServer() override;

        bool SendRawMsg(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id, const AFGUID& actor_rid) override;
        bool SendRawMsgToAllClient(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& actor_rid) override;

        bool CloseNetEntity(const AFGUID& conn_id) override;
        bool Log(int severity, const char* msg) override;

    protected:
        bool SendMsgToAllClient(const char* msg, const size_t msg_len);
        bool SendMsg(const char* msg, const size_t msg_len, const AFGUID& conn_id);
        bool AddNetEntity(const AFGUID& conn_id, AFHttpEntityPtr entity_ptr);
        bool RemoveNetEntity(const AFGUID& conn_id);
        AFHttpEntityPtr GetNetEntity(const AFGUID& conn_id);

        void ProcessMsgLogicThread();
        void ProcessMsgLogicThread(AFHttpEntityPtr entity_ptr);
        bool CloseSocketAll();
        bool DismantleNet(AFHttpEntityPtr entity_ptr);

        int EnCode(const ARK_PKG_CS_HEAD& head, const char* msg, const size_t len, OUT std::string& out_data);
        int DeCode(const char* data, const size_t len, ARK_PKG_CS_HEAD& head);

    private:
        std::map<AFGUID, AFHttpEntityPtr> net_entities_;
        AFCReaderWriterLock rw_lock_;
        int max_connection_{ 0 };
        int thread_num_{ 0 };
        int bus_id_{ 0 };

        NET_PKG_RECV_FUNCTOR net_recv_cb_{ nullptr };
        NET_EVENT_FUNCTOR net_event_cb_{ nullptr };

        brynet::net::TcpService::PTR tcp_service_ptr_{ nullptr };
        brynet::net::ListenThread::PTR listen_thread_ptr_{ nullptr };
        std::atomic<std::uint64_t> next_conn_id_{ 1 };
    };

}

#pragma pack(pop)