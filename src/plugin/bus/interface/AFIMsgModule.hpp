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

#include "base/AFVector3D.hpp"
#include "proto/AFProtoCPP.hpp"
#include "interface/AFIModule.hpp"
#include "kernel/include/AFCNode.hpp"
#include "kernel/include/AFCTable.hpp"
#include "kernel/include/AFCData.hpp"
#include "kernel/interface/AFIEntity.hpp"
#include "net/include/AFNetMsg.hpp"

namespace ark {

class AFIMsgModule : public AFIModule
{
public:
    virtual bool SendSuitSSMsg(const uint8_t app_type, const std::string& hash_key, const int msg_id,
        const google::protobuf::Message& msg, const AFGUID& actor_id = 0) = 0;
    virtual bool SendSuitSSMsg(const uint8_t app_type, const uint32_t& hash_value, const int msg_id,
        const google::protobuf::Message& msg, const AFGUID& actor_id = 0) = 0;
    virtual bool SendParticularSSMsg(const int bus_id, const int msg_id, const google::protobuf::Message& msg,
        const AFGUID& conn_id, const AFGUID& actor_id = 0) = 0;

    virtual bool SendSSMsg(const int src_bus, const int target_bus, const int msg_id, const char* msg,
        const int msg_len, const AFGUID& conn_id, const AFGUID& actor_id = 0) = 0;
    virtual bool SendSSMsg(const int target_bus, const int msg_id, const google::protobuf::Message& msg,
        const AFGUID& conn_id, const AFGUID& actor_id = 0) = 0;
    virtual bool SendSSMsg(const int target_bus, const int msg_id, const char* msg, const int msg_len,
        const AFGUID& conn_id, const AFGUID& actor_id = 0) = 0;

    virtual bool SendSSMsgByRouter(const AFSSMsgHead& head) = 0;

    static bool RecvPB(const AFNetMsg* msg, std::string& str_msg, AFGUID& actor_id)
    {
        str_msg.assign(msg->GetMsgData(), msg->GetMsgLength());
        actor_id = msg->GetActorId();
        return true;
    }

    static bool RecvPB(const AFNetMsg* msg, google::protobuf::Message& pb_msg, AFGUID& actor_id)
    {
        if (!pb_msg.ParseFromString(std::string(msg->GetMsgData(), msg->GetMsgLength())))
        {
            return false;
        }

        actor_id = msg->GetActorId();
        return true;
    }

    static AFVector3D PBToVec(AFMsg::Point3D xPoint)
    {
        AFVector3D xID;
        xID.x = xPoint.x();
        xID.y = xPoint.y();
        xID.z = xPoint.z();
        return xID;
    }

    static AFMsg::Point3D VecToPB(AFVector3D xID)
    {
        AFMsg::Point3D xPoint;
        xPoint.set_x(xID.x);
        xPoint.set_y(xID.y);
        xPoint.set_z(xID.z);
        return xPoint;
    }

    static bool NodeToPBData(AFINode* pNode, AFMsg::pb_entity_data* pb_data)
    {
        ARK_ASSERT_RET_VAL(pNode != nullptr && pb_data != nullptr, false);

        auto index = pNode->GetIndex();
        switch (pNode->GetType())
        {
            case ArkDataType::DT_BOOLEAN:
                pb_data->mutable_datas_bool()->insert({index, pNode->GetBool()});
                break;
            case ArkDataType::DT_INT32:
                pb_data->mutable_datas_int32()->insert({index, pNode->GetInt32()});
                break;
            case ArkDataType::DT_UINT32:
                pb_data->mutable_datas_uint32()->insert({index, pNode->GetUInt32()});
                break;
            case ArkDataType::DT_INT64:
                pb_data->mutable_datas_int64()->insert({index, pNode->GetInt64()});
                break;
            case ArkDataType::DT_UINT64:
                pb_data->mutable_datas_uint64()->insert({index, pNode->GetUInt64()});
                break;
            case ArkDataType::DT_FLOAT:
                pb_data->mutable_datas_float()->insert({index, pNode->GetFloat()});
                break;
            case ArkDataType::DT_DOUBLE:
                pb_data->mutable_datas_double()->insert({index, pNode->GetDouble()});
                break;
            case ArkDataType::DT_STRING:
                pb_data->mutable_datas_string()->insert({index, pNode->GetString()});
                break;
            default:
                ARK_ASSERT_RET_VAL(0, false);
                break;
        }

        return true;
    }

