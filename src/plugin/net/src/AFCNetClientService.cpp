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

#include "base/time/AFDateTime.hpp"
#include "net/include/AFCTCPClient.hpp"
#include "net/include/AFCRDPClient.hpp"
#include "net/include/AFCNetClientService.hpp"

namespace ark {

AFCNetClientService::AFCNetClientService(AFPluginManager* p)
    : m_pPluginManager(p)
{
    m_pNetServiceManagerModule = m_pPluginManager->FindModule<AFINetServiceManagerModule>();
    m_pBusModule = m_pPluginManager->FindModule<AFIBusModule>();
    m_pMsgModule = m_pPluginManager->FindModule<AFIMsgModule>();

    consistent_hashmap_.set_vnode_count(EConstDefine_DefaultWeight);
}

bool AFCNetClientService::StartClient(std::shared_ptr<const AFIMsgHeader> head, bus_id_t target_bus_id, const AFEndpoint& endpoint)
{
    AFConnectionData data;
    data.head_ = head;
    data.server_bus_id_ = target_bus_id;
    data.endpoint_ = endpoint;

    tmp_connections_.push_back(data);
    return true;
}

void AFCNetClientService::Update()
{
    ProcessAddConnection();
    ProcessUpdate();
}

bool AFCNetClientService::RegMsgCallback(msg_id_t msg_id, const NET_MSG_FUNCTOR&& cb)
{
    if (net_msg_callbacks_.find(msg_id) != net_msg_callbacks_.end())
    {
        return false;
    }
    else
    {
        net_msg_callbacks_.insert(std::make_pair(msg_id, std::forward<const NET_MSG_FUNCTOR>(cb)));
        return true;
    }
}

bool AFCNetClientService::RegForwardMsgCallback(const NET_MSG_FUNCTOR&& cb)
{
    net_msg_forward_callbacks_.push_back(std::forward<const NET_MSG_FUNCTOR>(cb));
    return true;
}

bool AFCNetClientService::RegNetEventCallback(const NET_EVENT_FUNCTOR&& cb)
{
    net_event_callbacks_.push_back(std::forward<const NET_EVENT_FUNCTOR>(cb));
    return true;
}

void AFCNetClientService::ProcessUpdate()
{
    for (auto iter : real_connections_)
    {
        auto connection_data = iter.second;
        if (connection_data->net_client_ == nullptr)
        {
	        continue;
        }
    	
        switch (connection_data->net_state_)
        {
        case AFConnectionData::DISCONNECT:
        {
            connection_data->net_state_ = AFConnectionData::RECONNECT;
            connection_data->net_client_->Shutdown();
        }
        break;
        case AFConnectionData::CONNECTING:
        {
            connection_data->net_client_->Update();
        }
        break;
        case AFConnectionData::CONNECTED:
        {
            connection_data->net_client_->Update();

            KeepAlive(connection_data);
        }
        break;
        case AFConnectionData::RECONNECT:
        {
            // RECONNECT 30s/time
            if ((connection_data->last_active_time_ + 30 * AFTimespan::SECOND_MS) >= m_pPluginManager->GetNowTime())
            {
                break;
            }

            connection_data->last_active_time_ = m_pPluginManager->GetNowTime();

            connection_data->net_client_->Shutdown();

            // based on protocol to create a new client
            bool ret = connection_data->net_client_->StartClient(connection_data->head_,
                connection_data->server_bus_id_, connection_data->endpoint_.GetIP(),
                connection_data->endpoint_.GetPort());
            if (!ret)
            {
                connection_data->net_state_ = AFConnectionData::RECONNECT;
            }
            else
            {
                connection_data->net_state_ = AFConnectionData::CONNECTING;
            }
        }
        break;
        default:
            break;
        }
    }
}

std::shared_ptr<AFINet> AFCNetClientService::CreateNet(const proto_type proto)
{
    if (proto == proto_type::tcp)
    {
        return std::make_shared<AFCTCPClient>(this, &AFCNetClientService::OnNetMsg, &AFCNetClientService::OnNetEvent);
    }
    else if (proto == proto_type::udp)
    {
        return std::make_shared<AFCRDPClient>(this, &AFCNetClientService::OnNetMsg, &AFCNetClientService::OnNetEvent);
    }
    else if (proto == proto_type::ws)
    {
        // will add web-socket client
    }

    // other protocol will be supported
    return nullptr;
}

void AFCNetClientService::LogServerInfo()
{
    LogServerInfo("This is a client, begin to print Server Info----------------------------------");

    for (auto iter : real_connections_)
    {
        auto connection_data = iter.second;
        if (connection_data != nullptr)
        {
            std::string info = ARK_FORMAT("TargetBusID={} State={} url={}", connection_data->server_bus_id_,
                connection_data->net_state_, connection_data->endpoint_.ToString());
            LogServerInfo(info);
        }
    }

    LogServerInfo("This is a client, end to print Server Info----------------------------------");
}

void AFCNetClientService::KeepAlive(std::shared_ptr<AFConnectionData> pServerData)
{
    if ((pServerData->last_active_time_ + 5000) > m_pPluginManager->GetNowTime())
    {
        return;
    }

    SendReport(pServerData);

    pServerData->last_active_time_ = m_pPluginManager->GetNowTime();

    KeepReport(pServerData);
    LogServerInfo();
}

bool AFCNetClientService::AddServerNode(std::shared_ptr<AFConnectionData> data)
{
    consistent_hashmap_.insert(data->server_bus_id_, data->endpoint_.GetIP(), data->endpoint_.GetPort());
    return true;
}

bool AFCNetClientService::GetServerNode(const std::string& hash_key, AFVNode& vnode)
{
    auto iter = consistent_hashmap_.find(hash_key);
    if (iter == consistent_hashmap_.end())
    {
        return false;
    }
    else
    {
        vnode = iter->second;
        return true;
    }
}

void AFCNetClientService::RemoveServerNode(std::shared_ptr<AFConnectionData> data)
{
    AFVNode vnode(data->server_bus_id_, data->endpoint_.GetIP(), data->endpoint_.GetPort(), 0);
    consistent_hashmap_.erase(vnode);
}

int AFCNetClientService::OnConnect(const AFNetEvent* event)
{
    ARK_LOG_INFO("Connected [{}] successfully, ip={} session_id={}", AFBusAddr(event->GetBusId()).ToString(),
        event->GetIP(), event->GetId());

    auto connection_info = GetConnectionInfo(event->GetBusId());

    if (connection_info != nullptr)
    {
        AddServerNode(connection_info);
        connection_info->net_state_ = AFConnectionData::CONNECTED;

        // add server-bus-id -> client-bus-id
        m_pNetServiceManagerModule->AddNetConnectionBus(event->GetBusId(), connection_info->net_client_);

        // send reg message to server
        RegToServer(*(connection_info->head_.get()), event->GetBusId());
    }

    return 0;
}

int AFCNetClientService::OnDisconnect(const AFNetEvent* event)
{
    ARK_LOG_ERROR("Disconnect [{}] successfully, ip={} session_id={}", AFBusAddr(event->GetBusId()).ToString(),
        event->GetIP(), event->GetId());

    std::shared_ptr<AFConnectionData> connection_info = GetConnectionInfo(event->GetBusId());

    if (connection_info != nullptr)
    {
        RemoveServerNode(connection_info);
        connection_info->net_state_ = AFConnectionData::DISCONNECT;
        connection_info->last_active_time_ = m_pPluginManager->GetNowTime();
        // remove net bus
        m_pNetServiceManagerModule->RemoveNetConnectionBus(event->GetBusId());
    }

    return 0;
}

void AFCNetClientService::ProcessAddConnection()
{
    for (auto& iter : tmp_connections_)
    {
        const AFConnectionData& connection_data = iter;
        std::shared_ptr<AFConnectionData> target_connection_data =
            real_connections_.find_value(connection_data.server_bus_id_);
        if (target_connection_data != nullptr)
        {
            continue;
        }
        // add new server
        target_connection_data = std::make_shared<AFConnectionData>();

        *target_connection_data = connection_data;
        target_connection_data->last_active_time_ = m_pPluginManager->GetNowTime();

        // based on protocol to create a new client
        target_connection_data->net_client_ = CreateNet(target_connection_data->endpoint_.proto());
        int ret = target_connection_data->net_client_->StartClient(target_connection_data->head_,
            target_connection_data->server_bus_id_, target_connection_data->endpoint_.GetIP(),
            target_connection_data->endpoint_.GetPort());
        if (!ret)
        {
            target_connection_data->net_state_ = AFConnectionData::RECONNECT;
        }
        else
        {
            target_connection_data->net_state_ = AFConnectionData::CONNECTING;
        }

        real_connections_.insert(target_connection_data->server_bus_id_, target_connection_data);
    }

    tmp_connections_.clear();
}

void AFCNetClientService::OnNetMsg(const AFNetMsg* msg, conv_id_t session_id)
{
    ARK_LOG_DEBUG("[msg][recv] msg id = {} session id = {}", msg->GetMsgId(), session_id);

    auto it = net_msg_callbacks_.find(msg->GetMsgId());

    if (it != net_msg_callbacks_.end())
    {
        (it->second)(msg, session_id);
    }
    else
    {
        ARK_LOG_ERROR("Invalid message, id = {}", msg->GetMsgId());
        // TODO:forward to other server process
    }
}

void AFCNetClientService::OnNetEvent(const AFNetEvent* event)
{
    switch (event->GetType())
    {
    case AFNetEventType::CONNECT:
        OnConnect(event);
        break;
    case AFNetEventType::DISCONNECT:
        OnDisconnect(event);
        break;
    default:
        break;
    }

    for (const auto& it : net_event_callbacks_)
    {
        (it)(event);
    }
}

std::shared_ptr<AFConnectionData> AFCNetClientService::GetConnectionInfo(bus_id_t bus_id)
{
    return real_connections_.find_value(bus_id);
}

std::shared_ptr<AFConnectionData> AFCNetClientService::GetSuitableConnect(const std::string& key)
{
    static AFVNode vnode(0, NULL_STR, 0, 0);
    if (!GetServerNode(key, vnode))
    {
        return nullptr;
    }

    return GetConnectionInfo(vnode.bus_id);
}

void AFCNetClientService::AddAccountBusID(const std::string& account, bus_id_t bus_id)
{
    account_bus_map_.insert(std::make_pair(account, bus_id));
}

void AFCNetClientService::RemoveAccountBusID(const std::string& account)
{
    account_bus_map_.erase(account);
}

int AFCNetClientService::GetAccountBusID(const std::string& account)
{
    auto iter = account_bus_map_.find(account);
    if (iter == account_bus_map_.end())
    {
        return -1;
    }

    return iter->second;
}

void AFCNetClientService::AddActorBusID(const guid_t& actor, bus_id_t bus_id)
{
    actor_bus_map_.insert(std::make_pair(actor, bus_id));
}

void AFCNetClientService::RemoveActorBusID(const guid_t& actor)
{
    actor_bus_map_.erase(actor);
}

int AFCNetClientService::GetActorBusID(const guid_t& actor)
{
    auto iter = actor_bus_map_.find(actor);
    if (iter == actor_bus_map_.end())
    {
        return -1;
    }

    return iter->second;
}

void AFCNetClientService::RegToServer(const AFIMsgHeader& head, bus_id_t target_bus_id)
{
    auto bus_id = m_pBusModule->GetSelfBusID();
    auto pNet = m_pNetServiceManagerModule->GetNetConnectionBus(bus_id, target_bus_id);
    if (!pNet)
    {
        return;
    }

    auto new_head = head.New();
    new_head->SetMessageId(AFMsg::E_SS_MSG_ID_REG_TO_SERVER);
    new_head->SetSourceBusId(bus_id);
    new_head->SetDestBusId(target_bus_id);

    pNet->SendMsg(new_head, "", 0);

    delete new_head;
}

void AFCNetClientService::SendReport(std::shared_ptr<AFConnectionData> pServerData)
{
    auto head = pServerData->head_->New();
    head->SetMessageId(AFMsg::E_SS_MSG_ID_HEART_BEAT);
    head->SetSourceBusId(m_pBusModule->GetSelfBusID());
    head->SetDestBusId(pServerData->server_bus_id_);

    pServerData->net_client_->SendMsg(head, "", 0);

    delete head;
}

} // namespace ark
