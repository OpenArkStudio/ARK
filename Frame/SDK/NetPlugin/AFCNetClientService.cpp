#include "SDK/Core/AFDateTime.hpp"
#include "AFCTCPClient.h"
#include "AFCNetClientService.h"

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

    AFCNetClientService::~AFCNetClientService()
    {
        Shutdown();
    }

    bool AFCNetClientService::StartClient(const int& target_bus_id, const AFEndpoint& endpoint)
    {
        AFConnectionData data;
        data.server_bus_id_ = target_bus_id;
        data.endpoint_ = endpoint;

        _tmp_nets.push_back(data);
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

        for (auto connection_data = target_servers_.First(id); connection_data != nullptr; connection_data = target_servers_.Next(id))
        {
            if (connection_data->net_client_ptr_ != nullptr)
            {
                ARK_DELETE(connection_data->net_client_ptr_); //shutdown in AFINet destructor function
            }
        }
    }

    bool AFCNetClientService::RegMsgCallback(const int nMsgID, const NET_PKG_RECV_FUNCTOR_PTR& cb)
    {
        if (net_msg_callbacks_.find(nMsgID) != net_msg_callbacks_.end())
        {
            return false;
        }
        else
        {
            net_msg_callbacks_.insert(std::make_pair(nMsgID, cb));
            return true;
        }
    }

    bool AFCNetClientService::RegForwardMsgCallback(const NET_PKG_RECV_FUNCTOR_PTR& cb)
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
        target_servers_.DoEveryElement([ & ](AFMapEx<int, AFConnectionData>::PTRTYPE & connection_data)
        {
            switch (connection_data->net_state_)
            {
            case AFConnectionData::DISCONNECT:
                {
                    //TODO:这里不应该释放_net_client_ptr
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
                    //计算时间
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
                    bool ret = connection_data->net_client_ptr_->Start(connection_data->server_bus_id_, connection_data->endpoint_.ip(), connection_data->endpoint_.port(), connection_data->endpoint_.is_v6());
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
            return true;
        });
    }

    AFINet* AFCNetClientService::CreateNet(const proto_type proto)
    {
        if (proto == proto_type::tcp)
        {
            return ARK_NEW AFCTCPClient(this, &AFCNetClientService::OnRecvNetPack, &AFCNetClientService::OnSocketEvent);
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

        target_servers_.DoEveryElement([ = ](AFMapEx<int, AFConnectionData>::PTRTYPE & pServerData)
        {
            if (pServerData)
            {
                std::string info = ARK_FORMAT("TargetBusID={} State={} url={}", pServerData->server_bus_id_, pServerData->net_state_, pServerData->endpoint_.to_string());
                LogServerInfo(info);
            }
            return true;
        });

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
        uint32_t nCRC32 = CRC32(strServerID);
        return mxConsistentHash.GetSuitNode(nCRC32, xMachineData);
    }

    void AFCNetClientService::AddServerWeightData(ARK_SHARE_PTR<AFConnectionData>& xInfo)
    {
        //根据权重创建节点
        for (int i = 0; i < EConstDefine_DefaultWeight; ++i)
        {
            AFCMachineNode vNode(i);

            vNode.nMachineID = xInfo->server_bus_id_;
            vNode.strIP = xInfo->endpoint_.ip();
            vNode.nPort = xInfo->endpoint_.port();
            vNode.nWeight = EConstDefine_DefaultWeight;
            mxConsistentHash.Insert(vNode);
        }
    }

    void AFCNetClientService::RemoveServerWeightData(ARK_SHARE_PTR<AFConnectionData>& xInfo)
    {
        for (int i = 0; i < EConstDefine_DefaultWeight; ++i)
        {
            AFCMachineNode vNode(i);

            vNode.nMachineID = xInfo->server_bus_id_;
            vNode.strIP = xInfo->endpoint_.ip();
            vNode.nPort = xInfo->endpoint_.port();
            vNode.nWeight = EConstDefine_DefaultWeight;
            mxConsistentHash.Erase(vNode);
        }
    }

    int AFCNetClientService::OnConnect(const NetEventType event, const AFGUID& conn_id, const std::string& ip, int bus_id)
    {
        ARK_LOG_INFO("Connected [{}] successfully, ip={} conn_id={}", AFBusAddr(bus_id).ToString(), ip, conn_id.ToString());

        ARK_SHARE_PTR<AFConnectionData> pServerInfo = GetServerNetInfo(bus_id);

        if (pServerInfo != nullptr)
        {
            AddServerWeightData(pServerInfo);
            pServerInfo->net_state_ = AFConnectionData::CONNECTED;

            //add server-bus-id -> client-bus-id
            m_pNetServiceManagerModule->AddNetConnectionBus(bus_id, pServerInfo->net_client_ptr_);
            //register to this server
            RegisterToServer(conn_id, bus_id);
        }

        return 0;
    }

    void AFCNetClientService::RegisterToServer(const AFGUID& conn_id, const int bus_id)
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
        msg.set_url(server_config->public_ep_.to_string());
        msg.set_max_online(server_config->max_connection);
        msg.set_logic_status(AFMsg::E_ST_NARMAL);

        m_pMsgModule->SendParticularSSMsg(bus_id, AFMsg::E_SS_MSG_ID_SERVER_REPORT, msg, conn_id);
        ARK_LOG_INFO("Register self server_id = {}, target_id = {}", server_config->self_id, bus_id);
    }

    int AFCNetClientService::OnDisconnect(const NetEventType event, const AFGUID& conn_id, const std::string& ip, int bus_id)
    {
        ARK_LOG_INFO("Disconnect [{}], ip={} conn_id={}", AFBusAddr(bus_id).ToString(), ip, conn_id.ToString());

        ARK_SHARE_PTR<AFConnectionData> pServerInfo = GetServerNetInfo(bus_id);

        if (pServerInfo != nullptr)
        {
            RemoveServerWeightData(pServerInfo);
            pServerInfo->net_state_ = AFConnectionData::DISCONNECT;
            pServerInfo->last_active_time_ = m_pPluginManager->GetNowTime();
            //remove net bus
            m_pNetServiceManagerModule->RemoveNetConnectionBus(bus_id);
        }

        return 0;
    }

    void AFCNetClientService::ProcessAddNewNetClient()
    {
        for (auto& iter : _tmp_nets)
        {
            const AFConnectionData& connection_data = iter;
            ARK_SHARE_PTR<AFConnectionData> target_connection_data = target_servers_.GetElement(connection_data.server_bus_id_);
            if (nullptr == target_connection_data)
            {
                //正常，添加新服务器
                target_connection_data = std::make_shared<AFConnectionData>();

                *target_connection_data = connection_data;
                target_connection_data->last_active_time_ = m_pPluginManager->GetNowTime();

                //based on protocol to create a new client
                target_connection_data->net_client_ptr_ = CreateNet(target_connection_data->endpoint_.proto());
                int ret = target_connection_data->net_client_ptr_->Start(target_connection_data->server_bus_id_, target_connection_data->endpoint_.ip(), target_connection_data->endpoint_.port());
                if (!ret)
                {
                    target_connection_data->net_state_ = AFConnectionData::RECONNECT;
                }
                else
                {
                    target_connection_data->net_state_ = AFConnectionData::CONNECTING;
                }

                target_servers_.AddElement(target_connection_data->server_bus_id_, target_connection_data);
                AFINetClientService::RegMsgCallback(AFMsg::E_SS_MSG_ID_SERVER_NOTIFY, this, &AFCNetClientService::OnServerNotify);
            }
        }

        _tmp_nets.clear();
    }

    void AFCNetClientService::OnRecvNetPack(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id)
    {
        auto it = net_msg_callbacks_.find(msg_id);

        if (net_msg_callbacks_.end() != it)
        {
            (*it->second)(head, msg_id, msg, msg_len, conn_id);
        }
        else
        {
            //TODO:forward to other server process

            ARK_LOG_ERROR("Invalid message, id = {}", msg_id);
            //for (const auto& iter : mxCallBackList)
            //{
            //    (*iter)(head, msg_id, msg, msg_len, conn_id);
            //}
        }
    }

    void AFCNetClientService::OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, int bus_id)
    {
        switch (event)
        {
        case CONNECTED:
            OnConnect(event, conn_id, ip, bus_id);
            break;
        case DISCONNECTED:
            OnDisconnect(event, conn_id, ip, bus_id);
            break;
        default:
            break;
        }

        for (const auto& it : net_event_callbacks_)
        {
            (*it)(event, conn_id, ip, bus_id);
        }
    }

    //void AFCNetClientService::SendByServerID(const int nServerID, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    //{
    //    SendByServerID(nServerID, nMsgID, strData.c_str(), strData.length(), nPlayerID);
    //}

    //void AFCNetClientService::SendByServerID(const int nServerID, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID)
    //{
    //    ARK_SHARE_PTR<AFConnectionData> pServer = mxTargetServerMap.GetElement(nServerID);
    //    if (pServer != nullptr)
    //    {
    //        if (pServer->net_client_ptr_ != nullptr)
    //        {
    //            pServer->net_client_ptr_->SendRawMsg(nMsgID, msg, nLen, 0, nPlayerID);
    //        }
    //    }
    //}

    //void AFCNetClientService::SendToAllServer(const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    //{
    //    for (ARK_SHARE_PTR<AFConnectionData> pServer = mxTargetServerMap.First(); pServer != nullptr; pServer = mxTargetServerMap.Next())
    //    {
    //        if (pServer->net_client_ptr_ != nullptr)
    //        {
    //            pServer->net_client_ptr_->SendRawMsg(nMsgID, strData.data(), strData.size(), 0, nPlayerID);
    //        }
    //    }
    //}

    //void AFCNetClientService::SendToServerByPB(const int nServerID, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    //{
    //    std::string strData;

    //    if (!xData.SerializeToString(&strData))
    //    {
    //        return;
    //    }

    //    ARK_SHARE_PTR<AFConnectionData> pServer = mxTargetServerMap.GetElement(nServerID);

    //    if (pServer)
    //    {
    //        if (pServer->net_client_ptr_ != nullptr)
    //        {
    //            pServer->net_client_ptr_->SendRawMsg(nMsgID, strData.data(), strData.length(), AFGUID(0), nPlayerID);
    //        }
    //    }
    //}

    //void AFCNetClientService::SendToAllServerByPB(const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    //{
    //    std::string strData;

    //    if (!xData.SerializeToString(&strData))
    //    {
    //        return;
    //    }

    //    for (ARK_SHARE_PTR<AFConnectionData> pServer = mxTargetServerMap.First(); pServer != nullptr; pServer = mxTargetServerMap.Next())
    //    {
    //        if (pServer->net_client_ptr_ != nullptr)
    //        {
    //            pServer->net_client_ptr_->SendRawMsg(nMsgID, strData.data(), strData.length(), AFGUID(0), nPlayerID);
    //        }
    //    }
    //}

    //void AFCNetClientService::SendBySuit(const std::string& strHashKey, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    //{
    //    uint32_t nCRC32 = CRC32(strHashKey);
    //    SendBySuit(nCRC32, nMsgID, strData, nPlayerID);
    //}

    //void AFCNetClientService::SendBySuit(const std::string& strHashKey, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID)
    //{
    //    uint32_t nCRC32 = CRC32(strHashKey);
    //    SendBySuit(nCRC32, nMsgID, msg, nLen, nPlayerID);
    //}

    //void AFCNetClientService::SendBySuit(const int& nHashKey, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    //{
    //    SendBySuit(nHashKey, nMsgID, strData.c_str(), strData.length(), nPlayerID);
    //}

    //void AFCNetClientService::SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID)
    //{
    //    if (mxConsistentHash.Size() <= 0)
    //    {
    //        return;
    //    }

    //    AFCMachineNode xNode;

    //    if (!GetServerMachineData(ARK_LEXICAL_CAST<std::string>(nHashKey), xNode))
    //    {
    //        return;
    //    }

    //    SendByServerID(xNode.nMachineID, nMsgID, msg, nLen, nPlayerID);
    //}

    //void AFCNetClientService::SendSuitByPB(const std::string& strHashKey, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    //{
    //    uint32_t nCRC32 = CRC32(strHashKey);
    //    SendSuitByPB(nCRC32, nMsgID, xData, nPlayerID);
    //}

    //void AFCNetClientService::SendSuitByPB(const int& nHashKey, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    //{
    //    if (mxConsistentHash.Size() <= 0)
    //    {
    //        return;
    //    }

    //    AFCMachineNode xNode;

    //    if (!GetServerMachineData(ARK_LEXICAL_CAST<std::string>(nHashKey), xNode))
    //    {
    //        return;
    //    }

    //    SendToServerByPB(xNode.nMachineID, nMsgID, xData, nPlayerID);
    //}

    const ARK_SHARE_PTR<AFConnectionData>& AFCNetClientService::GetServerNetInfo(const int nServerID)
    {
        return target_servers_.GetElement(nServerID);
    }

    AFMapEx<int, AFConnectionData>& AFCNetClientService::GetServerList()
    {
        return target_servers_;
    }

    void AFCNetClientService::OnServerNotify(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len, const AFGUID& conn_id)
    {
        ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::msg_ss_server_notify);
        for (int i = 0; i < x_msg.server_list_size(); ++i)
        {
            const AFMsg::msg_ss_server_report& report = x_msg.server_list(i);
            if (!m_pBusModule->IsUndirectBusRelation(report.bus_id()))
            {
                continue;
            }

            //Create single cluster client with bus id and url
            m_pNetServiceManagerModule->CreateClusterClient(report.bus_id(), report.url());

            //管理为三个数值，channel zone proc
            AFBusAddr bus_addr(report.bus_id());
            bus_addr.inst_id = 0;//前三个数值相同表示同一个区

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