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

#include "proxy/include/proxy_net_module_impl.hpp"

namespace ark {

bool proxy_net_module_impl::init() {
    m_pBusModule = find_module<bus_module>();
    m_pMsgModule = find_module<msg_module>();
    m_pNetServiceManagerModule = find_module<net_service_manager_module>();

    return true;
}

bool proxy_net_module_impl::post_init() {
    StartServer();

    return true;
}

int proxy_net_module_impl::StartServer() {
    auto ret = m_pNetServiceManagerModule->create_server();
    if (!ret)
    {
        ARK_LOG_ERROR("Cannot start server net, busid = {}", m_pBusModule->get_self_bus_name());
        ARK_ASSERT_NO_EFFECT(0);
        exit(0);
    }

    m_pNetServer = m_pNetServiceManagerModule->get_inter_net_server();
    if (m_pNetServer == nullptr)
    {
        ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->get_self_bus_name());
        exit(0);
    }

    return 0;
}

void proxy_net_module_impl::OnServerInfoProcess(const net_msg* msg, conv_id_t session_id) {}

void proxy_net_module_impl::OnSelectServerResultProcess(const net_msg* msg, conv_id_t session_id) {}

bool proxy_net_module_impl::VerifyConnectData(const std::string& strAccount, const std::string& strKey) {
    auto pConnectData = mxWantToConnectMap.find_value(strAccount);
    if (pConnectData != nullptr && strKey == pConnectData->strConnectKey)
    {
        mxWantToConnectMap.erase(strAccount);
        return true;
    }

    return false;
}

void proxy_net_module_impl::OnSocketEvent(const net_event* event) {
    switch (event->get_type())
    {
    case net_event_type::CONNECT:
    {
        ARK_LOG_INFO("Connected success, id = {}", event->get_id());
        OnClientConnected(event->get_id());
    }
    break;
    case net_event_type::DISCONNECT:
    {
        ARK_LOG_INFO("Connection closed, id = {}", event->get_id());
        OnClientDisconnect(event->get_id());
    }
    break;
    default:
        break;
    }
}

void proxy_net_module_impl::OnClientConnected(const guid_t& conn_id) {
    //std::shared_ptr<AFClientConnectionData> pSessionData = std::make_shared<AFClientConnectionData>();

    //pSessionData->conn_id_ = conn_id;
    //client_connections_.insert(conn_id, pSessionData);
}

void proxy_net_module_impl::OnClientDisconnect(const guid_t& conn_id) {
    // do something
}

bool proxy_net_module_impl::CheckSessionState(
    const int nGameID, const guid_t& xClientID, const std::string& strAccount) {
    //AFINetClientService* pWorldNetService = m_pNetServiceManagerModule->GetNetClientServiceByBusID(nGameID);
    //std::shared_ptr<AFConnectionData> pServerData = pWorldNetService->GetServerNetInfo(nGameID);
    //if (pServerData != nullptr && AFConnectionData::CONNECTED == pServerData->net_state_)
    //{
    //    std::shared_ptr<AFClientConnectionData> pSessionData = client_connections_.find_value(xClientID);

    //    if (pSessionData != nullptr && pSessionData->logic_state_ > 0 && pSessionData->game_id_ == nGameID &&
    //        pSessionData->account_ == strAccount)
    //    {
    //        return true;
    //    }
    //}

    return false;
}

void proxy_net_module_impl::OnOtherMessage(const net_msg* msg, conv_id_t session_id) {
    // Transpond(xHead, nMsgID  msg, nLen);
}

void proxy_net_module_impl::OnBrocastmsg(const net_msg* msg, conv_id_t session_id) {
    // ARK_PROCESS_MSG(xHead, s g, nLen, AFMsg::BrocastMsg);

    // for (int i = 0; i < xMsg.target_entity_list_size(); i++)
    //{
    //    const AFMsg::PBGUID& xPlayerClientID = xMsg.target_entity_list(i);
    //    m_pProxyServerNet_ServerModule->SendToPlayerClient(xMsg.msg_id(), xMsg.msg_data().c_str(),
    //    xMsg.msg_data().size(), AFIMsgModule::PBToGUID(xPlayerC ientID), nPlayerID);
}

} // namespace ark
