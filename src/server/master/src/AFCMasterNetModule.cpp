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

#include "interface/AFIPluginManager.h"
#include "master/include/AFCMasterNetModule.h"

namespace ark
{

bool AFCMasterNetModule::Init()
{
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
    m_pNetServiceManagerModule = pPluginManager->FindModule<AFINetServiceManagerModule>();
    m_pTimerModule = pPluginManager->FindModule<AFITimerModule>();
    m_pMsgModule = pPluginManager->FindModule<AFIMsgModule>();

    // log timer
    // m_pTimerModule->AddForeverTimer("master-sub-server-timer", pPluginManager->BusID(), 30 * 1000, this,
    // &AFCMasterNetServerModule::OnTimerLogServer);

    return true;
}

bool AFCMasterNetModule::PostInit()
{
    int ret = StartServer();
    if (ret != 0)
    {
        exit(0);
        return false;
    }

    return true;
}

int AFCMasterNetModule::StartServer()
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

    // m_pNetServer->RegMsgCallback(AFMsg::EGMI_STS_HEART_BEAT, this, &AFCMasterNetServerModule::OnHeartBeat);
    // m_pNetServer->RegMsgCallback(AFMsg::EGMI_MTL_WORLD_REGISTERED, this, &AFCMasterNetServerModule::OnWorldRegisteredProcess);
    // m_pNetServer->RegMsgCallback(AFMsg::EGMI_MTL_WORLD_UNREGISTERED, this, &AFCMasterNetServerModule::OnWorldUnRegisteredProcess);
    // m_pNetServer->RegMsgCallback(AFMsg::EGMI_MTL_WORLD_REFRESH, this, &AFCMasterNetServerModule::OnRefreshWorldInfoProcess);
    // m_pNetServer->RegMsgCallback(AFMsg::EGMI_LTM_LOGIN_REGISTERED, this, &AFCMasterNetServerModule::OnLoginRegisteredProcess);
    // m_pNetServer->RegMsgCallback(AFMsg::EGMI_LTM_LOGIN_UNREGISTERED, this, &AFCMasterNetServerModule::OnLoginUnRegisteredProcess);
    // m_pNetServer->RegMsgCallback(AFMsg::EGMI_LTM_LOGIN_REFRESH, this, &AFCMasterNetServerModule::OnRefreshLoginInfoProcess);
    // m_pNetServer->RegMsgCallback(AFMsg::EGMI_REQ_CONNECT_WORLD, this, &AFCMasterNetServerModule::OnSelectWorldProcess);
    // m_pNetServer->RegMsgCallback(AFMsg::EGMI_ACK_CONNECT_WORLD, this, &AFCMasterNetServerModule::OnSelectServerResultProcess);

    // m_pNetServer->RegMsgCallback(AFMsg::E_SS_MSG_ID_SERVER_REPORT, this, &AFCMasterNetServerModule::OnServerReport);

    // m_pNetServer->RegMsgCallback(this, &AFCMasterNetServerModule::InvalidMessage);

    // m_pNetServer->AddEventCallBack(this, &AFCMasterNetServerModule::OnSocketEvent);

    return 0;
}

// void AFCMasterNetServerModule::OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id)
//{
//    switch (event)
//    {
//    case CONNECTED:
//        {
//            ARK_LOG_INFO("Connected success, id = {}", conn_id.ToString());
//            OnClientConnected(conn_id);
//        }
//        break;
//    case DISCONNECTED:
//        {
//            ARK_LOG_INFO("Connection closed, id = {}", conn_id.ToString());
//            OnClientDisconnect(bus_id, conn_id);
//        }
//        break;
//    default:
//        break;
//    }
//}

// void AFCMasterNetServerModule::OnClientConnected(const AFGUID& conn_id)
//{
//    //连接上来啥都不做
//}

// void AFCMasterNetServerModule::OnClientDisconnect(int bus_id, const AFGUID& conn_id)
//{
//    reg_servers_.RemoveElement(bus_id);
//}

