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

#include "ark/kernel/data_impl.hpp"
#include "ark/kernel/entity.hpp"
#include "ark/kernel/node_impl.hpp"
#include "ark/kernel/table_impl.hpp"
#include "ark/math/vector3d.hpp"
#include "ark/module.hpp"
#include "ark/net/net_msg.hpp"
#include "ark/proto.hpp"

namespace ark {

class msg_module : public module
{
public:
    //virtual bool SendSuitSSMsg(const uint8_t app_type, const std::string& hash_key, msg_id_t msg_id,
    //    const google::protobuf::Message& msg, const guid_t& actor_id = 0) = 0;
    //virtual bool SendSuitSSMsg(const uint8_t app_type, const uint32_t& hash_value, msg_id_t msg_id,
    //    const google::protobuf::Message& msg, const guid_t& actor_id = 0) = 0;
    //virtual bool SendParticularSSMsg(bus_id_t bus_id, msg_id_t msg_id, const google::protobuf::Message& msg,
    //    const guid_t& conn_id, const guid_t& actor_id = 0) = 0;

    //virtual bool SendSSMsg(bus_id_t src_bus, bus_id_t target_bus, msg_id_t msg_id, const char* msg,
    //    const int msg_len, const guid_t& conn_id, const guid_t& actor_id = 0) = 0;
    //virtual bool SendSSMsg(bus_id_t target_bus, msg_id_t msg_id, const google::protobuf::Message& msg,
    //    const guid_t& conn_id, const guid_t& actor_id = 0) = 0;
    //virtual bool SendSSMsg(bus_id_t target_bus, msg_id_t msg_id, const char* msg, const int msg_len,
    //    const guid_t& conn_id, const guid_t& actor_id = 0) = 0;

    //virtual bool SendSSMsgByRouter(const AFSSMsgHead& head) = 0;

    //--------new interface---------
    virtual bool send_def_cs_msg_by_app_type(
        const app_type app_type, const msg_id_t msg_id, const google::protobuf::Message& msg) = 0;
    virtual bool send_def_ss_msg_by_app_type(
        const app_type app_type, const msg_id_t msg_id, const google::protobuf::Message& msg) = 0;
    virtual bool send_msg_by_app_type(
        const app_type app_type, msg_header* header, const google::protobuf::Message& msg) = 0;
    virtual bool send_msg_by_bus_id(
        bus_id_t bus_id, msg_id_t msg_id, const google::protobuf::Message& msg, const guid_t& guid = NULL_GUID) = 0;
    virtual bool send_to_client(conv_id_t session_id, msg_id_t msg_id, const google::protobuf::Message& msg,
        const guid_t& guid = NULL_GUID) = 0;
    virtual bool send_to_client(conv_id_t session_id, msg_id_t msg_id, const std::string& msg_data,
        bool compressed = false, const guid_t& guid = NULL_GUID) = 0;
    virtual bool send_to_intra(conv_id_t session_id, msg_id_t msg_id, const google::protobuf::Message& msg,
        const guid_t& guid = NULL_GUID) = 0;

    static bool recv_pb(const net_msg* msg, std::string& str_msg, guid_t& actor_id) {
        str_msg.assign(msg->get_msg_data(), msg->get_msg_length());
        actor_id = msg->get_actor_id();
        return true;
    }

    static bool recv_pb(const net_msg* msg, google::protobuf::Message& pb_msg, guid_t& actor_id) {
        if (!pb_msg.ParseFromString(std::string(msg->get_msg_data(), msg->get_msg_length())))
        {
            return false;
        }

        actor_id = msg->get_actor_id();
        return true;
    }

    static vector3d pb_to_vec(AFMsg::Point3D xPoint) {
        vector3d xID;
        xID.x = xPoint.x();
        xID.y = xPoint.y();
        xID.z = xPoint.z();
        return xID;
    }

    static AFMsg::Point3D vec_to_pb(vector3d xID) {
        AFMsg::Point3D xPoint;
        xPoint.set_x(xID.x);
        xPoint.set_y(xID.y);
        xPoint.set_z(xID.z);
        return xPoint;
    }
};

} // namespace ark
