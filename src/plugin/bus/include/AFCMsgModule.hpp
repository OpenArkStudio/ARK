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

#pragma once

#include "base/AFPluginManager.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "log/interface/AFILogModule.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "bus/interface/AFIMsgModule.hpp"

namespace ark {

class AFCMsgModule : public AFIMsgModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;

    bool SendSuitSSMsg(const uint8_t app_type, const std::string& hash_key, const int msg_id,
        const google::protobuf::Message& msg, const AFGUID& actor_id = 0) override;
    bool SendSuitSSMsg(const uint8_t app_type, const uint32_t& hash_value, const int msg_id,
        const google::protobuf::Message& msg, const AFGUID& actor_id = 0) override;
    bool SendParticularSSMsg(const int bus_id, const int msg_id, const google::protobuf::Message& msg,
        const AFGUID& conn_id, const AFGUID& actor_id = 0) override;

    bool SendSSMsg(const int src_bus, const int target_bus, const int msg_id, const char* msg, const int msg_len,
        const AFGUID& conn_id, const AFGUID& actor_id = 0) override;
    bool SendSSMsg(const int target_bus, const int msg_id, const google::protobuf::Message& msg, const AFGUID& conn_id,
        const AFGUID& actor_id = 0) override;
    bool SendSSMsg(const int target_bus, const int msg_id, const char* msg, const int msg_len, const AFGUID& conn_id,
        const AFGUID& actor_id = 0) override;

    bool SendSSMsgByRouter(const AFSSMsgHead& head) override;

private:
    AFINetServiceManagerModule* m_pNetServiceManagerModule;
    AFIBusModule* m_pBusModule;
    AFILogModule* m_pLogModule;
};

} // namespace ark