    static bool NodeToPBData(const uint32_t index, const AFIData& data, AFMsg::pb_entity_data* pb_data)
    {
        ARK_ASSERT_RET_VAL(index > 0 && pb_data != nullptr, false);

        switch (data.GetType())
        {
            case ArkDataType::DT_BOOLEAN:
                pb_data->mutable_datas_bool()->insert({index, data.GetBool()});
                break;
            case ArkDataType::DT_INT32:
                pb_data->mutable_datas_int32()->insert({index, data.GetInt()});
                break;
            case ArkDataType::DT_UINT32:
                pb_data->mutable_datas_uint32()->insert({index, data.GetUInt()});
                break;
            case ArkDataType::DT_INT64:
                pb_data->mutable_datas_int64()->insert({index, data.GetInt64()});
                break;
            case ArkDataType::DT_UINT64:
                pb_data->mutable_datas_uint64()->insert({index, data.GetUInt64()});
                break;
            case ArkDataType::DT_FLOAT:
                pb_data->mutable_datas_float()->insert({index, data.GetFloat()});
                break;
            case ArkDataType::DT_DOUBLE:
                pb_data->mutable_datas_double()->insert({index, data.GetDouble()});
                break;
            case ArkDataType::DT_STRING:
                pb_data->mutable_datas_string()->insert({index, data.GetString()});
                break;
            default:
                ARK_ASSERT_RET_VAL(0, false);
                break;
        }

        return true;
    }

    static bool TableToPBData(AFITable* pTable, const uint32_t index, AFMsg::pb_table* pb_data)
    {
        ARK_ASSERT_RET_VAL(pTable != nullptr && index > 0 && pb_data != nullptr, false);

        for (AFIRow* pRow = pTable->First(); pRow != nullptr; pRow = pTable->Next())
        {
            AFMsg::pb_entity_data row_data;
            if (!RowToPBData(pRow, pRow->GetRow(), &row_data))
            {
                continue;
            }

            pb_data->mutable_datas_value()->insert({index, row_data});
        }

        return true;
    }

    static bool RowToPBData(AFIRow* pRow, const uint32_t index, AFMsg::pb_entity_data* pb_data)
    {
        ARK_ASSERT_RET_VAL(pRow != nullptr && index > 0 && pb_data != nullptr, false);

        for (auto pNode = pRow->First(); pNode != nullptr; pNode = pRow->Next())
        {
            NodeToPBData(pNode, pb_data);
        }

        return true;
    }

    static bool TableRowDataToPBData(
        const uint32_t index, uint32_t row, const uint32_t col, const AFIData& data, AFMsg::pb_entity_data* pb_data)
    {
        ARK_ASSERT_RET_VAL(index > 0 && row > 0 && col > 0 && pb_data != nullptr, false);

        AFMsg::pb_entity_data row_data;
        if (!NodeToPBData(col, data, &row_data))
        {
            return false;
        }

        AFMsg::pb_table table_data;
        table_data.mutable_datas_value()->insert({row, row_data});

        pb_data->mutable_datas_table()->insert({index, table_data});

        return true;
    }

    //node all to pb data
    static bool NodeAllToPBData(ARK_SHARE_PTR<AFIEntity> pEntity, AFMsg::pb_entity_data* pb_data)
    {
        ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

        for (auto pNode = pEntity->FirstNode(); pNode != nullptr; pNode = pEntity->NextNode())
        {
            NodeToPBData(pNode, pb_data);
        }

        return true;
    }

    //table all to pb data
    static bool TableAllToPBData(ARK_SHARE_PTR<AFIEntity> pEntity, AFMsg::pb_entity_data* pb_data)
    {
        ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);
        for (AFITable* pTable = pEntity->FirstTable(); pTable != nullptr; pTable = pEntity->NextTable())
        {
            const auto index = pTable->GetIndex();

            AFMsg::pb_table table_data;
            if (!TableToPBData(pTable, index, &table_data))
            {
                continue;
            }

            pb_data->mutable_datas_table()->insert({index, table_data});
        }

        return true;
    }

    static bool EntityToPBData(ARK_SHARE_PTR<AFIEntity> pEntity, AFMsg::pb_entity* pb_data)
    {
        ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

        pb_data->set_id(pEntity->GetID());

        NodeAllToPBData(pEntity, pb_data->mutable_data());

        TableAllToPBData(pEntity, pb_data->mutable_data());

        return true;
    }

