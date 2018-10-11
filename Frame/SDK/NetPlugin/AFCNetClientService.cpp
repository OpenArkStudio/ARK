#include "SDK/Core/AFDateTime.hpp"
#include "SDK/Interface/AFINetClientManagerModule.h"
#include "AFCTCPClient.h"
#include "AFCNetClientService.h"

namespace ark
{

    AFCNetClientService::AFCNetClientService(AFIPluginManager* p) : m_pPluginManager(p)
    {
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
        ProcessAddNetConnect();
    }

    void AFCNetClientService::Shutdown()
    {
        int id = 0;

        for (auto connection_data = mxTargetServerMap.First(id); connection_data != nullptr; connection_data = mxTargetServerMap.Next(id))
        {
            if (connection_data->net_client_ptr_ != nullptr)
            {
                ARK_DELETE(connection_data->net_client_ptr_); //shutdown in AFINet destructor function
            }
        }
    }

    bool AFCNetClientService::AddRecvCallback(const int nMsgID, const NET_PKG_RECV_FUNCTOR_PTR& cb)
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

    bool AFCNetClientService::AddRecvCallback(const NET_PKG_RECV_FUNCTOR_PTR& cb)
    {
        mxCallBackList.push_back(cb);

        return true;
    }

    bool AFCNetClientService::AddEventCallBack(const NET_EVENT_FUNCTOR_PTR& cb)
    {
        mxEventCallBackList.push_back(cb);

        return true;
    }

    void AFCNetClientService::ProcessUpdate()
    {
        for (bool bRet = mxTargetServerMap.Begin(); bRet; bRet = mxTargetServerMap.Increase())
        {
            ARK_SHARE_PTR<AFConnectionData>& connection_data = mxTargetServerMap.GetCurrentData();

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
                    if ((connection_data->_last_active_time + 30 * AFTimespan::SECOND_MS) >= m_pPluginManager->GetNowTime())
                    {
                        break;
                    }

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
        }
    }

