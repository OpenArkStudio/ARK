/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2017 QuadHex authors.
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

#include "base/AFProtoCPP.hpp"
#include "base/AFDataNode.hpp"
#include "base/AFDataTable.hpp"
#include "AFIModule.h"
#include "AFINet.h"
#include "AFIDataNodeManager.h"
#include "AFIDataTableManager.h"

namespace ark
{

    class AFIMsgModule : public AFIModule
    {
    public:
        virtual ~AFIMsgModule() = default;

        virtual bool SendSuitSSMsg(const uint8_t app_type, const std::string& hash_key, const int msg_id, const google::protobuf::Message& msg, const AFGUID& actor_id = 0) = 0;
        virtual bool SendSuitSSMsg(const uint8_t app_type, const uint32_t& hash_value, const int msg_id, const google::protobuf::Message& msg, const AFGUID& actor_id = 0) = 0;
        virtual bool SendParticularSSMsg(const int bus_id, const int msg_id, const google::protobuf::Message& msg, const AFGUID& conn_id, const AFGUID& actor_id = 0) = 0;

        virtual bool SendSSMsg(const int src_bus, const int target_bus, const int msg_id, const char* msg, const int msg_len, const AFGUID& conn_id, const AFGUID& actor_id = 0) = 0;
        virtual bool SendSSMsg(const int target_bus, const int msg_id, const google::protobuf::Message& msg, const AFGUID& conn_id, const AFGUID& actor_id = 0) = 0;
        virtual bool SendSSMsg(const int target_bus, const int msg_id, const char* msg, const int msg_len, const AFGUID& conn_id, const AFGUID& actor_id = 0) = 0;

        virtual bool SendSSMsgByRouter(const AFSSMsgHead& head) = 0;

        static bool RecvPB(const AFNetMsg* msg, std::string& strMsg, AFGUID& nPlayer)
        {
            strMsg.assign(msg->msg_data_, msg->length_);
            nPlayer = msg->actor_id_;
            return true;
        }

        static bool RecvPB(const AFNetMsg* msg, google::protobuf::Message& pb_msg, AFGUID& actor_id)
        {
            if (!pb_msg.ParseFromString(std::string(msg->msg_data_, msg->length_)))
            {
                return false;
            }

            actor_id = msg->actor_id_;
            return true;
        }

        static Point3D PBToVec(AFMsg::Point3D xPoint)
        {
            Point3D xID;
            xID.x = xPoint.x();
            xID.y = xPoint.y();
            xID.z = xPoint.z();
            return xID;
        }

        static AFMsg::Point3D VecToPB(Point3D xID)
        {
            AFMsg::Point3D xPoint;
            xPoint.set_x(xID.x);
            xPoint.set_y(xID.y);
            xPoint.set_z(xID.z);
            return xPoint;
        }

        static bool AFData2PBVariant(const AFIData& DataVar, AFMsg::VariantData* variantData)
        {
            if (variantData == nullptr)
            {
                return false;
            }

            switch (DataVar.GetType())
            {
            case DT_BOOLEAN:
                variantData->set_bool_value(DataVar.GetBool());
                break;
            case DT_INT:
                variantData->set_int_value(DataVar.GetInt());
                break;
            case DT_INT64:
                variantData->set_int64_value(DataVar.GetInt64());
                break;
            case DT_FLOAT:
                variantData->set_float_value(DataVar.GetFloat());
                break;
            case DT_DOUBLE:
                variantData->set_double_value(DataVar.GetDouble());
                break;
            case DT_STRING:
                variantData->set_str_value(DataVar.GetString());
                break;
            default:
                ARK_ASSERT_RET_VAL(0, false);
                break;
            }

            return true;
        }

        static bool DataNodeToPBNode(const AFIData& DataVar, const char* name, AFMsg::PBNodeData& xMsg)
        {
            if (nullptr == name)
            {
                return false;
            }

            xMsg.set_node_name(name);
            xMsg.set_data_type(DataVar.GetType());
            AFMsg::VariantData* variantData = xMsg.mutable_variant_data();

            return AFData2PBVariant(DataVar, variantData);
        }

        static bool TableCellToPBCell(const AFIData& DataVar, const int nRow, const int nCol, AFMsg::PBCellData& xMsg)
        {
            xMsg.set_col(nCol);
            xMsg.set_row(nRow);
            xMsg.set_data_type(DataVar.GetType());
            AFMsg::VariantData* variantData = xMsg.mutable_variant_data();

            return AFData2PBVariant(DataVar, variantData);
        }

        static bool TableCellToPBCell(const AFIDataList& DataList, const int nRow, const int nCol, AFMsg::PBCellData& xMsg)
        {
            xMsg.set_col(nCol);
            xMsg.set_row(nRow);
            xMsg.set_data_type(DataList.GetType(nCol));
            AFMsg::VariantData* variantData = xMsg.mutable_variant_data();

            switch (DataList.GetType(nCol))
            {
            case DT_BOOLEAN:
                variantData->set_bool_value(DataList.Bool(nCol));
                break;
            case DT_INT:
                variantData->set_int_value(DataList.Int(nCol));
                break;
            case DT_INT64:
                variantData->set_int64_value(DataList.Int64(nCol));
                break;
            case DT_FLOAT:
                variantData->set_float_value(DataList.Float(nCol));
                break;
            case DT_DOUBLE:
                variantData->set_double_value(DataList.Double(nCol));
                break;
            case DT_STRING:
                variantData->set_str_value(DataList.String(nCol));
                break;
            default:
                ARK_ASSERT_RET_VAL(0, false);
                break;
            }

            return true;
        }