    static bool EntityToPBDataByFeature(
        ARK_SHARE_PTR<AFIEntity> pEntity, AFFeatureType feature, AFMsg::pb_entity* pb_data)
    {
        ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

        pb_data->set_id(pEntity->GetID());

        NodeToPBDataByFeature(pEntity, feature, pb_data->mutable_data());

        TableToPBDataByFeature(pEntity, feature, pb_data->mutable_data());

        return true;
    }

    //node all to pb data
    static bool NodeToPBDataByFeature(
        ARK_SHARE_PTR<AFIEntity> pEntity, const AFFeatureType feature, AFMsg::pb_entity_data* pb_data)
    {
        ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

        for (auto pNode = pEntity->FirstNode(); pNode != nullptr; pNode = pEntity->NextNode())
        {
            auto result = (pNode->GetFeature() & feature);
            if (!result.any())
            {
                continue;
            }

            NodeToPBData(pNode, pb_data);
        }

        return true;
    }

    static bool TableToPBDataByFeature(
        ARK_SHARE_PTR<AFIEntity> pEntity, const AFFeatureType feature, AFMsg::pb_entity_data* pb_data)
    {
        ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);
        for (AFITable* pTable = pEntity->FirstTable(); pTable != nullptr; pTable = pEntity->NextTable())
        {
            auto result = (pTable->GetFeature() & feature);
            if (!result.any())
            {
                continue;
            }

            const auto index = pTable->GetIndex();

            AFMsg::pb_table table_data;
            if (!TableToPBData(pTable, index, &table_data))
            {
                continue;
            }

            pb_data->mutable_datas_table()->insert({index, table_data});
        }

        return true;
    }
};

#define ARK_PROCESS_ACTOR_MSG(head, msg_id, msg, msg_len, pb_msg_type)                                                 \
    AFGUID actor_id;                                                                                                   \
    pb_msg_type pb_msg;                                                                                                \
    if (!AFIMsgModule::RecvPB(head, msg, msg_len, pb_msg, actor_id))                                                   \
    {                                                                                                                  \
        ARK_LOG_ERROR("Parse msg error, msg_id = {}", msg_id);                                                         \
        return;                                                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    ARK_LOG_DEBUG("Recv msg log src={} dst={} msg_name={} msg_id={} msg_len={}\nmsg_data={}", "", "",                  \
        pb_msg.GetTypeName(), msg_id, msg_len, pb_msg.DebugString());                                                  \
                                                                                                                       \
    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(actor_id);                                           \
    if (nullptr == pEntity)                                                                                            \
    {                                                                                                                  \
        ARK_LOG_ERROR("FromClient actor_entity do not Exist, msg_id={} player_id={}", msg_id, actor_id.ToString());    \
        return;                                                                                                        \
    }

#define ARK_PROCESS_MSG(msg, pb_msg_type)                                                                              \
    AFGUID actor_id;                                                                                                   \
    pb_msg_type pb_msg;                                                                                                \
    if (!AFIMsgModule::RecvPB(msg, pb_msg, actor_id))                                                                  \
    {                                                                                                                  \
        ARK_LOG_ERROR("Parse msg error, msg_id={} pb_msg_type={}", msg->GetMsgId(), pb_msg.GetTypeName());             \
        return;                                                                                                        \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        std::string pb_json;                                                                                           \
        google::protobuf::util::MessageToJsonString(pb_msg, &pb_json);                                                 \
        ARK_LOG_DEBUG("Recv msg log src={} dst={} msg_name={} msg_id={} msg_len={}\nmsg_data={}", msg->GetSrcBus(),    \
            msg->GetDstBus(), pb_msg.GetTypeName(), msg->GetMsgId(), msg->GetMsgLength(), pb_json);                    \
    }

#define ARK_PROCESS_ACTOR_STRING_MSG(head, msg, msg_len)                                                               \
    std::string msg_data;                                                                                              \
    AFGUID actor_id;                                                                                                   \
    if (!AFIMsgModule::RecvPB(head, msg, msg_len, msg_data, actor_id))                                                 \
    {                                                                                                                  \
        ARK_LOG_ERROR("Parse msg error, msg_id = {}", msg_id);                                                         \
        return;                                                                                                        \
    }

} // namespace ark
