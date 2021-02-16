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

#pragma once

#include "base/AFPluginManager.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "bus/interface/AFIMsgModule.hpp"

namespace ark {

class AFCMsgModule final : public AFIMsgModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;

    //bool SendSuitSSMsg(const uint8_t app_type, const std::string& hash_key, const msg_id_t msg_id,
    //    const google::protobuf::Message& msg, const guid_t& actor_id = 0) override;
    //bool SendSuitSSMsg(const uint8_t app_type, const uint32_t& hash_value, msg_id_t msg_id,
    //    const google::protobuf::Message& msg, const guid_t& actor_id = 0) override;
    //bool SendParticularSSMsg(bus_id_t bus_id, msg_id_t msg_id, const google::protobuf::Message& msg,
    //    const guid_t& conn_id, const guid_t& actor_id = 0) override;

    //bool SendSSMsg(bus_id_t src_bus, bus_id_t target_bus, msg_id_t msg_id, const char* msg, const int msg_len,
    //    const guid_t& conn_id, const guid_t& actor_id = 0) override;
    //bool SendSSMsg(bus_id_t target_bus, msg_id_t msg_id, const google::protobuf::Message& msg, const guid_t& conn_id,
    //    const guid_t& actor_id = 0) override;
    //bool SendSSMsg(bus_id_t target_bus, msg_id_t msg_id, const char* msg, const int msg_len, const guid_t& conn_id,
    //    const guid_t& actor_id = 0) override;

    //bool SendSSMsgByRouter(const AFSSMsgHead& head) override;

    //--------new interface---------
    virtual bool SendDefCSMsgByAppType(const ARK_APP_TYPE app_type, const msg_id_t msg_id, const google::protobuf::Message& msg) override;
    virtual bool SendDefSSMsgByAppType(const ARK_APP_TYPE app_type, const msg_id_t msg_id, const google::protobuf::Message& msg) override;
    virtual bool SendMsgByAppType(const ARK_APP_TYPE app_type, AFIMsgHeader* header, const google::protobuf::Message& msg) override;
    bool SendMsgByBusID(bus_id_t bus_id, msg_id_t msg_id, const google::protobuf::Message& msg,
        const guid_t& guid = NULL_GUID) override;
    bool SendToClient(conv_id_t session_id, msg_id_t msg_id, const google::protobuf::Message& msg,
        const guid_t& guid = NULL_GUID) override;
    bool SendToClient(conv_id_t session_id, msg_id_t msg_id, const std::string& msg_data, bool compressed = false,
        const guid_t& guid = NULL_GUID) override;
    bool SendToIntra(conv_id_t session_id, msg_id_t msg_id, const google::protobuf::Message& msg,
        const guid_t& guid = NULL_GUID) override;

protected:
    bool SendMsg(std::shared_ptr<AFINet>& pNet, AFIMsgHeader* header, const google::protobuf::Message& msg, const conv_id_t session_id = 0);
    bool SendMsg(std::shared_ptr<AFINet>& pNet, AFIMsgHeader* header, const std::string& msg_data, const conv_id_t session_id = 0);

private:
    AFINetServiceManagerModule* m_pNetServiceManagerModule;
    AFIBusModule* m_pBusModule;
};

} // namespace ark