        static bool PackTableToPB(AFDataTable* pTable, AFMsg::EntityDataTableBase* pEntityTableBase)
        {
            if (pTable == nullptr || pEntityTableBase == nullptr)
            {
                return false;
            }

            for (size_t i = 0; i < pTable->GetRowCount(); i++)
            {
                AFMsg::DataTableAddRow* pAddRowStruct = pEntityTableBase->add_row();
                pAddRowStruct->set_row(i);

                for (size_t j = 0; j < pTable->GetColCount(); j++)
                {
                    AFMsg::PBCellData* pAddData = pAddRowStruct->add_cell_list();

                    AFCData xRowColData;

                    if (!pTable->GetValue(i, j, xRowColData))
                    {
                        continue;
                    }

                    TableCellToPBCell(xRowColData, i, j, *pAddData);
                }
            }

            return true;
        }

        static bool AddTableToPB(AFDataTable* pTable, AFMsg::EntityDataTableList* pPBData)
        {
            if (!pTable || !pPBData)
            {
                return false;
            }

            AFMsg::EntityDataTableBase* pTableBase = pPBData->add_data_table_list();
            pTableBase->set_table_name(pTable->GetName());

            if (!PackTableToPB(pTable, pTableBase))
            {
                return false;
            }

            return true;
        }


        static bool TableListToPB(AFGUID self, ARK_SHARE_PTR<AFIDataTableManager>& pTableManager, AFMsg::EntityDataTableList& xPBData, const AFFeatureType nFeature)
        {
            AFMsg::EntityDataTableList* pPBData = &xPBData;
            pPBData->set_entity_id(self);

            if (pTableManager == nullptr)
            {
                return false;
            }

            size_t nTableCount = pTableManager->GetCount();

            for (size_t i = 0; i < nTableCount; ++i)
            {
                AFDataTable* pTable = pTableManager->GetTableByIndex(i);

                if (pTable == nullptr)
                {
                    continue;
                }

                AFFeatureType xResult = (pTable->GetFeature() & nFeature);

                if (xResult.any())
                {
                    AddTableToPB(pTable, pPBData);
                }
            }

            return true;
        }

        static bool NodeListToPB(AFGUID self, ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager, AFMsg::EntityDataNodeList& xPBData, const AFFeatureType nFeature)
        {
            if (pNodeManager == nullptr)
            {
                return false;
            }

            for (size_t i = 0; i < pNodeManager->GetNodeCount(); i++)
            {
                AFDataNode* pNode = pNodeManager->GetNodeByIndex(i);

                if (pNode == nullptr)
                {
                    continue;
                }

                AFFeatureType xResult = (pNode->GetFeature() & nFeature);

                if (pNode->Changed() && xResult.any())
                {
                    AFMsg::PBNodeData* pData = xPBData.add_data_node_list();
                    DataNodeToPBNode(pNode->GetValue(), pNode->GetName(), *pData);
                }
            }

            return true;
        }
    };

#define ARK_PROCESS_ACTOR_MSG(head, msg_id, msg, msg_len, pb_msg)                   \
    AFGUID actor_id;                                                                \
    pb_msg x_msg;                                                                   \
    if (!AFIMsgModule::RecvPB(head, msg, msg_len, x_msg, actor_id))                 \
    {                                                                               \
        ARK_LOG_ERROR("Parse msg error, msg_id = {}", msg_id);                      \
        return;                                                                     \
    }                                                                               \
                                                                                    \
    ARK_LOG_DEBUG("Recv msg log src={} dst={} msg_name={} msg_id={} msg_len={}\nmsg_data={}", \
        "",                                                                         \
        "",                                                                         \
        x_msg.GetTypeName(),                                                        \
        msg_id,                                                                     \
        msg_len,                                                                    \
        x_msg.DebugString());                                                       \
                                                                                    \
    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(actor_id);        \
    if (nullptr == pEntity)                                                                                             \
    {                                                                                                                   \
        ARK_LOG_ERROR("FromClient actor_entity do not Exist, msg_id={} player_id={}", msg_id, actor_id.ToString());     \
        return;                                                                                                         \
    }

#define ARK_PROCESS_MSG(msg, pb_msg_type)                                       \
    AFGUID actor_id;                                                            \
    pb_msg_type pb_msg;                                                         \
    if (!AFIMsgModule::RecvPB(msg, pb_msg, actor_id))                           \
    {                                                                           \
        ARK_LOG_ERROR("Parse msg error, msg_id={} pb_msg_type={}", msg->id_, pb_msg.GetTypeName()); \
        return;                                                                 \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        std::string pb_json;                                                    \
        google::protobuf::util::MessageToJsonString(pb_msg, &pb_json);          \
        ARK_LOG_DEBUG("Recv msg log src={} dst={} msg_name={} msg_id={} nmsg_len={}\nmsg_data={}", \
              msg->src_bus_,                                                    \
              msg->dst_bus_,                                                    \
              pb_msg.GetTypeName(),                                             \
              msg->id_,                                                         \
              msg->length_,                                                     \
              pb_json);                                                         \
    }


#define  ARK_PROCESS_ACTOR_STRING_MSG(head, msg, msg_len)                       \
    std::string msg_data;                                                       \
    AFGUID actor_id;                                                            \
    if (!AFIMsgModule::RecvPB(head, msg, msg_len, msg_data, actor_id))          \
    {                                                                           \
        ARK_LOG_ERROR("Parse msg error, msg_id = {}", msg_id);                  \
        return;                                                                 \
    }

}