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

#include "base/container/AFMap.hpp"
#include "base/AFPluginManager.hpp"
#include "bus/interface/AFIMsgModule.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "net/interface/AFINetServerService.hpp"

namespace ark {

class AFConnection
{
public:
    enum class eConnectState : uint8_t
    {
        DISCONNECT = 0,
        CONNECTING,
        CONNECTED,
        RECONNECT,
    };

    AFConnection() = default;
    AFConnection(bus_id_t bus_id, conv_id_t session_id, int64_t last_active_time, eConnectState connect_state)
        : bus_id_(bus_id)
        , session_id_(session_id)
        , last_active_time_(last_active_time)
        , connect_state_(connect_state)
    {
    }

    bus_id_t bus_id_{0};
    conv_id_t session_id_{0};
    int64_t last_active_time_{0};
    eConnectState connect_state_{eConnectState::DISCONNECT};
};

class AFCNetServerService final : public AFINetServerService
{
public:
    explicit AFCNetServerService(AFPluginManager* p);

    bool Start(std::shared_ptr<const AFIMsgHeader> head, bus_id_t bus_id, const AFEndpoint& ep, uint8_t thread_count, uint32_t max_connection, const size_t silent_timeout) override;
    bool Update() override;

    std::shared_ptr<AFINet> GetNet() override;

    bool RegMsgCallback(msg_id_t msg_id, NET_MSG_FUNCTOR&& cb) override;
    bool RegForwardMsgCallback(NET_MSG_FUNCTOR&& cb) override;
    bool RegNetEventCallback(NET_EVENT_FUNCTOR&& cb) override;

    // RegServer msg callback
    bool RegRegServerCallBack(NET_MSG_SESSION_FUNCTOR&& cb) override;

    void SetTimeoutInterval(const int32_t interval) override;

protected:
    void OnNetMsg(const AFNetMsg* msg, conv_id_t session_id);
    void OnNetEvent(const AFNetEvent* event);
    void OnDisconnectNetEvent(const AFNetEvent* event);

    //void ProcessConnection();
    void ProcessClientConnection();

    void AddConnection(bus_id_t bus_id, conv_id_t session_id);
    void UpdateConnection(bus_id_t bus_id);
    void RemoveConnection(bus_id_t bus_id);

    void UpdateClientConnection(conv_id_t session_id);
    inline void RemoveClientConnection(conv_id_t session_id)
    {
        auto iter = client_connection_list_.find(session_id);
        if (iter == client_connection_list_.end())
        {
            return;
        }

        client_connection_list_.erase(iter);
    }

    //void OnClientRegister(const AFNetMsg* msg, conv_id_t session_id);
    //void SyncToAllClient(bus_id_t bus_id, conv_id_t session_id);

private:
    AFPluginManager* m_pPluginManager;
    AFINetServiceManagerModule* m_pNetServiceManagerModule;
    AFIMsgModule* m_pMsgModule;

    std::shared_ptr<AFINet> m_pNet{nullptr};

    NET_MSG_SESSION_FUNCTOR reg_server_call_back_{nullptr};

    std::map<msg_id_t, NET_MSG_FUNCTOR> net_msg_callbacks_;
    std::list<NET_MSG_FUNCTOR> net_forward_msg_callbacks_;
    std::list<NET_EVENT_FUNCTOR> net_event_callbacks_;

    using AFConnectionMap = AFSmartPtrMap<bus_id_t, AFConnection>;
    AFConnectionMap connection_list_;
    //AFMapEx<int, AFServerData> reg_clients_;

    using AFClientConnectionMap = std::map<conv_id_t, AFConnection>;
    AFClientConnectionMap client_connection_list_;
    int32_t client_timeout_interval_{ std::chrono::milliseconds(ARK_NET_HEART_TIME).count() };
};

} // namespace ark
