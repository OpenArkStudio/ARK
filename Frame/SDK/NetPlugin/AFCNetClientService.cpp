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

    bool AFCNetClientService::StartClient(const AFHeadLength head_len, const int& target_bus_id, const AFEndpoint& endpoint)
    {
        AFConnectionData data;
        data.head_len_ = head_len;
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
                    bool ret = connection_data->net_client_ptr_->StartClient(connection_data->head_len_, connection_data->server_bus_id_, connection_data->endpoint_.ip(), connection_data->endpoint_.port(), connection_data->endpoint_.is_v6());
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
        ARK_LOG_INFO("Disconnect [{}] successfully, ip={} session_id={}", AFBusAddr(event->bus_id_).ToString(), event->ip_, event->id_);

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
        msg.set_url(server_config->public_ep_.to_string());
        msg.set_max_online(server_config->max_connection);
        msg.set_logic_status(AFMsg::E_ST_NARMAL);

        m_pMsgModule->SendParticularSSMsg(bus_id, AFMsg::E_SS_MSG_ID_SERVER_REPORT, msg, session_id);
        ARK_LOG_INFO("Register self server_id = {}, target_id = {}", server_config->self_id, bus_id);
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
                int ret = target_connection_data->net_client_ptr_->StartClient(target_connection_data->head_len_, target_connection_data->server_bus_id_, target_connection_data->endpoint_.ip(), target_connection_data->endpoint_.port());
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

    const ARK_SHARE_PTR<AFConnectionData>& AFCNetClientService::GetServerNetInfo(const int nServerID)
    {
        return target_servers_.GetElement(nServerID);
    }

    AFMapEx<int, AFConnectionData>& AFCNetClientService::GetServerList()
    {
        return target_servers_;
    }

    void AFCNetClientService::OnServerNotify(const AFNetMsg* msg, const int64_t session_id)
    {
        //ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::msg_ss_server_notify);
        //for (int i = 0; i < x_msg.server_list_size(); ++i)
        //{
        //    const AFMsg::msg_ss_server_report& report = x_msg.server_list(i);
        //    if (!m_pBusModule->IsUndirectBusRelation(report.bus_id()))
        //    {
        //        continue;
        //    }

        //    //Create single cluster client with bus id and url
        //    m_pNetServiceManagerModule->CreateClusterClient(report.bus_id(), report.url());

        //    //管理为三个数值，channel zone proc
        //    AFBusAddr bus_addr(report.bus_id());
        //    bus_addr.inst_id = 0;//前三个数值相同表示同一个区

        //    auto iter = reg_servers_.find(bus_addr.bus_id);
        //    if (iter != reg_servers_.end())
        //    {
        //        iter->second.insert(std::make_pair(report.bus_id(), report));
        //    }
        //    else
        //    {
        //        std::map<int, AFMsg::msg_ss_server_report> others;
        //        others.insert(std::make_pair(report.bus_id(), report));
        //        reg_servers_.insert(std::make_pair(bus_addr.bus_id, others));
        //    }
        //}
    }

}