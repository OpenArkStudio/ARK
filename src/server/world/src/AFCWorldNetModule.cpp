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

#include "world/include/AFCWorldNetModule.hpp"

namespace ark {

bool AFCWorldNetModule::Init()
{
    mnLastCheckTime = GetPluginManager()->GetNowTime();

    m_pKernelModule = FindModule<AFIKernelModule>();
    m_pBusModule = FindModule<AFIBusModule>();
    m_pMsgModule = FindModule<AFIMsgModule>();
    m_pNetServiceManagerModule = FindModule<AFINetServiceManagerModule>();

    return true;
}

bool AFCWorldNetModule::PostInit()
{
    int ret = StartServer();
    if (ret != 0)
    {
        exit(0);
    }

    return true;
}

int AFCWorldNetModule::StartServer()
{
    auto ret = m_pNetServiceManagerModule->CreateServer();
    if (!ret)
    {
        ARK_LOG_ERROR("Cannot start server net, busid = {}", m_pBusModule->GetSelfBusName());
        ARK_ASSERT_NO_EFFECT(0);
        exit(0);
    }

    m_pNetServer = m_pNetServiceManagerModule->GetInterNetServer();
    if (m_pNetServer == nullptr)
    {
        ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->GetSelfBusName());
        exit(0);
    }

    return 0;
}

//bool AFCWorldNetModule::PreUpdate()
//{
//    int ret = StartClient();
//    return (ret == 0);
//}
//
//int AFCWorldNetModule::StartClient()
//{
//    int ret = m_pNetServiceManagerModule->CreateClusterClients();
//    if (ret != 0)
//    {
//        ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
//        ARK_ASSERT_NO_EFFECT(0);
//        return ret;
//    }
//
//    // if need to add a member
//    AFINetClientService* pNetClientWorld =
//        m_pNetServiceManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_MASTER);
//    if (pNetClientWorld == nullptr)
//    {
//        return 0;
//    }
//
//    // pNetClientWorld->RegMsgCallback(AFMsg::EGMI_REQ_CONNECT_WORLD, this,
//    // &AFCWorldNetClientModule::OnSelectServerProcess);
//    // pNetClientWorld->RegMsgCallback(AFMsg::EGMI_REQ_KICK_CLIENT_INWORLD, this,
//    // &AFCWorldNetClientModule::OnKickClientProcess); pNetClientWorld->RegMsgCallback(this,
//    // &AFCWorldNetClientModule::InvalidMessage);
//
//    // pNetClientWorld->AddNetEventCallBack(this, &AFCWorldNetClientModule::OnSocketEvent);
//
//    return 0;
//}

// void AFCWorldNetServerModule::OnSocketEvent(const NetEventType event, const guid_t& conn_id, const std::string& ip,
// bus_id_t bus_id)
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
//            OnClientDisconnect(conn_id);
//        }
//        break;
//    default:
//        break;
//    }
//}

// void AFCWorldNetServerModule::OnClientDisconnect(const guid_t& xClientID)
//{
//    //for (std::shared_ptr<AFServerData> pServerData = mGameMap.First(); nullptr != pServerData; pServerData =
//    mGameMap.Next())
//    //{
//    //    if (xClientID == pServerData->xClient)
//    //    {
//    //        pServerData->xData.set_logic_status(AFMsg::EST_CRASH);
//    //        pServerData->xClient = 0;

//    //        return;
//    //    }
//    //}
//    ////////////////////////////////////////////////////////////////////////////
//    //int nServerID = 0;
//    //for (std::shared_ptr<AFServerData> pServerData = mProxyMap.First(); pServerData != nullptr; pServerData =
//    mProxyMap.Next())
//    //{
//    //    if (xClientID == pServerData->xClient)
//    //    {
//    //        nServerID = pServerData->xData.bus_id();
//    //        break;
//    //    }
//    //}

//    //mProxyMap.RemoveElement(nServerID);
//}

// void AFCWorldNetServerModule::OnClientConnected(const guid_t& xClientID)
//{
//    //log
//}

bool AFCWorldNetModule::SendMsgToGame(
    const int nGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const guid_t nPlayer)
{
    // std::shared_ptr<AFServerData> pData = mGameMap.GetElement(nGameID);

    // if (nullptr != pData)
    //{
    //    m_pNetServer->SendPBMsg(eMsgID, xData, pData->xClient, nPlayer);
    //}

    return true;
}

bool AFCWorldNetModule::SendMsgToGame(const AFIDataList& argObjectVar, const AFIDataList& argGameID,
    const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData)
{
    if (argGameID.GetCount() != argObjectVar.GetCount())
    {
        return false;
    }

    for (size_t i = 0; i < argObjectVar.GetCount(); i++)
    {
        guid_t identOther = argObjectVar.Int64(i);
        int nGameID = argGameID.Int(i);

        SendMsgToGame(nGameID, eMsgID, xData, identOther);
    }

    return true;
}

bool AFCWorldNetModule::SendMsgToPlayer(
    const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const guid_t nPlayer)
{
    int nGameID = GetPlayerGameID(nPlayer);

    if (nGameID < 0)
    {
        return false;
    }

    return SendMsgToGame(nGameID, eMsgID, xData, nPlayer);
}

//std::shared_ptr<AFServerData> AFCWorldNetModule::GetSuitProxyForEnter()
//{
//    return mProxyMap.begin()->second;
//}

std::shared_ptr<AFConnectionData> AFCWorldNetModule::GetSuitProxyForEnter()
{
    // return mProxyMap.begin()->second;
    auto pClientService = m_pNetServiceManagerModule->GetClientService(ARK_APP_TYPE::ARK_APP_PROXY);
    if (pClientService == nullptr)
    {
        ARK_LOG_INFO("Connection data not found connection");
        return nullptr;
    }

    return pClientService->GetSuitableConnect(NULL_STR); // todo : change key
}

std::shared_ptr<AFINetServerService> AFCWorldNetModule::GetNetServer()
{
    return m_pNetServer;
}

int AFCWorldNetModule::GetPlayerGameID(const guid_t& self)
{
    // do something
    return -1;
}

} // namespace ark
