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

#include "SDK/Core/AFMap.hpp"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFINetServiceManagerModule.h"
#include "SDK/Interface/AFINetServerService.h"

namespace ark
{

    class AFCNetServerService : public AFINetServerService
    {
    public:
        explicit AFCNetServerService(AFIPluginManager* p);
        virtual ~AFCNetServerService();

        bool Start(const int bus_id, const AFEndpoint& ep, const uint8_t thread_count, const uint32_t max_connection) override;
        bool Update() override;

        //bool SendBroadcastMsg(const int msg_id, const std::string& msg, const AFGUID& player_id) override;
        //bool SendBroadcastPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& player_id) override;
        //bool SendPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list = nullptr) override;
        //bool SendMsg(const uint16_t msg_id, const std::string& data, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list = nullptr) override;
        AFINet* GetNet() override;

        bool RegMsgCallback(const int msg_id, const NET_PKG_RECV_FUNCTOR_PTR& cb) override;
        bool RegForwardMsgCallback(const NET_PKG_RECV_FUNCTOR_PTR& cb) override;
        bool RegNetEventCallback(const NET_EVENT_FUNCTOR_PTR& cb) override;

    protected:
        void OnRecvNetPack(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id);
        void OnSocketNetEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id);

        void OnClientRegister(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len, const AFGUID& conn_id);
        void SyncToAllClient(const int bus_id, const AFGUID& conn_id);

    private:
        AFIPluginManager* m_pPluginManager;
        AFINetServiceManagerModule* m_pNetServiceManagerModule;
        AFILogModule* m_pLogModule;
        AFIMsgModule* m_pMsgModule;

        AFINet* m_pNet{ nullptr };

        std::map<int, NET_PKG_RECV_FUNCTOR_PTR> net_msg_callbacks_;
        std::list<NET_EVENT_FUNCTOR_PTR> net_event_callbacks_;
        std::list<NET_PKG_RECV_FUNCTOR_PTR> net_forward_msg_callbacks_;

        AFMapEx<int, AFServerData> reg_clients_;
    };

}