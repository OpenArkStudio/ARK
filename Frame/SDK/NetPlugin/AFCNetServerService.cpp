#include "SDK/Interface/AFIPluginManager.h"
#include "AFCTCPServer.h"
#include "AFCNetServerService.h"

namespace ark
{

    AFCNetServerService::AFCNetServerService(AFIPluginManager* p) :
        m_pPluginManager(p)
    {
        m_pNetServiceManagerModule = m_pPluginManager->FindModule<AFINetServiceManagerModule>();
        m_pLogModule = m_pPluginManager->FindModule<AFILogModule>();
        m_pMsgModule = m_pPluginManager->FindModule<AFIMsgModule>();

        ARK_ASSERT_NO_EFFECT(m_pNetServiceManagerModule != nullptr &&
                             m_pLogModule != nullptr &&
                             m_pMsgModule != nullptr);
    }

    AFCNetServerService::~AFCNetServerService()
    {
        ARK_DELETE(m_pNet);
    }

    bool AFCNetServerService::Start(const AFHeadLength len, const int bus_id, const AFEndpoint& ep, const uint8_t thread_count, const uint32_t max_connection)
    {
        bool ret = false;
        if (ep.proto() == proto_type::tcp)
        {
            m_pNet = ARK_NEW AFCTCPServer(this, &AFCNetServerService::OnNetMsg, &AFCNetServerService::OnNetEvent);
            ret = m_pNet->StartServer(len, bus_id, ep.ip(), ep.port(), thread_count, max_connection, ep.is_v6());

            AFINetServerService::RegMsgCallback(AFMsg::E_SS_MSG_ID_SERVER_REPORT, this, &AFCNetServerService::OnClientRegister);
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

        return ret;
    }

    bool AFCNetServerService::Update()
    {
        ARK_ASSERT_RET_VAL(m_pNet != nullptr, false);
        m_pNet->Update();
        return true;
    }

    AFINet* AFCNetServerService::GetNet()
    {
        return m_pNet;
    }

    bool AFCNetServerService::RegMsgCallback(const int nMsgID, const NET_PKG_RECV_FUNCTOR_PTR& cb)
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

    bool AFCNetServerService::RegForwardMsgCallback(const NET_PKG_RECV_FUNCTOR_PTR& cb)
    {
        net_forward_msg_callbacks_.push_back(cb);
        return true;
    }

    bool AFCNetServerService::RegNetEventCallback(const NET_EVENT_FUNCTOR_PTR& cb)
    {
        net_event_callbacks_.push_back(cb);
        return true;
    }

    void AFCNetServerService::OnNetMsg(const AFNetMsg* msg)
    {
        auto it = net_msg_callbacks_.find(msg->id_);
        if (it != net_msg_callbacks_.end())
        {
            (*it->second)(msg);
        }
        else
        {
            //TODO:forward to other server process

            ARK_LOG_ERROR("Invalid message, id = {}", msg->id_);

            //for (const auto& iter : mxCallBackList)
            //{
            //    (*iter)(head, msg_id, msg, msg_len, conn_id);
            //}
        }
    }

    void AFCNetServerService::OnNetEvent(const AFNetEvent* event)
    {
        switch (event->type_)
        {
        case CONNECTED:
            ARK_LOG_INFO("Connected server = {} succenssfully, ip = {}, conn_id = {}", AFBusAddr(event->bus_id_).ToString(), event->ip_, event->id_);
            break;
        case DISCONNECTED:
            ARK_LOG_ERROR("Disconnected server = {} succenssfully, ip = {}, conn_id = {}", AFBusAddr(event->bus_id_).ToString(), event->ip_, event->id_);
            m_pNetServiceManagerModule->RemoveNetConnectionBus(event->bus_id_);
            break;
        default:
            break;
        }

        for (const auto& it : net_event_callbacks_)
        {
            (*it)(event);
        }
    }

    void AFCNetServerService::OnClientRegister(const AFNetMsg* msg)
    {
        //ARK_PROCESS_MSG(msg, AFMsg::msg_ss_server_report);

        ////Add server_bus_id -> client_bus_id relationship with net
        //m_pNetServiceManagerModule->AddNetConnectionBus(x_msg.bus_id(), m_pNet);
        ////////////////////////////////////////////////////////////////////////////
        //ARK_SHARE_PTR<AFServerData> server_data_ptr = reg_clients_.GetElement(x_msg.bus_id());
        //if (nullptr == server_data_ptr)
        //{
        //    server_data_ptr = std::make_shared<AFServerData>();
        //    reg_clients_.AddElement(x_msg.bus_id(), server_data_ptr);
        //}

        //server_data_ptr->Init(conn_id, x_msg);
        ////////////////////////////////////////////////////////////////////////////

        //SyncToAllClient(x_msg.bus_id(), conn_id);
    }

    void AFCNetServerService::SyncToAllClient(const int bus_id, const AFGUID& conn_id)
    {
        AFMsg::msg_ss_server_notify msg;

        reg_clients_.DoEveryElement([&](AFMapEx<int, AFServerData>::PTRTYPE & server_data)
        {
            AFMsg::msg_ss_server_report* report = msg.add_server_list();
            *report = server_data->server_info_;
            return true;
        });

        m_pMsgModule->SendSSMsg(bus_id, AFMsg::E_SS_MSG_ID_SERVER_NOTIFY, msg, conn_id);
    }
}