/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
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

#include "SDK/Interface/AFINetClientManagerModule.h"
#include "SDK/Interface/AFINetServerManagerModule.h"
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFIMsgModule.h"

class AFCMsgModule : public AFIMsgModule
{
public:
    explicit AFCMsgModule() = default;

    bool Init() override;

    bool SendSuitSSMsg(const uint8_t app_type, const std::string& hash_key, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id = 0) override;
    bool SendSuitSSMsg(const uint8_t app_type, const uint32_t& hash_value, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id = 0) override;
    bool SendParticularSSMsg(const int bus_id, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id = 0) override;

    bool SendSSMsg(const int src_bus, const int target_bus, const int msg_id, const char* msg, const int msg_len, const AFGUID& target_role_id = 0) override;
    bool SendSSMsg(const int target_bus, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id = 0) override;
    bool SendSSMsg(const int target_bus, const int msg_id, const char* msg, const int msg_len, const AFGUID& target_role_id = 0) override;

private:
    AFIBusModule* m_pBusModule;
};