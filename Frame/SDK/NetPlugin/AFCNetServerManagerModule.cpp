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

#include "AFCNetServerManagerModule.h"

AFCNetServer::AFCNetServer(AFIPluginManager* p)
{
    nLastTime = p->GetNowTime();
}

AFCNetServer::~AFCNetServer()
{
    if (m_pNet)
    {
        m_pNet->Shutdown();
        ARK_DELETE(m_pNet);
    }
}

int AFCNetServer::Start(const int bus_id, const std::string& url, const uint8_t thread_count, const uint32_t max_connection)
{
    std::regex reg("(tcp|udp|wss|http|https)://([^/ :]+):?([^/ ]*)");
    std::cmatch what;
    if (std::regex_match(url.c_str(), what, reg))
    {
        string protocol = string(what[1].first, what[1].second);
        string domain = string(what[2].first, what[2].second);
        string port = string(what[3].first, what[3].second);

        bool is_ip_v6 = false;
        std::string ip = {};
        if (!AFMisc::GetHost(domain, is_ip_v6, ip))
        {
            return false;
        }

        if (protocol == "tcp")
        {
            m_pNet = ARK_NEW AFCTCPServer(this, &AFCNetServer::OnRecvNetPack, &AFCNetServer::OnSocketNetEvent);
            return m_pNet->Start(bus_id, ip, ARK_LEXICAL_CAST<int>(port), thread_count, max_connection, is_ip_v6);
        }
        else if (protocol == "udp")
        {
            //for now, do not support udp server
        }
        else if (protocol == "wss")
        {
            //will add websocket server
        }
        else
        {
            //other protocol will be supported
            return -1;
        }
    }
    else
    {
        return -1;
    }

    return -1;
}

bool AFCNetServer::Update()
{
    ARK_ASSERT_RET_VAL(m_pNet != nullptr, false);
    m_pNet->Update();
    return true;
}

bool AFCNetServer::SendBroadcastMsg(const int msg_id, const std::string& msg, const AFGUID& player_id)
{
    return m_pNet->SendMsgToAllClient(msg.c_str(), msg.length(), player_id);
}

bool AFCNetServer::SendBroadcastPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& player_id)
{
    std::string send_data;
    if (!pb_msg.SerializeToString(&send_data))
    {
        return false;
    }

    return SendBroadcastMsg(msg_id, send_data, player_id);
}

bool AFCNetServer::SendPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list/* = nullptr*/)
{
    std::string send_data;
    if (!pb_msg.SerializeToString(&send_data))
    {
        return false;
    }

    return SendMsg(msg_id, send_data, connect_id, player_id, target_list);
}

bool AFCNetServer::SendMsg(const uint16_t msg_id, const std::string& data, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list/* = nullptr*/)
{
    if (m_pNet != nullptr)
    {
        return false;
    }

    if (target_list != nullptr && !target_list->empty())
    {
        //playerid主要是网关转发消息的时候做识别使用，其他情况不使用
        AFMsg::BrocastMsg xMsg;
        *xMsg.mutable_entity_id() = AFIMsgModule::GUIDToPB(player_id);
        xMsg.set_msg_data(data);
        xMsg.set_msg_id(msg_id);

        for (auto iter : *target_list)
        {
            AFMsg::PBGUID* pData = xMsg.add_target_entity_list();
            *pData = AFIMsgModule::GUIDToPB(iter);
        }

        std::string send_data;
        if (!xMsg.SerializeToString(&send_data))
        {
            return false;
        }

        return m_pNet->SendMsgWithOutHead(AFMsg::EGMI_GTG_BROCASTMSG, data.data(), data.size(), connect_id, player_id);
    }
    else
    {
        return m_pNet->SendMsgWithOutHead(msg_id, data.data(), data.size(), connect_id, player_id);
    }
}

AFINet* AFCNetServer::GetNet()
{
    return m_pNet;
}

bool AFCNetServer::AddRecvCallback(const int nMsgID, const NET_RECV_FUNCTOR_PTR& cb)
{
    if (mxRecvCallBack.find(nMsgID) != mxRecvCallBack.end())
    {
        return false;
    }
    else
    {
        mxRecvCallBack.insert(std::make_pair(nMsgID, cb));
        return true;
    }
}

bool AFCNetServer::AddRecvCallback(const NET_RECV_FUNCTOR_PTR& cb)
{
    mxCallBackList.push_back(cb);

    return true;
}

bool AFCNetServer::AddEventCallBack(const NET_EVENT_FUNCTOR_PTR& cb)
{
    mxEventCallBackList.push_back(cb);

    return true;
}

void AFCNetServer::OnRecvNetPack(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID)
{
    auto it = mxRecvCallBack.find(nMsgID);

    if (mxRecvCallBack.end() != it)
    {
        (*it->second)(xHead, nMsgID, msg, nLen, xClientID);
    }
    else
    {
        for (const auto& iter : mxCallBackList)
        {
            (*iter)(xHead, nMsgID, msg, nLen, xClientID);
        }
    }
}

void AFCNetServer::OnSocketNetEvent(const NetEventType event, const AFGUID& connect_id, int bus_id)
{
    for (const auto& it : mxEventCallBackList)
    {
        (*it)(event, connect_id, bus_id);
    }
}

//////////////////////////////////////////////////////////////////////////
bool AFCNetServerManagerModule::Init()
{
    m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();

    return true;
}

int AFCNetServerManagerModule::CreateServer()
{
    const AFServerConfig* server_config = m_pBusModule->GetAppServerInfo();
    if (server_config == nullptr)
    {
        ARK_LOG_ERROR("Cannot get proce server info, bus id = {}", m_pBusModule->GetSelfBusID());
        ARK_ASSERT_NO_EFFECT(0);
        return -1;
    }

    AFINetServer* pServer = ARK_NEW AFCNetServer(pPluginManager);
    _net_servers.AddElement(m_pBusModule->GetSelfBusID(), pServer);

    int nRet = pServer->Start(m_pBusModule->GetSelfBusID(), server_config->private_url, server_config->thread_num, server_config->max_connection);
    if (nRet < 0)
    {
        ARK_LOG_ERROR("Cannot start server net, Port = {}", server_config->port);
        ARK_ASSERT_NO_EFFECT(0);
        return -2;
    }

    return 0;
}

AFINetServer* AFCNetServerManagerModule::GetSelfNetServer()
{
    return _net_servers.GetElement(m_pBusModule->GetSelfBusID());
}

bool AFCNetServerManagerModule::Update()
{
    for (bool bRet = _net_servers.Begin(); bRet; bRet = _net_servers.Increase())
    {
        const auto& pServer = _net_servers.GetCurrentData();
        if (pServer != nullptr)
        {
            pServer->Update();
        }
    }

    return true;
}