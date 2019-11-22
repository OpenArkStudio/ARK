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

#include "proxy/include/AFCProxyNetModule.hpp"

namespace ark {

bool AFCProxyNetModule::Init()
{
    m_pLogModule = FindModule<AFILogModule>();
    m_pBusModule = FindModule<AFIBusModule>();
    m_pMsgModule = FindModule<AFIMsgModule>();
    m_pNetServiceManagerModule = FindModule<AFINetServiceManagerModule>();

    return true;
}

bool AFCProxyNetModule::PostInit()
{
    StartServer();

    return true;
}

int AFCProxyNetModule::StartServer()
{
    auto ret = m_pNetServiceManagerModule->CreateServer();
    ret.Then([=](const std::pair<bool, std::string>& resp) {
        if (!resp.first)
        {
            ARK_LOG_ERROR(
                "Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), resp.second);
            ARK_ASSERT_NO_EFFECT(0);
            exit(0);
        }

        m_pNetServer = m_pNetServiceManagerModule->GetSelfNetServer();
        if (m_pNetServer == nullptr)
        {
            ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->GetSelfBusName());
            exit(0);
        }
    });

    return 0;
}

void AFCProxyNetModule::OnServerInfoProcess(const AFNetMsg* msg, const int64_t session_id) {}

void AFCProxyNetModule::OnSelectServerResultProcess(const AFNetMsg* msg, const int64_t session_id) {}

bool AFCProxyNetModule::VerifyConnectData(const std::string& strAccount, const std::string& strKey)
{
    auto pConnectData = mxWantToConnectMap.find_value(strAccount);
    if (pConnectData != nullptr && strKey == pConnectData->strConnectKey)
    {
        mxWantToConnectMap.erase(strAccount);
        return true;
    }

    return false;
}

void AFCProxyNetModule::OnSocketEvent(const AFNetEvent* event)
{
    switch (event->GetType())
    {
        case AFNetEventType::CONNECTED:
        {
            ARK_LOG_INFO("Connected success, id = {}", event->GetId());
            OnClientConnected(event->GetId());
        }
        break;
        case AFNetEventType::DISCONNECTED:
        {
            ARK_LOG_INFO("Connection closed, id = {}", event->GetId());
            OnClientDisconnect(event->GetId());
        }
        break;
        default:
            break;
    }
}

void AFCProxyNetModule::OnClientConnected(const AFGUID& conn_id)
{
    //std::shared_ptr<AFClientConnectionData> pSessionData = std::make_shared<AFClientConnectionData>();

    //pSessionData->conn_id_ = conn_id;
    //client_connections_.insert(conn_id, pSessionData);
}

void AFCProxyNetModule::OnClientDisconnect(const AFGUID& conn_id)
{
    // do something
}

bool AFCProxyNetModule::CheckSessionState(const int nGameID, const AFGUID& xClientID, const std::string& strAccount)
{
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

void AFCProxyNetModule::OnOtherMessage(const AFNetMsg* msg, const int64_t session_id)
{
    // Transpond(xHead, nMsgID  msg, nLen);
}

void AFCProxyNetModule::OnBrocastmsg(const AFNetMsg* msg, const int64_t session_id)
{
    // ARK_PROCESS_MSG(xHead, s g, nLen, AFMsg::BrocastMsg);

    // for (int i = 0; i < xMsg.target_entity_list_size(); i++)
    //{
    //    const AFMsg::PBGUID& xPlayerClientID = xMsg.target_entity_list(i);
    //    m_pProxyServerNet_ServerModule->SendToPlayerClient(xMsg.msg_id(), xMsg.msg_data().c_str(),
    //    xMsg.msg_data().size(), AFIMsgModule::PBToGUID(xPlayerC ientID), nPlayerID);
}

} // namespace ark
