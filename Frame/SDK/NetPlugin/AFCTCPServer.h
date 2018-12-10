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

#include "SDK/Interface/AFINet.h"
#include "AFNetSession.h"

namespace ark
{

    class AFCTCPServer : public AFINet
    {
    public:
        AFCTCPServer();

        template<typename BaseType>
        AFCTCPServer(BaseType* pBaseType, void (BaseType::*handleRecieve)(AFNetMsg*), void (BaseType::*handleEvent)(const AFNetEvent*))
        {
            net_recv_cb_ = std::bind(handleRecieve, pBaseType, std::placeholders::_1);
            net_event_cb_ = std::bind(handleEvent, pBaseType, std::placeholders::_1);

            brynet::net::base::InitSocket();
            tcp_service_ptr_ = brynet::net::TcpService::Create();
            listen_thread_ptr_ = brynet::net::ListenThread::Create();
        }

        ~AFCTCPServer() override;

        void Update() override;

        //Just for pure virtual function
        bool StartClient(AFHeadLength head_len, const int dst_busid, const std::string& ip, const int port, bool ip_v6 = false) override
        {
            return false;
        }

        bool StartServer(AFHeadLength head_length, const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6 = false) override;
        bool Shutdown() override final;
        bool IsServer() override;

        bool SendRawMsg(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id, const AFGUID& actor_id) override;
        bool SendRawMsgToAllClient(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& actor_id) override;

        bool CloseSession(const int64_t& conn_id) override;

    protected:
        bool SendMsgToAllClient(const char* msg, const size_t msg_len);
        bool SendMsg(const char* msg, const size_t msg_len, const int64_t& session_id);

        bool AddNetSession(AFTCPSessionPtr session);
        AFTCPSessionPtr GetNetSession(const int64_t& conn_id);
        bool CloseSession(AFTCPSessionPtr& session);

        void UpdateNetSession();
        void UpdateNetEvent(AFTCPSessionPtr session);
        void UpdateNetMsg(AFTCPSessionPtr session);

        bool CloseAllSession();

    private:
        std::map<int64_t, AFTCPSessionPtr> sessions_;
        AFCReaderWriterLock rw_lock_;
        int max_connection_{ 0 };
        //int thread_num_{ 0 };
        int bus_id_{ 0 };

        NET_PKG_RECV_FUNCTOR net_recv_cb_;
        NET_EVENT_FUNCTOR net_event_cb_;

        brynet::net::TcpService::PTR tcp_service_ptr_{ nullptr };
        brynet::net::ListenThread::PTR listen_thread_ptr_{ nullptr };
        std::atomic<std::int64_t> trusted_session_id_{ 1 };
    };

}