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

#include "Common/AFProtoCPP.hpp"
#include "Common/AFBaseStruct.hpp"
#include "SDK/Interface/AFINet.h"

namespace ark
{

    class AFServerData
    {
    public:
        void Init(const AFGUID& conn_id, const AFMsg::msg_ss_server_report& data)
        {
            conn_id_ = conn_id;
            server_info_ = data;
        }

        AFGUID conn_id_{ 0 };
        AFMsg::msg_ss_server_report server_info_;
    };

    class AFClientConnectionData
    {
    public:
        int32_t logic_state_{ 0 };
        int32_t game_id_{ 0 };
        AFGUID actor_id_{ 0 };
        AFGUID conn_id_{ 0 };
        //AFGUID mnHashIdentID{ 0 };
        std::string account_{};
    };

    class AFINetServerService
    {
    public:
        virtual ~AFINetServerService() = default;

        template<typename BaseType>
        bool RegMsgCallback(const int nMsgID, BaseType* pBase, void (BaseType::*handleRecv)(const ARK_PKG_BASE_HEAD&, const int, const char*, const uint32_t, const AFGUID&))
        {
            NET_PKG_RECV_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
            return RegMsgCallback(nMsgID, std::make_shared<NET_PKG_RECV_FUNCTOR>(functor));
        }

        template<typename BaseType>
        bool RegForwardMsgCallback(BaseType* pBase, void (BaseType::*handleRecv)(const ARK_PKG_BASE_HEAD&, const int, const char*, const uint32_t, const AFGUID&))
        {
            NET_PKG_RECV_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
            return RegForwardMsgCallback(std::make_shared < NET_PKG_RECV_FUNCTOR>(functor));
        }

        template<typename BaseType>
        bool RegNetEventCallback(BaseType* pBase, void (BaseType::*handler)(const NetEventType, const AFGUID&, const std::string&, const int))
        {
            NET_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
            return RegNetEventCallback(std::make_shared<NET_EVENT_FUNCTOR>(functor));
        }

        virtual bool Start(const int bus_id, const AFEndpoint& ep, const uint8_t thread_count, const uint32_t max_connection) = 0;
        virtual bool Update() = 0;

        //virtual bool SendBroadcastMsg(const int nMsgID, const std::string& msg, const AFGUID& player_id) = 0;
        //virtual bool SendBroadcastPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& player_id) = 0;
        //virtual bool SendPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list = nullptr) = 0;
        //virtual bool SendMsg(const uint16_t msg_id, const std::string& data, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list = nullptr) = 0;
        virtual AFINet* GetNet() = 0;

        virtual bool RegMsgCallback(const int nMsgID, const NET_PKG_RECV_FUNCTOR_PTR& cb) = 0;
        virtual bool RegForwardMsgCallback(const NET_PKG_RECV_FUNCTOR_PTR& cb) = 0;
        virtual bool RegNetEventCallback(const NET_EVENT_FUNCTOR_PTR& cb) = 0;
    };

}