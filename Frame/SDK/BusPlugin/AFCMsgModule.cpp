#include "SDK/Interface/AFIPluginManager.h"
#include "AFCMsgModule.h"

namespace ark
{

    bool AFCMsgModule::Init()
    {
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();

        return true;
    }

    bool AFCMsgModule::SendSuitSSMsg(const uint8_t app_type, const std::string& hash_key, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id/* = 0*/)
    {
        uint32_t crc32 = CRC32(hash_key);
        return SendSuitSSMsg(app_type, crc32, msg_id, msg, target_role_id);
    }

    bool AFCMsgModule::SendSuitSSMsg(const uint8_t app_type, const uint32_t& hash_value, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id/* = 0*/)
    {
        //TODO
        int suit_bus_id = 0;// GetSuitBus(app_type, hash_value);
        return SendSSMsg(suit_bus_id, msg_id, msg, target_role_id);
    }

    bool AFCMsgModule::SendParticularSSMsg(const int bus_id, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id/* = 0*/)
    {
        std::string msg_data = msg.SerializeAsString();
        return SendSSMsg(bus_id, msg_id, msg, target_role_id);
    }

    bool AFCMsgModule::SendSSMsg(const int target_bus, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id)
    {
        std::string msg_data;
        ARK_ASSERT_RET_VAL(msg.SerializeToString(&msg_data), false);

        return SendSSMsg(m_pBusModule->GetSelfBusID(), target_bus, msg_id, msg_data.c_str(), msg_data.length(), target_role_id);
    }

    bool AFCMsgModule::SendSSMsg(const int target_bus, const int msg_id, const char* msg, const int msg_len, const AFGUID& target_role_id /*= 0*/)
    {
        return SendSSMsg(m_pBusModule->GetSelfBusID(), target_bus, msg_id, msg, msg_len, target_role_id);
    }

    bool AFCMsgModule::SendSSMsg(const int src_bus, const int target_bus, const int msg_id, const char* msg, const int msg_len, const AFGUID& target_role_id/* = 0*/)
    {
        //TODO:
        //根据busid来确定转发到哪里去/直接发送到哪里去

        //src_bus -> target_bus
        //m_pBusModule->IsUndirectBusRelation();

        //check who is the server side
        //if ()
        //{
        //}

        //m_pNetClientManagerModule->GetNetClientService(m_pBusModule->GetAppTypeByBus(src_bus));

        return true;
    }

}