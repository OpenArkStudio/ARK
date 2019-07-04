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

#include "base/AFDateTime.hpp"
#include "interface/AFIPluginManager.h"
#include "net/include/AFCTCPClient.h"
#include "net/include/AFCNetClientService.h"

namespace ark
{

    AFCNetClientService::AFCNetClientService(AFIPluginManager* p) :
        m_pPluginManager(p)
    {
        m_pNetServiceManagerModule = m_pPluginManager->FindModule<AFINetServiceManagerModule>();
        m_pBusModule = m_pPluginManager->FindModule<AFIBusModule>();
        m_pMsgModule = m_pPluginManager->FindModule<AFIMsgModule>();
        m_pLogModule = m_pPluginManager->FindModule<AFILogModule>();

        ARK_ASSERT_RET_NONE(m_pNetServiceManagerModule != nullptr &&
                            m_pBusModule != nullptr &&
                            m_pMsgModule != nullptr &&
                            m_pLogModule != nullptr);
    }

    bool AFCNetClientService::StartClient(const AFHeadLength head_len, const int& target_bus_id, const AFEndpoint& endpoint)
    {
        AFConnectionData data;
        data.head_len_ = head_len;
        data.server_bus_id_ = target_bus_id;
        data.endpoint_ = endpoint;

        tmp_nets_.push_back(data);
        return true;
    }

    void AFCNetClientService::Update()
    {
        ProcessUpdate();
        ProcessAddNewNetClient();
    }

    void AFCNetClientService::Shutdown()
    {
        int id = 0;

        for (auto iter : target_servers_)
        {
            auto connection_data = iter.second;
            if (connection_data->net_client_ptr_ != nullptr)
            {
                ARK_DELETE(connection_data->net_client_ptr_); //shutdown in AFINet destructor function
            }
        }
    }

    bool AFCNetClientService::RegMsgCallback(const int msg_id, const NET_MSG_FUNCTOR_PTR& cb)
    {
        if (net_msg_callbacks_.find(msg_id) != net_msg_callbacks_.end())
        {
            return false;
        }
        else
        {
            net_msg_callbacks_.insert(std::make_pair(msg_id, cb));
            return true;
        }
    }

    bool AFCNetClientService::RegForwardMsgCallback(const NET_MSG_FUNCTOR_PTR& cb)
    {
        net_msg_forward_callbacks_.push_back(cb);
        return true;
    }

    bool AFCNetClientService::RegNetEventCallback(const NET_EVENT_FUNCTOR_PTR& cb)
    {
        net_event_callbacks_.push_back(cb);
        return true;
    }

