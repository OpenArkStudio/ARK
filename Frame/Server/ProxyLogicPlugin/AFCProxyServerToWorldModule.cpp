/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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

#include "AFCProxyServerToWorldModule.h"
#include "SDK/Core/AFIHeartBeatManager.h"
#include "SDK/Core/AFCHeartBeatManager.h"
#include "SDK/Interface/AFIClassModule.h"

bool AFCProxyServerToWorldModule::Init()
{
    m_pProxyLogicModule             = FindModule<AFIProxyLogicModule>();
    m_pProxyServerNet_ServerModule  = FindModule<AFIProxyNetServerModule>();
    m_pLogModule                    = FindModule<AFILogModule>();
    m_pNetClientManagerModule       = FindModule<AFINetClientManagerModule>();
    m_pMsgModule                    = FindModule<AFIMsgModule>();
    m_pBusModule                    = FindModule<AFIBusModule>();

    return true;
}

bool AFCProxyServerToWorldModule::PostInit()
{
    int ret = StartClient();
    return (ret == 0);
}

int AFCProxyServerToWorldModule::StartClient()
{
    //创建所有与对端链接的client
    int ret = m_pNetClientManagerModule->CreateClusterClients();
    if (ret != 0)
    {
        ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
        ARK_ASSERT_NO_EFFECT(0);
        return ret;
    }

    //if need to add a member
    AFINetClientService* pNetClientWorld = m_pNetClientManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_WORLD);
    if (pNetClientWorld == nullptr)
    {
        return -1;
    }

    pNetClientWorld->AddRecvCallback(AFMsg::EGMI_ACK_CONNECT_WORLD, this, &AFCProxyServerToWorldModule::OnSelectServerResultProcess);
    pNetClientWorld->AddRecvCallback(AFMsg::EGMI_STS_NET_INFO, this, &AFCProxyServerToWorldModule::OnServerInfoProcess);
    pNetClientWorld->AddRecvCallback(AFMsg::EGMI_GTG_BROCASTMSG, this, &AFCProxyServerToWorldModule::OnBrocastmsg);
    pNetClientWorld->AddRecvCallback(this, &AFCProxyServerToWorldModule::OnOtherMessage);
    pNetClientWorld->AddEventCallBack(this, &AFCProxyServerToWorldModule::OnSocketWSEvent);


    AFINetClientService* pNetClientGame = m_pNetClientManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_GAME);
    if (pNetClientGame == nullptr)
    {
        return -1;
    }

    pNetClientGame->AddRecvCallback(AFMsg::EGMI_ACK_ENTER_GAME, this, &AFCProxyServerToWorldModule::OnAckEnterGame);
    pNetClientGame->AddRecvCallback(AFMsg::EGMI_GTG_BROCASTMSG, this, &AFCProxyServerToWorldModule::OnBrocastmsg);
    pNetClientGame->AddRecvCallback(this, &AFCProxyServerToWorldModule::OnOtherMessage);
    pNetClientGame->AddEventCallBack(this, &AFCProxyServerToWorldModule::OnSocketWSEvent);
}

void AFCProxyServerToWorldModule::OnServerInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        //TODO:修改为url
        //AFConnectionData xServerData;
        //xServerData._server_bus_id = xData.bus_id();
        //xServerData._protocol = xData.protocol();
        //xServerData._ip = xData.ip();
        //xServerData._is_ip_v6 = xData.ip_v6();

        //m_pNetClientManagerModule->AddDynamicNetClientService(xServerData);
    }
}

void AFCProxyServerToWorldModule::OnSocketWSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if (eEvent == DISCONNECTED)
    {
        ARK_LOG_INFO("Connection closed, id = {}", xClientID.ToString());
    }
    else if (eEvent == CONNECTED)
    {
        ARK_LOG_INFO("Connected success, id = {}", xClientID.ToString());
        Register(nServerID);
    }
}

void AFCProxyServerToWorldModule::Register(const int bus_id)
{
    AFINetClientService* pNetClient = m_pNetClientManagerModule->GetNetClientServiceByBusID(bus_id);
    if (pNetClient == nullptr)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return;
    }

    const AFServerConfig* server_config = m_pBusModule->GetAppServerInfo();
    if (server_config == nullptr)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return;
    }

    AFMsg::ServerInfoReportList xMsg;
    AFMsg::ServerInfoReport* pData = xMsg.add_server_list();

    pData->set_bus_id(server_config->self_id);
    pData->set_cur_online(0);
    pData->set_url(server_config->public_url);
    pData->set_max_online(server_config->max_connection);
    pData->set_logic_status(AFMsg::EST_NARMAL);

    m_pMsgModule->SendParticularSSMsg(bus_id, AFMsg::EGameMsgID::EGMI_GTW_GAME_REGISTERED, xMsg);
    ARK_LOG_INFO("Register self server_id = {}", pData->bus_id());
}

void AFCProxyServerToWorldModule::OnSelectServerResultProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::AckConnectWorldResult);
    ARK_SHARE_PTR<ClientConnectData> pConnectData = mxWantToConnectMap.GetElement(xMsg.account());

    if (pConnectData != nullptr)
    {
        pConnectData->strConnectKey = xMsg.world_key();
        return;
    }

    pConnectData = std::make_shared<ClientConnectData>();
    pConnectData->strAccount = xMsg.account();
    pConnectData->strConnectKey = xMsg.world_key();
    mxWantToConnectMap.AddElement(pConnectData->strAccount, pConnectData);
}

AFINetClientModule* AFCProxyServerToWorldModule::GetClusterModule()
{
    return m_pNetClientModule;
}

bool AFCProxyServerToWorldModule::VerifyConnectData(const std::string& strAccount, const std::string& strKey)
{
    ARK_SHARE_PTR<ClientConnectData> pConnectData = mxWantToConnectMap.GetElement(strAccount);

    if (pConnectData != nullptr && strKey == pConnectData->strConnectKey)
    {
        mxWantToConnectMap.RemoveElement(strAccount);
        return true;
    }

    return false;
}

void AFCProxyServerToWorldModule::LogServerInfo(const std::string& strServerInfo)
{
    ARK_LOG_INFO("{}", strServerInfo);
}

void AFCProxyServerToWorldModule::OnOtherMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    m_pProxyServerNet_ServerModule->Transpond(xHead, nMsgID, msg, nLen);
}

void AFCProxyServerToWorldModule::OnBrocastmsg(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::BrocastMsg);

    for (int i = 0; i < xMsg.target_entity_list_size(); i++)
    {
        const AFMsg::PBGUID& xPlayerClientID = xMsg.target_entity_list(i);
        m_pProxyServerNet_ServerModule->SendToPlayerClient(xMsg.msg_id(), xMsg.msg_data().c_str(), xMsg.msg_data().size(), AFIMsgModule::PBToGUID(xPlayerClientID), nPlayerID);
    }
}

void AFCProxyServerToWorldModule::OnAckEnterGame(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::AckEventResult);

    if (xMsg.event_code() == AFMsg::EGEC_ENTER_GAME_SUCCESS)
    {
        const AFGUID& xClient = AFIMsgModule::PBToGUID(xMsg.event_client());
        const AFGUID& xPlayer = AFIMsgModule::PBToGUID(xMsg.event_object());

        m_pProxyServerNet_ServerModule->EnterGameSuccessEvent(xClient, xPlayer);
    }
}