/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "proto/AFProtoCPP.hpp"
#include "base/AFBus.hpp"
#include "AFINet.hpp"

namespace ark {

//class AFServerData
//{
//public:
//    void Init(const guid_t& conn_id, const AFMsg::msg_ss_server_report& data)
//    {
//        conn_id_ = conn_id;
//        server_info_ = data;
//    }
//
//    guid_t conn_id_{0};
//    AFMsg::msg_ss_server_report server_info_;
//};
//
//class AFClientConnectionData
//{
//public:
//    int32_t logic_state_{0};
//    int32_t game_id_{0};
//    guid_t actor_id_{0};
//    guid_t conn_id_{0};
//    std::string account_{};
//};

class AFINetServerService
{
public:
    virtual ~AFINetServerService() = default;

    template<typename BaseType>
    bool RegMsgCallback(
        const msg_id_t msg_id, BaseType* pBase, void (BaseType::*handleRecv)(AFNetMsg const*, conv_id_t))
    {
        NET_MSG_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2);
        return RegMsgCallback(msg_id, std::move(functor));
    }

    template<typename BaseType>
    bool RegForwardMsgCallback(BaseType* pBase, void (BaseType::*handleRecv)(AFNetMsg const*, int))
    {
        NET_MSG_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2);
        return RegForwardMsgCallback(std::move(functor));
    }

    template<typename BaseType>
    bool RegNetEventCallback(BaseType* pBase, void (BaseType::*handler)(AFNetEvent const*))
    {
        NET_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1);
        return RegNetEventCallback(std::move(functor));
    }

    template<typename BaseType>
    bool RegRegServerCallBack(BaseType* pBase, void (BaseType::*handler)(AFNetMsg const*, conv_id_t))
    {
        NET_MSG_SESSION_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2);
        return RegRegServerCallBack(std::move(functor));
    }

    virtual bool Start(
        std::shared_ptr<const AFIMsgHeader> head, bus_id_t bus_id, const AFEndpoint& ep, uint8_t thread_count, uint32_t max_connection, const size_t silent_timeout) = 0;
    virtual bool Update() = 0;

    // virtual bool SendBroadcastMsg(msg_id_t nMsgID, const std::string& msg, const guid_t& player_id) = 0;
    // virtual bool SendBroadcastPBMsg(msg_id_t msg_id, const google::protobuf::Message& pb_msg, const guid_t&
    // player_id) = 0; virtual bool SendPBMsg(msg_id_t msg_id, const google::protobuf::Message& pb_msg, const
    // guid_t& connect_id, const guid_t& player_id, const std::vector<guid_t>* target_list = nullptr) = 0; virtual bool
    // SendMsg(msg_id_t msg_id, const std::string& data, const guid_t& connect_id, const guid_t& player_id, const
    // std::vector<guid_t>* target_list = nullptr) = 0;
    virtual std::shared_ptr<AFINet> GetNet() = 0;

    virtual bool RegMsgCallback(msg_id_t msg_id, NET_MSG_FUNCTOR&& cb) = 0;
    virtual bool RegForwardMsgCallback(NET_MSG_FUNCTOR&& cb) = 0;
    virtual bool RegNetEventCallback(NET_EVENT_FUNCTOR&& cb) = 0;
    virtual bool RegRegServerCallBack(NET_MSG_SESSION_FUNCTOR&& cb) = 0;

    virtual void SetTimeoutInterval(const int32_t interval) = 0;
};

} // namespace ark
