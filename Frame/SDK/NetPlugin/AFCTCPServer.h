/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include <brynet/net/SocketLibFunction.h>
#include <brynet/net/TCPService.h>
#include <brynet/net/ListenThread.h>
#include <brynet/net/Socket.h>
#include "SDK/Interface/AFINet.h"
#include "SDK/Core/AFQueue.hpp"
#include "SDK/Core/AFRWLock.hpp"

#pragma pack(push, 1)

namespace ark
{

    class AFCTCPServer : public AFINet
    {
    public:
        using AFTCPEntityPtr = AFTCPEntity*;

        AFCTCPServer();

        template<typename BaseType>
        AFCTCPServer(AFHeadLength head_length, BaseType* pBaseType, void (BaseType::*handleRecieve)(const AFIMsgHead&, const int, const char*, const size_t, const AFGUID&), void (BaseType::*handleEvent)(const NetEventType, const AFGUID&, const std::string&, const int))
        {
            net_recv_cb_ = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
            net_event_cb_ = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
            SetWorking(false);

            SetHeadLength(head_length);

            brynet::net::base::InitSocket();

            tcp_service_ptr_ = brynet::net::TcpService::Create();
            listen_thread_ptr_ = brynet::net::ListenThread::Create();
        }

        ~AFCTCPServer() override;

        void Update() override;

        //Just for pure virtual function
        bool Start(const int dst_busid, const std::string& ip, const int port, bool ip_v6 = false) override
        {
            return false;
        }

        bool Start(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6 = false) override;
        bool Shutdown() override final;
        bool IsServer() override;

        bool SendRawMsg(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id, const AFGUID& actor_id) override;
        bool SendRawMsgToAllClient(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& actor_id) override;

        bool CloseNetEntity(const int64_t& conn_id) override;
        bool Log(int severity, const char* msg) override;

    protected:
        bool SendMsgToAllClient(const char* msg, const size_t msg_len);
        bool SendMsg(const char* msg, const size_t msg_len, const int64_t& conn_id);
        bool AddNetEntity(const int64_t& conn_id, AFTCPEntityPtr entity_ptr);
        bool RemoveNetEntity(const int64_t& conn_id);
        AFTCPEntityPtr GetNetEntity(const int64_t& conn_id);

        void ProcessMsgLogicThread();
        void ProcessMsgLogicThread(AFTCPEntityPtr entity_ptr);
        bool CloseSocketAll();
        bool unwrap_msg(AFTCPEntityPtr entity_ptr);

    private:
        std::map<int64_t, AFTCPEntityPtr> net_entities_;
        AFCReaderWriterLock rw_lock_;
        int max_connection_{ 0 };
        int thread_num_{ 0 };
        int bus_id_{ 0 };

        NET_PKG_RECV_FUNCTOR net_recv_cb_;
        NET_EVENT_FUNCTOR net_event_cb_;

        brynet::net::TcpService::PTR tcp_service_ptr_{ nullptr };
        brynet::net::ListenThread::PTR listen_thread_ptr_{ nullptr };
        std::atomic<std::int64_t> next_conn_id_{ 1 };
    };

}

#pragma pack(pop)