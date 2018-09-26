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

#include "AFCWorldNetClientModule.h"

bool AFCWorldNetClientModule::Init()
{
    m_pLogModule                = pPluginManager->FindModule<AFILogModule>();
    m_pBusModule                = pPluginManager->FindModule<AFIBusModule>();
    m_pMsgModule                = pPluginManager->FindModule<AFIMsgModule>();
    m_pWorldNetServerModule     = pPluginManager->FindModule<AFIWorldNetServerModule>();
    m_pNetClientManagerModule   = pPluginManager->FindModule<AFINetClientManagerModule>();

    return true;
}

bool AFCWorldNetClientModule::PostInit()
{
    int ret = StartClient();
    return (ret == 0);
}

int AFCWorldNetClientModule::StartClient()
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
    AFINetClientService* pNetClientWorld = m_pNetClientManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_MASTER);
    if (pNetClientWorld == nullptr)
    {
        return -1;
    }

    pNetClientWorld->AddRecvCallback(AFMsg::EGMI_REQ_CONNECT_WORLD, this, &AFCWorldNetClientModule::OnSelectServerProcess);
    pNetClientWorld->AddRecvCallback(AFMsg::EGMI_REQ_KICK_CLIENT_INWORLD, this, &AFCWorldNetClientModule::OnKickClientProcess);
    pNetClientWorld->AddRecvCallback(this, &AFCWorldNetClientModule::InvalidMessage);

    pNetClientWorld->AddEventCallBack(this, &AFCWorldNetClientModule::OnSocketMSEvent);

    return 0;
}

void AFCWorldNetClientModule::Register(const int bus_id)
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

void AFCWorldNetClientModule::RefreshWorldInfo()
{
    //Will add
}

void AFCWorldNetClientModule::OnSelectServerProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqConnectWorld);

    ARK_SHARE_PTR<AFServerData> xServerData = m_pWorldNetServerModule->GetSuitProxyForEnter();

    if (xServerData)
    {
        AFMsg::AckConnectWorldResult xData;

        xData.set_world_id(xMsg.world_id());
        xData.mutable_sender()->CopyFrom(xMsg.sender());
        xData.set_login_id(xMsg.login_id());
        xData.set_account(xMsg.account());
        xData.set_world_url(xServerData->xData.url());
        xData.set_world_key(xMsg.account());

        m_pWorldNetServerModule->GetNetServer()->SendPBMsg(AFMsg::EGMI_ACK_CONNECT_WORLD, xData, xServerData->xClient, nPlayerID);

        //TODO:will fix this
        //m_pNetClientModule->SendSuitByPB(xMsg.account(), AFMsg::EGMI_ACK_CONNECT_WORLD, xData, xHead.GetPlayerID());
    }

}

void AFCWorldNetClientModule::OnKickClientProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqKickFromWorld);
}

void AFCWorldNetClientModule::InvalidMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_LOG_ERROR("invalid msg id = {}", nMsgID);
}

void AFCWorldNetClientModule::OnSocketMSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
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

void AFCWorldNetClientModule::OnClientDisconnect(const AFGUID& xClientID)
{
    //do something
}

void AFCWorldNetClientModule::OnClientConnected(const AFGUID& xClientID)
{
    //do something
}

void AFCWorldNetClientModule::LogServerInfo(const std::string& strServerInfo)
{
    ARK_LOG_INFO("{}", strServerInfo);
}