// void AFCMasterNetServerModule::OnWorldRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t
// nLen, const AFGUID& xClientID)
//{
//    ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

//    for (int i = 0; i < xMsg.server_list_size(); ++i)
//    {
//        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
//        ARK_SHARE_PTR<AFServerData> pServerData = mWorldMap.GetElement(xData.bus_id());

//        if (nullptr == pServerData)
//        {
//            pServerData = std::make_shared<AFServerData>();
//            mWorldMap.AddElement(xData.bus_id(), pServerData);
//        }

//        pServerData->Init(xClientID, xData);

//        ARK_LOG_INFO("WorldRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    }

//    SynWorldToLogin();
//}

// void AFCMasterNetServerModule::OnWorldUnRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const
// uint32_t nLen, const AFGUID& xClientID)
//{
//    ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

//    for (int i = 0; i < xMsg.server_list_size(); ++i)
//    {
//        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
//        mWorldMap.RemoveElement(xData.bus_id());

//        ARK_LOG_INFO("WorldUnRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    }

//    SynWorldToLogin();
//}

// void AFCMasterNetServerModule::OnRefreshWorldInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const
// uint32_t nLen, const AFGUID& xClientID)
//{
//    ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

//    for (int i = 0; i < xMsg.server_list_size(); ++i)
//    {
//        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
//        ARK_SHARE_PTR<AFServerData> pServerData = mWorldMap.GetElement(xData.bus_id());

//        if (nullptr == pServerData)
//        {
//            pServerData = std::make_shared<AFServerData>();
//            mWorldMap.AddElement(xData.bus_id(), pServerData);
//        }

//        pServerData->Init(xClientID, xData);

//        ARK_LOG_INFO("RefreshWorldInfo, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    }

//    SynWorldToLogin();
//}

// void AFCMasterNetServerModule::OnLoginRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t
// nLen, const AFGUID& xClientID)
//{
//    ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

//    for (int i = 0; i < xMsg.server_list_size(); ++i)
//    {
//        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
//        ARK_SHARE_PTR<AFServerData> pServerData = mLoginMap.GetElement(xData.bus_id());

//        if (nullptr == pServerData)
//        {
//            pServerData = std::make_shared<AFServerData>();
//            mLoginMap.AddElement(xData.bus_id(), pServerData);
//        }

//        pServerData->Init(xClientID, xData);

//        ARK_LOG_INFO("LoginRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    }

//    SynWorldToLogin();
//}

// void AFCMasterNetServerModule::OnLoginUnRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const
// uint32_t nLen, const AFGUID& xClientID)
//{
//    ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

//    for (int i = 0; i < xMsg.server_list_size(); ++i)
//    {
//        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

//        mLoginMap.RemoveElement(xData.bus_id());

//        ARK_LOG_INFO("LoginUnRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    }
//}

// void AFCMasterNetServerModule::OnRefreshLoginInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const
// uint32_t nLen, const AFGUID& xClientID)
//{
//    ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

//    for (int i = 0; i < xMsg.server_list_size(); ++i)
//    {
//        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
//        ARK_SHARE_PTR<AFServerData> pServerData = mLoginMap.GetElement(xData.bus_id());

//        if (nullptr == pServerData)
//        {
//            pServerData = std::make_shared<AFServerData>();
//            mLoginMap.AddElement(xData.bus_id(), pServerData);
//        }

//        pServerData->Init(xClientID, xData);

//        ARK_LOG_INFO("RefreshLoginInfo, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    }
//}

// void AFCMasterNetServerModule::OnSelectWorldProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t
// nLen, const AFGUID& xClientID)
//{
//    ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ReqConnectWorld);

//    ARK_SHARE_PTR<AFServerData> pServerData = mWorldMap.GetElement(xMsg.world_id());

//    if (nullptr == pServerData)
//    {
//        return;
//    }

//    //send to world
//    m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_REQ_CONNECT_WORLD, xMsg, pServerData->xClient, nPlayerID);
//}

