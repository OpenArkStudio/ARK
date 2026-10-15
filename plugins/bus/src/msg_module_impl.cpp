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

#include "ark/bus/msg_module_impl.hpp"
#include "ark/misc.hpp"

namespace ark {

bool msg_module_impl::init() {
    m_pBusModule = find_module<bus_module>();
    m_pNetServiceManagerModule = find_module<net_service_manager_module>();

    return true;
}

//bool AFCMsgModule::SendSuitSSMsg(const uint8_t app_type, const std::string& hash_key, msg_id_t msg_id,
//    const google::protobuf::Message& msg, const guid_t& actor_id /* = 0*/)
//{
//    uint32_t crc32 = AFCRC::crc32(hash_key);
//    return SendSuitSSMsg(app_type, crc32, msg_id, msg, actor_id);
//}
//
//bool AFCMsgModule::SendSuitSSMsg(const uint8_t app_type, const uint32_t& hash_value, msg_id_t msg_id,
//    const google::protobuf::Message& msg, const guid_t& actor_id /* = 0*/)
//{
//    int suit_bus_id = 0; // GetSuitBus(app_type, hash_value);
//    return SendSSMsg(suit_bus_id, msg_id, msg, actor_id);
//}
//
//bool AFCMsgModule::SendParticularSSMsg(bus_id_t bus_id, msg_id_t msg_id, const google::protobuf::Message& msg,
//    conv_id_t conn_id, const guid_t& actor_id /* = 0*/)
//{
//    std::string msg_data = msg.SerializeAsString();
//    return SendSSMsg(bus_id, msg_id, msg, conn_id, actor_id);
//}
//
////////////////////////////////////////////////////////////////////////////
//
//bool AFCMsgModule::SendSSMsg(bus_id_t target_bus, msg_id_t msg_id, const google::protobuf::Message& msg,
//    conv_id_t conn_id, const guid_t& actor_id /* = 0*/)
//{
//    std::string msg_data;
//    ARK_ASSERT_RET_VAL(msg.SerializeToString(&msg_data), false);
//
//    auto src_bus = m_pBusModule->GetSelfBusID();
//#ifdef ARK_RUN_MODE_DEBUG
//    std::string pb_json;
//    auto status = google::protobuf::util::MessageToJsonString(msg, &pb_json);
//    if (!status.ok())
//    {
//        return false;
//    }
//
//    ARK_LOG_DEBUG("Send msg log\nsrc={}\ndst={}\nmsg_name={}\nmsg_id={}\nmsg_len={}\nmsg_data={}",
//        AFMisc::Bus2Str(src_bus), AFMisc::Bus2Str(target_bus), msg.GetTypeName(), msg_id, msg_data.length(), pb_json);
//#endif
//    return SendSSMsg(src_bus, target_bus, msg_id, msg_data.c_str(), msg_data.length(), conn_id, actor_id);
//}
//
//bool AFCMsgModule::SendSSMsg(bus_id_t target_bus, msg_id_t msg_id, const char* msg, const int msg_len,
//    conv_id_t conn_id, const guid_t& actor_id /*= 0*/)
//{
//    return SendSSMsg(m_pBusModule->GetSelfBusID(), target_bus, msg_id, msg, msg_len, conn_id, actor_id);
//}
//
//bool AFCMsgModule::SendSSMsg(bus_id_t src_bus, bus_id_t target_bus, msg_id_t msg_id, const char* msg_data,
//    const int msg_len, conv_id_t session_id, const guid_t& actor_id /* = 0*/)
//{
//    AFSSMsgHead head;
//    head.id_ = msg_id;
//    head.length_ = msg_len;
//    head.actor_id_ = actor_id;
//    head.src_bus_ = src_bus;
//    head.dst_bus_ = target_bus;
//
//    // TODO: NickYang
//    //AFINet* net_ptr = m_pNetServiceManagerModule->GetNetConnectionBus(src_bus, target_bus);
//    //if (net_ptr != nullptr)
//    //{
//    //    return net_ptr->SendMsg(&head, msg_data, session_id);
//    //}
//
//    ARK_LOG_ERROR("send ss msg error, src_bus={} target_bus={} msg_id={} conn_id={} target_role_id={}", src_bus,
//        target_bus, msg_id, session_id, actor_id);
//    return false;
//}
//
//bool AFCMsgModule::SendSSMsgByRouter(const AFSSMsgHead& head)
//{
//    // TODO: will add later
//    return true;
//}

bool msg_module_impl::send_def_cs_msg_by_app_type(
    const app_type app_type, const msg_id_t msg_id, const google::protobuf::Message& msg) {
    s_msg_header head;
    head.set_message_id(msg_id);
    return send_msg_by_app_type(app_type, &head, msg);
}

bool msg_module_impl::send_def_ss_msg_by_app_type(
    const app_type app_type, const msg_id_t msg_id, const google::protobuf::Message& msg) {
    ss_msg_header head;
    head.set_message_id(msg_id);
    return send_msg_by_app_type(app_type, &head, msg);
}

bool msg_module_impl::send_msg_by_app_type(
    const app_type app_type, msg_header* header, const google::protobuf::Message& msg) {
    /// firstly find net client service
    auto pClientService = m_pNetServiceManagerModule->get_client_service(app_type);
    if (pClientService == nullptr)
    {
        ARK_LOG_ERROR("net client service not be found, src_bus={} app_type={} msg_id={} actor_id={}",
            m_pBusModule->get_self_bus_id(), (uint8_t)app_type, header->message_id(), header->actor_id());
        return false;
    }

    /// todo : if no client service then try net service

    auto busid = m_pBusModule->get_self_bus_id();
    auto pConnetionInfo = pClientService->get_suitable_connect(misc::to_string(busid));
    if (pConnetionInfo == nullptr)
    {
        ARK_LOG_ERROR("connection info not be found, src_bus={} app_type={} msg_id={} actor_id={}", busid,
            (uint8_t)app_type, header->message_id(), header->actor_id());
        return false;
    }

    header->set_source_bus_id(busid);
    header->set_dest_bus_id(pConnetionInfo->server_bus_id_);
    return send_msg(pConnetionInfo->net_client_, header, msg);
}

bool msg_module_impl::send_msg_by_bus_id(
    bus_id_t bus_id, msg_id_t msg_id, const google::protobuf::Message& msg, const guid_t& guid /* = NULL_GUID*/) {
    auto src_bus = m_pBusModule->get_self_bus_id();
    auto pNet = m_pNetServiceManagerModule->get_net_connection_bus(src_bus, bus_id);

    ss_msg_header header;
    header.set_message_id(msg_id);
    header.set_actor_id(guid);
    header.set_source_bus_id(src_bus);
    header.set_dest_bus_id(bus_id);
    return send_msg(pNet, &header, msg);
}

bool msg_module_impl::send_to_client(
    conv_id_t session_id, msg_id_t msg_id, const google::protobuf::Message& msg, const guid_t& guid) {
    std::string msg_data;
    if (!msg.SerializeToString(&msg_data))
    {
        ARK_LOG_ERROR("msg serializing failed, src_bus={} target_bus={} msg_id={} actor_id={}", msg_id, guid);
        return false;
    }

    return send_to_client(session_id, msg_id, msg_data, false, guid);
}

bool msg_module_impl::send_to_client(conv_id_t session_id, msg_id_t msg_id, const std::string& msg_data,
    bool compressed /* = false*/, const guid_t& guid) {
    auto pNetService = m_pNetServiceManagerModule->get_inter_net_server();
    if (pNetService == nullptr)
    {
        ARK_LOG_ERROR("net service is null, src_bus={} session id ={} msg_id={} actor_id={}",
            m_pBusModule->get_self_bus_id(), session_id, msg_id, guid);
        return false;
    }

    s_msg_header header;
    header.set_message_id(msg_id);
    header.set_compressed(compressed);
    auto pNet = pNetService->get_net();
    return send_msg(pNet, &header, msg_data, session_id);
}

bool msg_module_impl::send_to_intra(
    conv_id_t session_id, msg_id_t msg_id, const google::protobuf::Message& msg, const guid_t& guid) {
    auto pNetService = m_pNetServiceManagerModule->get_intra_net_server();
    if (pNetService == nullptr)
    {
        ARK_LOG_ERROR("net service is null, src_bus={} session id ={} msg_id={} actor_id={}",
            m_pBusModule->get_self_bus_id(), session_id, msg_id, guid);
        return false;
    }

    auto pNet = pNetService->get_net();
    ss_msg_header header;
    header.set_message_id(msg_id);
    header.set_actor_id(guid);
    header.set_source_bus_id(m_pBusModule->get_self_bus_id());
    header.set_dest_bus_id(0);
    return send_msg(pNet, &header, msg, session_id);
}

bool msg_module_impl::send_msg(
    std::shared_ptr<net>& pNet, msg_header* header, const google::protobuf::Message& msg, const conv_id_t session_id) {
    std::string msg_data;
    if (!msg.SerializeToString(&msg_data))
    {
        ARK_LOG_ERROR("msg serializing failed, src_bus={} target_bus={} msg_id={} actor_id={}", header->source_bus_id(),
            header->dest_bus_id(), header->message_id(), header->actor_id());
        return false;
    }

    return send_msg(pNet, header, msg_data, session_id);
}

bool msg_module_impl::send_msg(
    std::shared_ptr<net>& pNet, msg_header* header, const std::string& msg_data, const conv_id_t session_id) {
    if (pNet == nullptr)
    {
        ARK_LOG_ERROR("net is invalid, src_bus={} target_bus={} msg_id={} actor_id={}", header->source_bus_id(),
            header->dest_bus_id(), header->message_id(), header->actor_id());
        return false;
    }

    auto conv_id = session_id;
    if (conv_id == 0)
    {
        conv_id = m_pNetServiceManagerModule->get_session_id(header->dest_bus_id());
    }

    ARK_LOG_DEBUG(
        "[msg][ack] msg id = {} session id = {} msg len = {}", header->message_id(), conv_id, msg_data.length());

    header->set_message_length(msg_data.size());
    return pNet->send_msg(header, msg_data.data(), conv_id);
}

} // namespace ark
