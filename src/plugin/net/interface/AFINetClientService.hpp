/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "base/AFMap.hpp"
#include "base/AFCConsistentHash.hpp"
#include "base/AFNoncopyable.hpp"
#include "base/AFSocketFunc.hpp"
#include "proto/AFProtoCPP.hpp"
#include "net/interface/AFINet.hpp"

namespace ark {

class AFConnectionData
{
public:
    enum ConnectState
    {
        DISCONNECT,
        CONNECTING,
        CONNECTED,
        RECONNECT,
    };

    AFHeadLength head_len_{AFHeadLength::SS_HEAD_LENGTH};
    int server_bus_id_{0};
    AFEndpoint endpoint_;
    std::shared_ptr<AFINet> net_client_{nullptr};

    ConnectState net_state_{DISCONNECT}; // net state
    int64_t last_active_time_{0};
};

class AFINetClientService : public AFNoncopyable
{
public:
    virtual ~AFINetClientService() = default;

    template<typename BaseType>
    bool RegMsgCallback(const int msg_id, BaseType* pBase, void (BaseType::*handleRecv)(const AFNetMsg*))
    {
        NET_MSG_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1);
        return RegMsgCallback(msg_id, std::move(functor));
    }

    template<typename BaseType>
    bool RegForwardMsgCallback(BaseType* pBase, void (BaseType::*handleRecv)(const AFNetMsg*, const int64_t))
    {
        NET_MSG_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2);
        return RegForwardMsgCallback(std::move(functor));
    }

    template<typename BaseType>
    bool RegNetEventCallback(BaseType* pBase, void (BaseType::*handler)(const AFNetEvent*))
    {
        NET_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1);
        return RegNetEventCallback(std::move(functor));
    }

    virtual bool StartClient(const AFHeadLength head_len, const int& target_bus_id, const AFEndpoint& endpoint) = 0;
    virtual void Update() = 0;
    //virtual void Shutdown() = 0;

    virtual std::shared_ptr<AFConnectionData> GetConnectionInfo(const int bus_id) = 0;
    //virtual AFMapEx<int, AFConnectionData>& GetServerList() = 0;

    virtual bool RegMsgCallback(const int nMsgID, const NET_MSG_FUNCTOR&& cb) = 0;
    virtual bool RegForwardMsgCallback(const NET_MSG_FUNCTOR&& cb) = 0;
    virtual bool RegNetEventCallback(const NET_EVENT_FUNCTOR&& cb) = 0;

    virtual std::shared_ptr<AFConnectionData> GetSuitableConnect(const std::string& key) = 0;

    virtual void AddAccountBusID(const std::string& account, const int bus_id) = 0;
    virtual void RemoveAccountBusID(const std::string& account) = 0;
    virtual int GetAccountBusID(const std::string& account) = 0;

    virtual void AddActorBusID(const AFGUID& actor, const int bus_id) = 0;
    virtual void RemoveActorBusID(const AFGUID& actor) = 0;
    virtual int GetActorBusID(const AFGUID& actor) = 0;
};

} // namespace ark
