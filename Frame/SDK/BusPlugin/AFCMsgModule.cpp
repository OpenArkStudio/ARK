#include "SDK/Interface/AFIPluginManager.h"
#include "AFCMsgModule.h"

namespace ark
{

    bool AFCMsgModule::Init()
    {
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pNetClientManagerModule = pPluginManager->FindModule<AFINetClientManagerModule>();
        m_pNetServerManagerModule = pPluginManager->FindModule<AFINetServerManagerModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();

        return true;
    }

    bool AFCMsgModule::SendSuitSSMsg(const uint8_t app_type, const std::string& hash_key, const int msg_id, const google::protobuf::Message& msg, const AFGUID& actor_id/* = 0*/)
    {
        uint32_t crc32 = CRC32(hash_key);
        return SendSuitSSMsg(app_type, crc32, msg_id, msg, actor_id);
    }

    bool AFCMsgModule::SendSuitSSMsg(const uint8_t app_type, const uint32_t& hash_value, const int msg_id, const google::protobuf::Message& msg, const AFGUID& actor_id/* = 0*/)
    {
        int suit_bus_id = 0; //GetSuitBus(app_type, hash_value);
        return SendSSMsg(suit_bus_id, msg_id, msg, actor_id);
    }

    bool AFCMsgModule::SendParticularSSMsg(const int bus_id, const int msg_id, const google::protobuf::Message& msg, const AFGUID& actor_id/* = 0*/)
    {
        std::string msg_data = msg.SerializeAsString();
        return SendSSMsg(bus_id, msg_id, msg, actor_id);
    }

    bool AFCMsgModule::SendSSMsg(const int target_bus, const int msg_id, const google::protobuf::Message& msg, const AFGUID& actor_id)
    {
        std::string msg_data;
        ARK_ASSERT_RET_VAL(msg.SerializeToString(&msg_data), false);

        return SendSSMsg(m_pBusModule->GetSelfBusID(), target_bus, msg_id, msg_data.c_str(), msg_data.length(), actor_id);
    }

    bool AFCMsgModule::SendSSMsg(const int target_bus, const int msg_id, const char* msg, const int msg_len, const AFGUID& actor_id /*= 0*/)
    {
        return SendSSMsg(m_pBusModule->GetSelfBusID(), target_bus, msg_id, msg, msg_len, actor_id);
    }

    bool AFCMsgModule::SendSSMsg(const int src_bus, const int target_bus, const int msg_id, const char* msg, const int msg_len, const AFGUID& conn_id/* = 0*/, const AFGUID& actor_id/* = 0*/)
    {
        AFINet* client_service_ptr = m_pNetClientManagerModule->GetNetConnectionBus(src_bus, target_bus);
        if (client_service_ptr != nullptr)
        {
            return client_service_ptr->SendRawMsg(msg_id, msg, msg_len, conn_id, actor_id);
        }

        AFINet* server_service_ptr = m_pNetServerManagerModule->GetNetConnectionBus(src_bus, target_bus);
        if (server_service_ptr != nullptr)
        {
            return server_service_ptr->SendRawMsg(msg_id, msg, msg_len, conn_id, actor_id);
        }

        ARK_LOG_ERROR("send ss msg error, src_bus={} target_bus={} msg_id={} conn_id={} target_role_id={}", src_bus, target_bus, msg_id, conn_id.ToString(), actor_id.ToString());
        return false;
    }

}