    AFINet* AFCNetClientService::CreateNet(const proto_type proto)
    {
        if (proto == proto_type::tcp)
        {
            return ARK_NEW AFCTCPClient(this, &AFCNetClientService::OnRecvNetPack, &AFCNetClientService::OnSocketNetEvent);
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
        for (bool bRet = mxTargetServerMap.Begin(); bRet; bRet = mxTargetServerMap.Increase())
        {
            const auto& pServerData = mxTargetServerMap.GetCurrentData();
            std::string info = ARK_FORMAT("TargetBusID={} State={} url={}", pServerData->server_bus_id_, pServerData->net_state_, pServerData->endpoint_.to_string());
            LogServerInfo(info);
        }

        LogServerInfo("This is a client, end to print Server Info----------------------------------");
    }

    void AFCNetClientService::KeepAlive(ARK_SHARE_PTR<AFConnectionData>& pServerData)
    {
        if ((pServerData->_last_active_time + 10) > m_pPluginManager->GetNowTime())
        {
            return;
        }

        pServerData->_last_active_time = m_pPluginManager->GetNowTime();

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

    int AFCNetClientService::OnConnected(const NetEventType event, const AFGUID& conn_id, const std::string& ip, int bus_id)
    {
        ARK_SHARE_PTR<AFConnectionData> pServerInfo = GetServerNetInfo(bus_id);

        if (pServerInfo != nullptr)
        {
            AddServerWeightData(pServerInfo);
            pServerInfo->net_state_ = AFConnectionData::CONNECTED;

            AFINetClientManagerModule* net_client_manager_module = m_pPluginManager->FindModule<AFINetClientManagerModule>();
            if (net_client_manager_module != nullptr)
            {
                net_client_manager_module->AddNetConnectionBus(bus_id, pServerInfo->net_client_ptr_);
            }
            else
            {
                ARK_ASSERT_NO_EFFECT(net_client_manager_module != nullptr);
            }
        }

        return 0;
    }

    int AFCNetClientService::OnDisConnected(const NetEventType event, const AFGUID& conn_id, const std::string& ip, int bus_id)
    {
        ARK_SHARE_PTR<AFConnectionData> pServerInfo = GetServerNetInfo(bus_id);

        if (pServerInfo != nullptr)
        {
            RemoveServerWeightData(pServerInfo);
            pServerInfo->net_state_ = AFConnectionData::DISCONNECT;
            pServerInfo->_last_active_time = m_pPluginManager->GetNowTime();

            AFINetClientManagerModule* net_client_manager_module = m_pPluginManager->FindModule<AFINetClientManagerModule>();
            if (net_client_manager_module != nullptr)
            {
                net_client_manager_module->RemoveNetConnectionBus(bus_id);
            }
            else
            {
                ARK_ASSERT_NO_EFFECT(net_client_manager_module != nullptr);
            }
        }

        return 0;
    }

    void AFCNetClientService::ProcessAddNetConnect()
    {
        for (auto& iter : _tmp_nets)
        {
            const AFConnectionData& connection_data = iter;
            ARK_SHARE_PTR<AFConnectionData> target_connection_data = mxTargetServerMap.GetElement(connection_data.server_bus_id_);
            if (nullptr == target_connection_data)
            {
                //正常，添加新服务器
                target_connection_data = std::make_shared<AFConnectionData>();

                *target_connection_data = connection_data;
                target_connection_data->_last_active_time = m_pPluginManager->GetNowTime();

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

                if (!mxTargetServerMap.AddElement(target_connection_data->server_bus_id_, target_connection_data))
                {
                    //add log
                }
            }
        }

        _tmp_nets.clear();
    }

    void AFCNetClientService::OnRecvNetPack(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id)
    {
        auto it = mxRecvCallBack.find(msg_id);

        if (mxRecvCallBack.end() != it)
        {
            (*it->second)(head, msg_id, msg, msg_len, conn_id);
        }
        else
        {
            for (const auto& iter : mxCallBackList)
            {
                (*iter)(head, msg_id, msg, msg_len, conn_id);
            }
        }
    }

    void AFCNetClientService::OnSocketNetEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, int bus_id)
    {
        switch (event)
        {
        case CONNECTED:
            //Add net client service in manager module
            OnConnected(event, conn_id, ip, bus_id);
            break;
        case DISCONNECTED:
            //Remove net client service in manager module
            OnDisConnected(event, conn_id, ip, bus_id);
            break;
        default:
            break;
        }

        for (const auto& it : mxEventCallBackList)
        {
            (*it)(event, conn_id, ip, bus_id);
        }
    }

    void AFCNetClientService::SendByServerID(const int nServerID, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    {
        SendByServerID(nServerID, nMsgID, strData.c_str(), strData.length(), nPlayerID);
    }

    void AFCNetClientService::SendByServerID(const int nServerID, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID)
    {
        ARK_SHARE_PTR<AFConnectionData> pServer = mxTargetServerMap.GetElement(nServerID);
        if (pServer != nullptr)
        {
            if (pServer->net_client_ptr_ != nullptr)
            {
                pServer->net_client_ptr_->SendRawMsg(nMsgID, msg, nLen, 0, nPlayerID);
            }
        }
    }

    void AFCNetClientService::SendToAllServer(const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    {
        for (ARK_SHARE_PTR<AFConnectionData> pServer = mxTargetServerMap.First(); pServer != nullptr; pServer = mxTargetServerMap.Next())
        {
            if (pServer->net_client_ptr_ != nullptr)
            {
                pServer->net_client_ptr_->SendRawMsg(nMsgID, strData.data(), strData.size(), 0, nPlayerID);
            }
        }
    }

    void AFCNetClientService::SendToServerByPB(const int nServerID, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    {
        std::string strData;

        if (!xData.SerializeToString(&strData))
        {
            return;
        }

        ARK_SHARE_PTR<AFConnectionData> pServer = mxTargetServerMap.GetElement(nServerID);

        if (pServer)
        {
            if (pServer->net_client_ptr_ != nullptr)
            {
                pServer->net_client_ptr_->SendRawMsg(nMsgID, strData.data(), strData.length(), AFGUID(0), nPlayerID);
            }
        }
    }

    void AFCNetClientService::SendToAllServerByPB(const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    {
        std::string strData;

        if (!xData.SerializeToString(&strData))
        {
            return;
        }

        for (ARK_SHARE_PTR<AFConnectionData> pServer = mxTargetServerMap.First(); pServer != nullptr; pServer = mxTargetServerMap.Next())
        {
            if (pServer->net_client_ptr_ != nullptr)
            {
                pServer->net_client_ptr_->SendRawMsg(nMsgID, strData.data(), strData.length(), AFGUID(0), nPlayerID);
            }
        }
    }

    void AFCNetClientService::SendBySuit(const std::string& strHashKey, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    {
        uint32_t nCRC32 = CRC32(strHashKey);
        SendBySuit(nCRC32, nMsgID, strData, nPlayerID);
    }

    void AFCNetClientService::SendBySuit(const std::string& strHashKey, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID)
    {
        uint32_t nCRC32 = CRC32(strHashKey);
        SendBySuit(nCRC32, nMsgID, msg, nLen, nPlayerID);
    }

    void AFCNetClientService::SendBySuit(const int& nHashKey, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    {
        SendBySuit(nHashKey, nMsgID, strData.c_str(), strData.length(), nPlayerID);
    }

    void AFCNetClientService::SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID)
    {
        if (mxConsistentHash.Size() <= 0)
        {
            return;
        }

        AFCMachineNode xNode;

        if (!GetServerMachineData(ARK_LEXICAL_CAST<std::string>(nHashKey), xNode))
        {
            return;
        }

        SendByServerID(xNode.nMachineID, nMsgID, msg, nLen, nPlayerID);
    }

    void AFCNetClientService::SendSuitByPB(const std::string& strHashKey, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    {
        uint32_t nCRC32 = CRC32(strHashKey);
        SendSuitByPB(nCRC32, nMsgID, xData, nPlayerID);
    }

    void AFCNetClientService::SendSuitByPB(const int& nHashKey, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    {
        if (mxConsistentHash.Size() <= 0)
        {
            return;
        }

        AFCMachineNode xNode;

        if (!GetServerMachineData(ARK_LEXICAL_CAST<std::string>(nHashKey), xNode))
        {
            return;
        }

        SendToServerByPB(xNode.nMachineID, nMsgID, xData, nPlayerID);
    }

    const ARK_SHARE_PTR<AFConnectionData>& AFCNetClientService::GetServerNetInfo(const int nServerID)
    {
        return mxTargetServerMap.GetElement(nServerID);
    }

    AFMapEx<int, AFConnectionData>& AFCNetClientService::GetServerList()
    {
        return mxTargetServerMap;
    }

}