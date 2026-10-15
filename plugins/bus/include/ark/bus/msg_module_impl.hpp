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

#include "ark/bus/bus_module.hpp"
#include "ark/bus/msg_module.hpp"
#include "ark/net/net_service_manager_module.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

class msg_module_impl final : public msg_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;

    virtual bool send_def_cs_msg_by_app_type(
        const app_type app_type, const msg_id_t msg_id, const google::protobuf::Message& msg) override;
    virtual bool send_def_ss_msg_by_app_type(
        const app_type app_type, const msg_id_t msg_id, const google::protobuf::Message& msg) override;
    virtual bool send_msg_by_app_type(
        const app_type app_type, msg_header* header, const google::protobuf::Message& msg) override;
    bool send_msg_by_bus_id(bus_id_t bus_id, msg_id_t msg_id, const google::protobuf::Message& msg,
        const guid_t& guid = null_guid) override;
    bool send_to_client(conv_id_t session_id, msg_id_t msg_id, const google::protobuf::Message& msg,
        const guid_t& guid = null_guid) override;
    bool send_to_client(conv_id_t session_id, msg_id_t msg_id, const std::string& msg_data, bool compressed = false,
        const guid_t& guid = null_guid) override;
    bool send_to_intra(conv_id_t session_id, msg_id_t msg_id, const google::protobuf::Message& msg,
        const guid_t& guid = null_guid) override;

protected:
    bool send_msg(std::shared_ptr<net>& net_ptr, msg_header* header, const google::protobuf::Message& msg,
        const conv_id_t session_id = 0);
    bool send_msg(
        std::shared_ptr<net>& net_ptr, msg_header* header, const std::string& msg_data, const conv_id_t session_id = 0);

private:
    net_service_manager_module* net_service_manager_module_;
    bus_module* bus_module_;
};

} // namespace ark
