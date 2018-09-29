/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "Common/AFProtoCPP.hpp"
#include "SDK/Interface/AFIModule.h"
#include "SDK/Interface/AFINet.h"
#include "SDK/Core/AFDataNode.hpp"
#include "SDK/Core/AFDataTable.h"
#include "SDK/Core/AFIDataNodeManager.h"
#include "SDK/Core/AFIDataTableManager.h"

namespace ark
{

    class AFIMsgModule : public AFIModule
    {
    public:
        virtual ~AFIMsgModule() = default;

        static bool RecvPB(const ARK_PKG_BASE_HEAD& xHead, const char* msg, const uint32_t nLen, std::string& strMsg, AFGUID& nPlayer)
        {
            strMsg.assign(msg, nLen);
            nPlayer = xHead.GetPlayerID();
            return true;
        }

        static bool RecvPB(const ARK_PKG_BASE_HEAD& xHead, const char* msg, const uint32_t nLen, google::protobuf::Message& xData, AFGUID& nPlayer)
        {
            if (!xData.ParseFromString(std::string(msg, nLen)))
            {
                return false;
            }

            nPlayer = xHead.GetPlayerID();

            return true;
        }

        static AFGUID PBToGUID(AFMsg::PBGUID xID)
        {
            AFGUID xIdent;
            xIdent.nHigh = xID.high();
            xIdent.nLow = xID.low();

            return xIdent;
        }

        static AFMsg::PBGUID GUIDToPB(AFGUID xID)
        {
            AFMsg::PBGUID  xIdent;
            xIdent.set_high(xID.nHigh);
            xIdent.set_low(xID.nLow);

            return xIdent;
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
            case DT_OBJECT:
                *variantData->mutable_guid_value() = GUIDToPB(DataVar.GetObject());
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

            case DT_OBJECT:
                *variantData->mutable_guid_value() = GUIDToPB(DataList.Object(nCol));
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
            *(pPBData->mutable_entity_id()) = GUIDToPB(self);

            if (!pTableManager)
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
            if (!pNodeManager)
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

        virtual bool SendSuitSSMsg(const uint8_t app_type, const std::string& hash_key, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id = 0) = 0;
        virtual bool SendSuitSSMsg(const uint8_t app_type, const uint32_t& hash_value, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id = 0) = 0;
        virtual bool SendParticularSSMsg(const int bus_id, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id = 0) = 0;

        virtual bool SendSSMsg(const int src_bus, const int target_bus, const int msg_id, const char* msg, const int msg_len, const AFGUID& target_role_id = 0) = 0;
        virtual bool SendSSMsg(const int target_bus, const int msg_id, const google::protobuf::Message& msg, const AFGUID& target_role_id = 0) = 0;
        virtual bool SendSSMsg(const int target_bus, const int msg_id, const char* msg, const int msg_len, const AFGUID& target_role_id = 0) = 0;
    };

#define ARK_MSG_PROCESS(xHead, nMsgID, msgData, nLen, msgType)                          \
    AFGUID nPlayerID;                                                                   \
    msgType xMsg;                                                                       \
    if (!AFIMsgModule::RecvPB(xHead, msgData, nLen, xMsg, nPlayerID))                   \
    {                                                                                   \
        ARK_LOG_ERROR("Parse msg error, nMsgID = %d", nMsgID);                          \
        return;                                                                         \
    }                                                                                   \
                                                                                        \
    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(nPlayerID);           \
    if (nullptr == pEntity)                                                                                         \
    {                                                                                                               \
        ARK_LOG_ERROR("FromClient Object do not Exist, nMsgID = %d player_id = %s", nMsgID, nPlayerID.ToString());  \
        return;                                                                                                     \
    }

#define ARK_MSG_PROCESS_NO_OBJECT(xHead, msgData, nLen, msgType)                        \
    AFGUID nPlayerID;                                                                   \
    msgType xMsg;                                                                       \
    if (!AFIMsgModule::RecvPB(xHead, msgData, nLen, xMsg, nPlayerID))                   \
    {                                                                                   \
        ARK_LOG_ERROR("Parse msg error, nMsgID = %d", nMsgID);                          \
        return;                                                                         \
    }

#define  ARK_MSG_PROCESS_NO_OBJECT_STRING(xHead, msg, nLen)                     \
    std::string strMsg;                                                         \
    AFGUID nPlayerID;                                                           \
    int nHasKey = 0;                                                            \
    if (AFIMsgModule::RecvPB((xHead), (msg), (nLen), strMsg, nPlayerID))        \
    {                                                                           \
        nHasKey = (int)nPlayerID.nLow;                                          \
    }

}