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

#include "SDK/Interface/AFIPluginManager.h"
#include "AFCProxyNetModule.h"

namespace ark
{

    bool AFCProxyNetModule::Init()
    {
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pMsgModule = pPluginManager->FindModule<AFIMsgModule>();
        m_pNetServiceManagerModule = pPluginManager->FindModule<AFINetServiceManagerModule>();

        return true;
    }

    bool AFCProxyNetModule::PostInit()
    {

        int ret = StartServer();
        if (ret != 0)
        {
            exit(0);
            return false;
        }

        return true;
    }

    int AFCProxyNetModule::StartServer()
    {
        int ret = m_pNetServiceManagerModule->CreateServer();
        if (ret != 0)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            ARK_ASSERT_NO_EFFECT(0);
            return ret;
        }

        m_pNetServer = m_pNetServiceManagerModule->GetSelfNetServer();
        if (m_pNetServer == nullptr)
        {
            ret = -3;
            ARK_LOG_ERROR("Cannot find server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            return ret;
        }

        m_pNetServer->AddNetRecvCallback(AFMsg::EGMI_REQ_CONNECT_KEY, this, &AFCProxyNetModule::OnConnectKeyProcess);
        m_pNetServer->AddNetRecvCallback(AFMsg::EGMI_REQ_WORLD_LIST, this, &AFCProxyNetModule::OnReqServerListProcess);
        m_pNetServer->AddNetRecvCallback(AFMsg::EGMI_REQ_SELECT_SERVER, this, &AFCProxyNetModule::OnSelectServerProcess);
        m_pNetServer->AddNetRecvCallback(AFMsg::EGMI_REQ_ROLE_LIST, this, &AFCProxyNetModule::OnReqRoleListProcess);
        m_pNetServer->AddNetRecvCallback(AFMsg::EGMI_REQ_CREATE_ROLE, this, &AFCProxyNetModule::OnReqCreateRoleProcess);
        m_pNetServer->AddNetRecvCallback(AFMsg::EGMI_REQ_DELETE_ROLE, this, &AFCProxyNetModule::OnReqDelRoleProcess);
        m_pNetServer->AddNetRecvCallback(AFMsg::EGMI_REQ_ENTER_GAME, this, &AFCProxyNetModule::OnReqEnterGameServer);
        //m_pNetServer->AddNetRecvCallback(this, &AFCProxyNetServerModule::OnTransMessage);

        //m_pNetServer->AddNetEventCallBack(this, &AFCProxyNetServerModule::OnSocketEvent);

        return 0;
    }

    bool AFCProxyNetModule::PreUpdate()
    {
        int ret = StartClient();
        return (ret == 0);
    }

    int AFCProxyNetModule::StartClient()
    {
        //创建所有与对端链接的client
        int ret = m_pNetServiceManagerModule->CreateClusterClients();
        if (ret != 0)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            ARK_ASSERT_NO_EFFECT(0);
            return ret;
        }