    void AFCNetClientService::ProcessUpdate()
    {
        for (auto iter : target_servers_)
        {
            auto connection_data = iter.second;
            switch (connection_data->net_state_)
            {
            case AFConnectionData::DISCONNECT:
                {
                    //TO CHECK & TEST:do not need to release net_client_ptr_
                    connection_data->net_state_ = AFConnectionData::RECONNECT;
                    if (connection_data->net_client_ptr_ != nullptr)
                    {
                        connection_data->net_client_ptr_->Shutdown();
                        //ARK_DELETE(connection_data->_net_client_ptr);
                    }
                }
                break;
            case AFConnectionData::CONNECTING:
                {
                    if (connection_data->net_client_ptr_ != nullptr)
                    {
                        connection_data->net_client_ptr_->Update();
                    }
                }
                break;
            case AFConnectionData::CONNECTED:
                {
                    if (connection_data->net_client_ptr_ != nullptr)
                    {
                        connection_data->net_client_ptr_->Update();

                        KeepAlive(connection_data);
                    }
                }
                break;
            case AFConnectionData::RECONNECT:
                {
                    //RECONNECT 30s/times
                    if ((connection_data->last_active_time_ + 30 * AFTimespan::SECOND_MS) >= m_pPluginManager->GetNowTime())
                    {
                        break;
                    }

                    connection_data->last_active_time_ = m_pPluginManager->GetNowTime();

                    if (connection_data->net_client_ptr_ != nullptr)
                    {
                        connection_data->net_client_ptr_->Shutdown();
                        //ARK_DELETE(connection_data->_net_client_ptr);
                    }

                    //based on protocol to create a new client
                    //connection_data->_net_client_ptr = CreateNet(connection_data->_protocol);
                    bool ret = connection_data->net_client_ptr_->StartClient(connection_data->head_len_, connection_data->server_bus_id_, connection_data->endpoint_.GetIP(), connection_data->endpoint_.GetPort(), connection_data->endpoint_.IsV6());
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

    AFINet* AFCNetClientService::CreateNet(const proto_type proto)
    {
        if (proto == proto_type::tcp)
        {
            return ARK_NEW AFCTCPClient(this, &AFCNetClientService::OnNetMsg, &AFCNetClientService::OnNetEvent);
        }
        else if (proto == proto_type::udp)
        {
            //for now, do not support udp server
        }
        else if (proto == proto_type::ws)
        {
            //will add websocket server
        }

        //other protocol will be supported
        return nullptr;
    }

    void AFCNetClientService::LogServerInfo()
    {
        LogServerInfo("This is a client, begin to print Server Info----------------------------------");

        for (auto iter : target_servers_)
        {
            auto connection_data = iter.second;
            if (connection_data != nullptr)
            {
                std::string info = ARK_FORMAT("TargetBusID={} State={} url={}", connection_data->server_bus_id_, connection_data->net_state_, connection_data->endpoint_.ToString());
                LogServerInfo(info);
            }
        }

        LogServerInfo("This is a client, end to print Server Info----------------------------------");
    }

    void AFCNetClientService::KeepAlive(ARK_SHARE_PTR<AFConnectionData>& pServerData)
    {
        if ((pServerData->last_active_time_ + 10) > m_pPluginManager->GetNowTime())
        {
            return;
        }

        pServerData->last_active_time_ = m_pPluginManager->GetNowTime();

        KeepReport(pServerData);
        LogServerInfo();
    }

    bool AFCNetClientService::GetServerMachineData(const std::string& strServerID, AFCMachineNode& xMachineData)
    {
        uint32_t nCRC32 = AFCRC32::Sum(strServerID);
        return consistent_hashmap_.GetSuitNode(nCRC32, xMachineData);
    }

    void AFCNetClientService::AddServerWeightData(ARK_SHARE_PTR<AFConnectionData>& xInfo)
    {
        //create virtual node by weight
        for (int i = 0; i < EConstDefine_DefaultWeight; ++i)
        {
            AFCMachineNode vNode(i);

            vNode.nMachineID = xInfo->server_bus_id_;
            vNode.strIP = xInfo->endpoint_.GetIP();
            vNode.nPort = xInfo->endpoint_.GetPort();
            vNode.nWeight = EConstDefine_DefaultWeight;
            consistent_hashmap_.Insert(vNode);
        }
    }

    void AFCNetClientService::RemoveServerWeightData(ARK_SHARE_PTR<AFConnectionData>& xInfo)
    {
        for (int i = 0; i < EConstDefine_DefaultWeight; ++i)
        {
            AFCMachineNode vNode(i);

            vNode.nMachineID = xInfo->server_bus_id_;
            vNode.strIP = xInfo->endpoint_.GetIP();
            vNode.nPort = xInfo->endpoint_.GetPort();
            vNode.nWeight = EConstDefine_DefaultWeight;
            consistent_hashmap_.Erase(vNode);
        }
    }

    int AFCNetClientService::OnConnect(const AFNetEvent* event)
    {
        ARK_LOG_INFO("Connected [{}] successfully, ip={} session_id={}", AFBusAddr(event->bus_id_).ToString(), event->ip_, event->id_);

        ARK_SHARE_PTR<AFConnectionData> pServerInfo = GetServerNetInfo(event->bus_id_);

        if (pServerInfo != nullptr)
        {
            AddServerWeightData(pServerInfo);
            pServerInfo->net_state_ = AFConnectionData::CONNECTED;

            //add server-bus-id -> client-bus-id
            m_pNetServiceManagerModule->AddNetConnectionBus(event->bus_id_, pServerInfo->net_client_ptr_);
            //register to this server
            RegisterToServer(event->id_, event->bus_id_);
        }

        return 0;
    }

    int AFCNetClientService::OnDisconnect(const AFNetEvent* event)
    {
        ARK_LOG_ERROR("Disconnect [{}] successfully, ip={} session_id={}", AFBusAddr(event->bus_id_).ToString(), event->ip_, event->id_);

        ARK_SHARE_PTR<AFConnectionData> pServerInfo = GetServerNetInfo(event->bus_id_);

        if (pServerInfo != nullptr)
        {
            RemoveServerWeightData(pServerInfo);
            pServerInfo->net_state_ = AFConnectionData::DISCONNECT;
            pServerInfo->last_active_time_ = m_pPluginManager->GetNowTime();
            //remove net bus
            m_pNetServiceManagerModule->RemoveNetConnectionBus(event->bus_id_);
        }

        return 0;
    }

    void AFCNetClientService::RegisterToServer(const AFGUID& session_id, const int bus_id)
    {
        const AFServerConfig* server_config = m_pBusModule->GetAppServerInfo();
        if (server_config == nullptr)
        {
            ARK_ASSERT_NO_EFFECT(0);
            return;
        }

        AFMsg::msg_ss_server_report msg;
        msg.set_bus_id(server_config->self_id);
        msg.set_cur_online(0);
        msg.set_url(server_config->public_ep_.ToString());
        msg.set_max_online(server_config->max_connection);
        msg.set_logic_status(AFMsg::E_ST_NARMAL);

        m_pMsgModule->SendParticularSSMsg(bus_id, AFMsg::E_SS_MSG_ID_SERVER_REPORT, msg, session_id);
        ARK_LOG_INFO("Register self server_id = {}, target_id = {}", server_config->self_id, bus_id);
    }

    void AFCNetClientService::ProcessAddNewNetClient()
    {
        for (auto& iter : tmp_nets_)
        {
            const AFConnectionData& connection_data = iter;
            ARK_SHARE_PTR<AFConnectionData> target_connection_data = target_servers_.find_value(connection_data.server_bus_id_);
            if (nullptr == target_connection_data)
            {
                //add new server
                target_connection_data = std::make_shared<AFConnectionData>();

                *target_connection_data = connection_data;
                target_connection_data->last_active_time_ = m_pPluginManager->GetNowTime();

                //based on protocol to create a new client
                target_connection_data->net_client_ptr_ = CreateNet(target_connection_data->endpoint_.proto());
                int ret = target_connection_data->net_client_ptr_->StartClient(target_connection_data->head_len_, target_connection_data->server_bus_id_, target_connection_data->endpoint_.GetIP(), target_connection_data->endpoint_.GetPort());
                if (!ret)
                {
                    target_connection_data->net_state_ = AFConnectionData::RECONNECT;
                }
                else
                {
                    target_connection_data->net_state_ = AFConnectionData::CONNECTING;
                }

                target_servers_.insert(target_connection_data->server_bus_id_, target_connection_data);
                AFINetClientService::RegMsgCallback(AFMsg::E_SS_MSG_ID_SERVER_NOTIFY, this, &AFCNetClientService::OnServerNotify);
            }
        }

        tmp_nets_.clear();
    }

    void AFCNetClientService::OnNetMsg(const AFNetMsg* msg, const int64_t session_id)
    {
        auto it = net_msg_callbacks_.find(msg->id_);

        if (net_msg_callbacks_.end() != it)
        {
            (*it->second)(msg, session_id);
        }
        else
        {
            //TODO:forward to other server process

            ARK_LOG_ERROR("Invalid message, id = {}", msg->id_);
            //for (const auto& iter : mxCallBackList)
            //{
            //    (*iter)(head, msg_id, msg, msg_len, session_id);
            //}
        }
    }

    void AFCNetClientService::OnNetEvent(const AFNetEvent* event)
    {
        switch (event->type_)
        {
        case CONNECTED:
            OnConnect(event);
            break;
        case DISCONNECTED:
            OnDisconnect(event);
            break;
        default:
            break;
        }

        for (const auto& it : net_event_callbacks_)
        {
            (*it)(event);
        }
    }

    ARK_SHARE_PTR<AFConnectionData> AFCNetClientService::GetServerNetInfo(const int nServerID)
    {
        return target_servers_.find_value(nServerID);
    }

    AFMapEx<int, AFConnectionData>& AFCNetClientService::GetServerList()
    {
        return target_servers_;
    }

    void AFCNetClientService::OnServerNotify(const AFNetMsg* msg, const int64_t session_id)
    {
        ARK_PROCESS_MSG(msg, AFMsg::msg_ss_server_notify);

        for (int i = 0; i < pb_msg.server_list_size(); ++i)
        {
            const AFMsg::msg_ss_server_report& report = pb_msg.server_list(i);
            ArkBusRelationType relation_type = m_pBusModule->GetBusRelationType(report.bus_id());
            if (relation_type != ArkBusRelationType::BRT_WAIT_NOTIFY)
            {
                continue;
            }

            //Create single cluster client with bus id and URL
            m_pNetServiceManagerModule->CreateClusterClient(AFHeadLength::SS_HEAD_LENGTH, report.bus_id(), report.url());

            //manage save zone by channel & zone & process
            AFBusAddr bus_addr(report.bus_id());
            bus_addr.inst_id = 0;//same zone when three front number is same

            auto iter = reg_servers_.find(bus_addr.bus_id);
            if (iter != reg_servers_.end())
            {
                iter->second.insert(std::make_pair(report.bus_id(), report));
            }
            else
            {
                std::map<int, AFMsg::msg_ss_server_report> others;
                others.insert(std::make_pair(report.bus_id(), report));
                reg_servers_.insert(std::make_pair(bus_addr.bus_id, others));
            }
        }
    }

}