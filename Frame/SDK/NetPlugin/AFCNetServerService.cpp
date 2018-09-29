#include "SDK/Interface/AFIMsgModule.h"
#include "AFCTCPServer.h"
#include "AFCNetServerService.h"

namespace ark
{

    AFCNetServerService::~AFCNetServerService()
    {
        if (m_pNet)
        {
            ARK_DELETE(m_pNet);
        }
    }

    bool AFCNetServerService::Start(const int bus_id, const AFEndpoint& ep, const uint8_t thread_count, const uint32_t max_connection)
    {
        if (ep.proto() == proto_type::tcp)
        {
            m_pNet = ARK_NEW AFCTCPServer(this, &AFCNetServerService::OnRecvNetPack, &AFCNetServerService::OnSocketNetEvent);
            return m_pNet->Start(bus_id, ep.ip(), ep.port(), thread_count, max_connection, ep.is_v6());
        }
        else if (ep.proto() == proto_type::udp)
        {
            //for now, do not support udp server
        }
        else if (ep.proto() == proto_type::ws)
        {
            //will add websocket server
        }
        else
        {
            //other protocol will be supported
            return false;
        }

        return false;
    }

    bool AFCNetServerService::Update()
    {
        ARK_ASSERT_RET_VAL(m_pNet != nullptr, false);
        m_pNet->Update();
        return true;
    }

    bool AFCNetServerService::SendBroadcastMsg(const int msg_id, const std::string& msg, const AFGUID& player_id)
    {
        return m_pNet->SendMsgToAllClient(msg.c_str(), msg.length(), player_id);
    }

    bool AFCNetServerService::SendBroadcastPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& player_id)
    {
        std::string send_data;
        if (!pb_msg.SerializeToString(&send_data))
        {
            return false;
        }

        return SendBroadcastMsg(msg_id, send_data, player_id);
    }

    bool AFCNetServerService::SendPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list/* = nullptr*/)
    {
        std::string send_data;
        if (!pb_msg.SerializeToString(&send_data))
        {
            return false;
        }

        return SendMsg(msg_id, send_data, connect_id, player_id, target_list);
    }

    bool AFCNetServerService::SendMsg(const uint16_t msg_id, const std::string& data, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list/* = nullptr*/)
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

            return m_pNet->SendRawMsg(AFMsg::EGMI_GTG_BROCASTMSG, data.data(), data.size(), connect_id, player_id);
        }
        else
        {
            return m_pNet->SendRawMsg(msg_id, data.data(), data.size(), connect_id, player_id);
        }
    }

    AFINet* AFCNetServerService::GetNet()
    {
        return m_pNet;
    }

    bool AFCNetServerService::AddRecvCallback(const int nMsgID, const NET_PKG_RECV_FUNCTOR_PTR& cb)
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

    bool AFCNetServerService::AddRecvCallback(const NET_PKG_RECV_FUNCTOR_PTR& cb)
    {
        mxCallBackList.push_back(cb);

        return true;
    }

    bool AFCNetServerService::AddEventCallBack(const NET_EVENT_FUNCTOR_PTR& cb)
    {
        mxEventCallBackList.push_back(cb);

        return true;
    }

    void AFCNetServerService::OnRecvNetPack(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID)
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

    void AFCNetServerService::OnSocketNetEvent(const NetEventType event, const AFGUID& connect_id, int bus_id)
    {
        for (const auto& it : mxEventCallBackList)
        {
            (*it)(event, connect_id, bus_id);
        }
    }

}