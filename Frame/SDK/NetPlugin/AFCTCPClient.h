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

    class AFCTCPClient : public AFINet
    {
    public:
        AFCTCPClient(const brynet::net::TcpService::PTR& service = nullptr, const brynet::net::AsyncConnector::PTR& connector = nullptr);

        template<typename BaseType>
        AFCTCPClient(BaseType* pBaseType, void (BaseType::*handleRecv)(const AFNetMsg*, const int64_t), void (BaseType::*handleEvent)(const AFNetEvent*))
        {
            net_msg_cb_ = std::bind(handleRecv, pBaseType, std::placeholders::_1, std::placeholders::_2);
            net_event_cb_ = std::bind(handleEvent, pBaseType, std::placeholders::_1);

            brynet::net::base::InitSocket();
            tcp_service_ptr_ = brynet::net::TcpService::Create();
            //虽然SyncConnectSocket可以自动创建connector,但是当多个client同时共用connector的时候，就用的上了
            connector_ptr_ = brynet::net::AsyncConnector::Create();
        }

        ~AFCTCPClient() override;

        void Update() override;

        bool StartClient(AFHeadLength head_len, const int dst_busid, const std::string& ip, const int port, bool ip_v6 = false) override;

        bool Shutdown() override final;
        bool SendRawMsg(const uint16_t msg_id, const char* msg_data, const size_t msg_len, const AFGUID& session_id = 0, const AFGUID& actor_id = 0) override;

        bool CloseSession(const AFGUID& session_id) override;

    protected:
        bool SendMsg(const char* msg, const size_t msg_len, const AFGUID& session_id = 0);

        void UpdateNetSession();
        void UpdateNetEvent(AFTCPSessionPtr session);
        void UpdateNetMsg(AFTCPSessionPtr session);

        bool CloseAllSession();

    private:
        std::unique_ptr<AFTCPSession> client_session_ptr_{ nullptr };
        int dst_bus_id_{ 0 };
        uint64_t trust_session_id_{ 1 };

        NET_MSG_FUNCTOR net_msg_cb_;
        NET_EVENT_FUNCTOR net_event_cb_;
        AFCReaderWriterLock rw_lock_;

        brynet::net::TcpService::PTR tcp_service_ptr_{ nullptr };
        brynet::net::AsyncConnector::PTR connector_ptr_{ nullptr };
    };

}