// void AFCMasterNetServerModule::OnSelectServerResultProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const
// uint32_t nLen, const AFGUID& xClientID)
//{
//    ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::AckConnectWorldResult);
//    ARK_SHARE_PTR<AFServerData> pServerData = mLoginMap.GetElement(xMsg.login_id());

//    if (nullptr == pServerData)
//    {
//        return;
//    }

//    //转发送到登录服务器
//    m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_CONNECT_WORLD, xMsg, pServerData->xClient, nPlayerID);
//}

// void AFCMasterNetServerModule::SynWorldToLogin()
//{
//    AFMsg::ServerInfoReportList xData;

//    for (ARK_SHARE_PTR<AFServerData> pServerData = mWorldMap.First(); nullptr != pServerData; pServerData = mWorldMap.Next())
//    {
//        AFMsg::ServerInfoReport* pData = xData.add_server_list();
//        *pData = pServerData->xData;
//    }

//    //广播给所有loginserver
//    for (ARK_SHARE_PTR<AFServerData> pServerData = mLoginMap.First(); nullptr != pServerData; pServerData = mLoginMap.Next())
//    {
//        m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_STS_NET_INFO, xData, pServerData->xClient, AFGUID(0));
//    }
//}

// void AFCMasterNetServerModule::OnHeartBeat(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len,
// const AFGUID& conn_id)
//{
//    //do nothing
//}

// void AFCMasterNetServerModule::InvalidMessage(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len,
// const AFGUID& conn_id)
//{
//    ARK_LOG_ERROR("Invalid msg id = {}", msg_id);
//}

//////////////////////////////////////////////////////////////////////////
// void AFCMasterNetServerModule::OnTimerLogServer(const std::string& name, const AFGUID& id)
//{
//    ARK_LOG_INFO("---------------------------Start to log all registered server---------------------------");
//    for (bool ret = reg_servers_.Begin(); ret; ret = reg_servers_.Increase())
//    {
//        auto& server_data = reg_servers_.GetCurrentData();
//        ARK_LOG_INFO("bus[{}] state[{}] url[{}] conn_id[{}]",
//                     server_data->server_info_.bus_id(),
//                     AFMsg::e_ss_server_state_Name(server_data->server_info_.logic_status()),
//                     server_data->conn_id_.nLow);
//    }
//    ARK_LOG_INFO("---------------------------End to log all registered server---------------------------");
//}
//////////////////////////////////////////////////////////////////////////
// void AFCMasterNetServerModule::OnServerReport(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len,
// const AFGUID& conn_id)
//{
//    ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::msg_ss_server_report);
//    ARK_SHARE_PTR<AFServerData> server_data_ptr = reg_servers_.GetElement(x_msg.bus_id());
//    if (nullptr == server_data_ptr)
//    {
//        server_data_ptr = std::make_shared<AFServerData>();
//        reg_servers_.AddElement(x_msg.bus_id(), server_data_ptr);
//    }

//    server_data_ptr->Init(conn_id, x_msg);

//    ARK_LOG_INFO("Server Registered, server_id[{}] server_url[{}]", x_msg.bus_id(), x_msg.url());

//    SyncAllProxyToDir(x_msg.bus_id(), conn_id);
//}

// void AFCMasterNetServerModule::SyncAllProxyToDir(int bus_id, const AFGUID& conn_id)
//{
//    AFMsg::msg_ss_server_notify msg;
//    for (bool ret = reg_servers_.Begin(); ret; ret = reg_servers_.Increase())
//    {
//        auto& server_data = reg_servers_.GetCurrentData();
//        AFBusAddr bus_addr(server_data->server_info_.bus_id());
//        //只发proxy-server到dir，以供client连接
//        if (bus_addr.proc_id != ARK_APP_PROXY)
//        {
//            continue;
//        }

//        AFMsg::msg_ss_server_report* report = msg.add_server_list();
//        *report = server_data->server_info_;
//    }

//    m_pMsgModule->SendSSMsg(bus_id, AFMsg::E_SS_MSG_ID_SERVER_NOTIFY, msg);
//}

} // namespace ark