        //if need to add a member
        AFINetClientService* pNetClientWorld = m_pNetServiceManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_WORLD);
        if (pNetClientWorld == nullptr)
        {
            return -1;
        }

        pNetClientWorld->AddNetRecvCallback(AFMsg::EGMI_ACK_CONNECT_WORLD, this, &AFCProxyNetModule::OnSelectServerResultProcess);
        pNetClientWorld->AddNetRecvCallback(AFMsg::EGMI_STS_NET_INFO, this, &AFCProxyNetModule::OnServerInfoProcess);
        pNetClientWorld->AddNetRecvCallback(AFMsg::EGMI_GTG_BROCASTMSG, this, &AFCProxyNetModule::OnBrocastmsg);
        //pNetClientWorld->AddRecvCallback(AFMsg::E_SS_MSG_ID_SERVER_NOTIFY, this, &AFCProxyNetClientModule::OnRecvServerNotify);
        //pNetClientWorld->AddNetRecvCallback(this, &AFCProxyNetClientModule::OnOtherMessage);
        //pNetClientWorld->AddEventCallBack(this, &AFCProxyNetClientModule::OnSocketEvent);


        //AFINetClientService* pNetClientGame = m_pNetClientManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_GAME);
        //if (pNetClientGame == nullptr)
        //{
        //    return -1;
        //}

        //pNetClientGame->AddRecvCallback(AFMsg::EGMI_ACK_ENTER_GAME, this, &AFCProxyNetClientModule::OnAckEnterGame);
        //pNetClientGame->AddRecvCallback(AFMsg::EGMI_GTG_BROCASTMSG, this, &AFCProxyNetClientModule::OnBrocastmsg);
        //pNetClientGame->AddRecvCallback(this, &AFCProxyNetClientModule::OnOtherMessage);
        //pNetClientGame->AddEventCallBack(this, &AFCProxyNetClientModule::OnSocketEvent);

        return 0;
    }

    void AFCProxyNetModule::OnServerInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

        //for (int i = 0; i < xMsg.server_list_size(); ++i)
        //{
        //    const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        //    //TODO:修改为url
        //    //AFConnectionData xServerData;
        //    //xServerData._server_bus_id = xData.bus_id();
        //    //xServerData._protocol = xData.protocol();
        //    //xServerData._ip = xData.ip();
        //    //xServerData._is_ip_v6 = xData.ip_v6();

        //    //m_pNetClientManagerModule->AddDynamicNetClientService(xServerData);
        //}
    }

    void AFCProxyNetModule::OnSelectServerResultProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::AckConnectWorldResult);
        //ARK_SHARE_PTR<ClientConnectData> pConnectData = mxWantToConnectMap.GetElement(xMsg.account());

        //if (pConnectData != nullptr)
        //{
        //    pConnectData->strConnectKey = xMsg.world_key();
        //    return;
        //}

        //pConnectData = std::make_shared<ClientConnectData>();
        //pConnectData->strAccount = xMsg.account();
        //pConnectData->strConnectKey = xMsg.world_key();
        //mxWantToConnectMap.AddElement(pConnectData->strAccount, pConnectData);
    }

    bool AFCProxyNetModule::VerifyConnectData(const std::string& strAccount, const std::string& strKey)
    {
        ARK_SHARE_PTR<ClientConnectData> pConnectData = mxWantToConnectMap.GetElement(strAccount);

        if (pConnectData != nullptr && strKey == pConnectData->strConnectKey)
        {
            mxWantToConnectMap.RemoveElement(strAccount);
            return true;
        }

        return false;
    }

    //void AFCProxyNetServerModule::OnTransMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    //{
    //    ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

    //    if (!pSessionData || pSessionData->mnLogicState <= 0 || pSessionData->mnGameID <= 0)
    //    {
    //        //state error
    //        return;
    //    }

    //    if (pSessionData->mnUserID != xHead.GetUID())
    //    {
    //        //when after entergame xHead.GetPlayerID() is really palyerID
    //        ARK_LOG_ERROR("xHead.GetPlayerID() is not really palyerID, id = {}", xHead.GetUID().ToString());
    //        return;
    //    }

    //    m_pMsgModule->SendSSMsg(pSessionData->mnGameID, nMsgID, msg, nLen, xHead.GetUID());
    //}

    void AFCProxyNetModule::OnConnectKeyProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ReqAccountLogin);
        //bool bRet = m_pProxyNetClientModule->VerifyConnectData(xMsg.account(), xMsg.security_code());

        //if (bRet)
        //{
        //    //可以进入,设置标志，选单服,心跳延迟,进入gs创建角色和删除角色,这里只是转发
        //    ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

        //    if (pSessionData)
        //    {
        //        pSessionData->mnLogicState = 1;
        //        pSessionData->mstrAccout = xMsg.account();

        //        AFMsg::AckEventResult xSendMsg;
        //        xSendMsg.set_event_code(AFMsg::EGEC_VERIFY_KEY_SUCCESS);
        //        *xSendMsg.mutable_event_client() = AFIMsgModule::GUIDToPB(pSessionData->mnClientID);//让前端记得自己的fd，后面有一些验证
        //        *xSendMsg.mutable_event_object() = AFIMsgModule::GUIDToPB(nPlayerID);

        //        m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_CONNECT_KEY, xSendMsg, xClientID, nPlayerID);
        //    }
        //}
        //else
        //{
        //    m_pNetServer->GetNet()->CloseNetEntity(xClientID);
        //}
    }

    void AFCProxyNetModule::OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id)
    {
        switch (event)
        {
        case CONNECTED:
            {
                ARK_LOG_INFO("Connected success, id = {}", conn_id.ToString());
                OnClientConnected(conn_id);
            }
            break;
        case DISCONNECTED:
            {
                ARK_LOG_INFO("Connection closed, id = {}", conn_id.ToString());
                OnClientDisconnect(conn_id);
            }
            break;
        default:
            break;
        }
    }

    void AFCProxyNetModule::OnClientDisconnect(const AFGUID& xClientID)
    {
        ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

        if (pSessionData)
        {
            if (pSessionData->mnGameID > 0 && !pSessionData->mnUserID.IsNULL())
            {
                AFMsg::ReqLeaveGameServer xData;
                m_pMsgModule->SendParticularSSMsg(pSessionData->mnGameID, AFMsg::EGameMsgID::EGMI_REQ_LEAVE_GAME, xData, pSessionData->mnUserID);
            }

            mmSessionData.RemoveElement(xClientID);
        }
    }

    void AFCProxyNetModule::OnSelectServerProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ReqSelectServer);
        //AFINetClientService* pWorldNetService = m_pNetClientManagerModule->GetNetClientServiceByBusID(xMsg.world_id());
        //ARK_SHARE_PTR<AFConnectionData> pServerData = pWorldNetService->GetServerNetInfo(xMsg.world_id());
        //if (pServerData != nullptr && AFConnectionData::CONNECTED == pServerData->net_state_)
        //{
        //    ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

        //    if (pSessionData)
        //    {
        //        //now this client bind a game server, after this time, all message will be sent to this game server who bind with client
        //        pSessionData->mnGameID = xMsg.world_id();

        //        AFMsg::AckEventResult xResultMsg;
        //        xResultMsg.set_event_code(AFMsg::EGameEventCode::EGEC_SELECTSERVER_SUCCESS);
        //        m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xResultMsg, xClientID, nPlayerID);
        //        return;
        //    }
        //}

        //AFMsg::AckEventResult xSendMsg;
        //xSendMsg.set_event_code(AFMsg::EGameEventCode::EGEC_SELECTSERVER_FAIL);
        //m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xMsg, xClientID, nPlayerID);
    }

    void AFCProxyNetModule::OnReqServerListProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ReqServerList);

        //if (xMsg.type() != AFMsg::RSLT_GAMES_ERVER)
        //{
        //    return;
        //}

        //ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

        //if (pSessionData && pSessionData->mnLogicState > 0)
        //{
        //    //ack all gameserver data
        //    AFMsg::AckServerList xData;
        //    xData.set_type(AFMsg::RSLT_GAMES_ERVER);

        //    AFINetClientService* pGameNetService = m_pNetClientManagerModule->GetNetClientService(ARK_APP_GAME);
        //    if (pGameNetService == nullptr)
        //    {
        //        return;
        //    }

        //    AFMapEx<int, AFConnectionData>& xServerList = pGameNetService->GetServerList();
        //    for (auto pGameData = xServerList.First(); pGameData != nullptr; pGameData = xServerList.Next())
        //    {
        //        if (AFConnectionData::CONNECTED == pGameData->net_state_)
        //        {
        //            AFMsg::ServerInfo* pServerInfo = xData.add_info();

        //            pServerInfo->set_status(AFMsg::EServerState::EST_NARMAL);
        //            pServerInfo->set_server_id(pGameData->server_bus_id_);
        //            pServerInfo->set_wait_count(0);
        //        }
        //    }

        //    m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_WORLD_LIST, xData, xClientID, nPlayerID);
        //}
    }

    int AFCProxyNetModule::Transpond(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen)
    {
        ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xHead.GetUID());

        if (pSessionData)
        {
            m_pNetServer->GetNet()->SendRawMsg(nMsgID, msg, nLen, pSessionData->mnClientID, xHead.GetUID());
        }

        return true;
    }

    int AFCProxyNetModule::SendToPlayerClient(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID&  nClientID, const AFGUID&  nPlayer)
    {
        m_pNetServer->GetNet()->SendRawMsg(nMsgID, msg, nLen, nClientID, nPlayer);

        return true;
    }

    void AFCProxyNetModule::OnClientConnected(const AFGUID& xClientID)
    {
        ARK_SHARE_PTR<AFSessionData> pSessionData = std::make_shared<AFSessionData>();

        pSessionData->mnClientID = xClientID;
        mmSessionData.AddElement(xClientID, pSessionData);
    }

    void AFCProxyNetModule::OnReqRoleListProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        CheckSessionTransMsg<AFMsg::ReqRoleList>(xHead, nMsgID, msg, nLen, xClientID);
    }

    void AFCProxyNetModule::OnReqCreateRoleProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        //在没有正式进入游戏之前，nPlayerID都是FD
        CheckSessionTransMsg<AFMsg::ReqCreateRole>(xHead, nMsgID, msg, nLen, xClientID);
    }

    void AFCProxyNetModule::OnReqDelRoleProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        CheckSessionTransMsg<AFMsg::ReqDeleteRole>(xHead, nMsgID, msg, nLen, xClientID);
    }

    void AFCProxyNetModule::OnReqEnterGameServer(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        CheckSessionTransMsg<AFMsg::ReqEnterGameServer>(xHead, nMsgID, msg, nLen, xClientID);
    }

    int AFCProxyNetModule::EnterGameSuccessEvent(const AFGUID xClientID, const AFGUID xPlayerID)
    {
        ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

        if (pSessionData)
        {
            pSessionData->mnUserID = xPlayerID;
        }

        return 0;
    }

    bool AFCProxyNetModule::CheckSessionState(const int64_t nGameID, const AFGUID& xClientID, const std::string& strAccount)
    {
        AFINetClientService* pWorldNetService = m_pNetServiceManagerModule->GetNetClientServiceByBusID(nGameID);
        ARK_SHARE_PTR<AFConnectionData> pServerData = pWorldNetService->GetServerNetInfo(nGameID);
        if (pServerData != nullptr && AFConnectionData::CONNECTED == pServerData->net_state_)
        {
            //数据匹配
            ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

            if (pSessionData
                    && pSessionData->mnLogicState > 0
                    && pSessionData->mnGameID == nGameID
                    && pSessionData->mstrAccout == strAccount)
            {
                return true;
            }
        }

        return false;
    }

    void AFCProxyNetModule::OnOtherMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        Transpond(xHead, nMsgID, msg, nLen);
    }

    void AFCProxyNetModule::OnBrocastmsg(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::BrocastMsg);

        //for (int i = 0; i < xMsg.target_entity_list_size(); i++)
        //{
        //    const AFMsg::PBGUID& xPlayerClientID = xMsg.target_entity_list(i);
        //    m_pProxyServerNet_ServerModule->SendToPlayerClient(xMsg.msg_id(), xMsg.msg_data().c_str(), xMsg.msg_data().size(), AFIMsgModule::PBToGUID(xPlayerClientID), nPlayerID);
        //}
    }

    void AFCProxyNetModule::OnAckEnterGame(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::AckEventResult);

        //if (xMsg.event_code() == AFMsg::EGEC_ENTER_GAME_SUCCESS)
        //{
        //    const AFGUID& xClient = AFIMsgModule::PBToGUID(xMsg.event_client());
        //    const AFGUID& xPlayer = AFIMsgModule::PBToGUID(xMsg.event_object());

        //    m_pProxyServerNet_ServerModule->EnterGameSuccessEvent(xClient, xPlayer);
        //}
    }

}