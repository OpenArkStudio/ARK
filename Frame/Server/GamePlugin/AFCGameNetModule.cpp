/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include "Common/AFEventDefine.hpp"
#include "SDK/Interface/AFIModule.h"
#include "Common/AFDataDefine.hpp"
#include "SDK/Core/AFDataNode.hpp"
#include "AFCGameNetModule.h"

namespace ark
{

    bool AFCGameNetModule::Init()
    {
        m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
        m_pClassModule = pPluginManager->FindModule<AFIMetaClassModule>();
        m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();
        m_pUUIDModule = pPluginManager->FindModule<AFIGUIDModule>();
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pMsgModule = pPluginManager->FindModule<AFIMsgModule>();
        m_pNetServiceManagerModule = pPluginManager->FindModule<AFINetServiceManagerModule>();
        m_pSceneProcessModule = pPluginManager->FindModule<AFISceneProcessModule>();
        m_AccountModule = pPluginManager->FindModule<AFIAccountModule>();
        m_pMapModule = pPluginManager->FindModule<AFIMapModule>();

        m_pKernelModule->RegCommonClassEvent(this, &AFCGameNetModule::OnCommonClassEvent);
        m_pKernelModule->RegCommonDataNodeEvent(this, &AFCGameNetModule::OnCommonDataNodeEvent);
        m_pKernelModule->RegCommonDataTableEvent(this, &AFCGameNetModule::OnCommonDataTableEvent);

        m_pKernelModule->AddClassCallBack(ark::Player::ThisName(), this, &AFCGameNetModule::OnEntityEvent);

        return true;
    }

    bool AFCGameNetModule::PostInit()
    {
        int ret = StartServer();
        if (ret != 0)
        {
            exit(0);
            return false;
        }

        return true;
    }

    int AFCGameNetModule::StartServer()
    {
        int ret = m_pNetServiceManagerModule->CreateServer();
        if (ret != 0)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            ARK_ASSERT_NO_EFFECT(0);
            return ret;
        }

        m_pNetServerService = m_pNetServiceManagerModule->GetSelfNetServer();
        if (m_pNetServerService == nullptr)
        {
            ret = -3;
            ARK_LOG_ERROR("Cannot find server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            return ret;
        }

        m_pNetServerService->RegMsgCallback(AFMsg::EGMI_PTWG_PROXY_REFRESH, this, &AFCGameNetModule::OnRefreshProxyServerInfoProcess);
        m_pNetServerService->RegMsgCallback(AFMsg::EGMI_PTWG_PROXY_REGISTERED, this, &AFCGameNetModule::OnProxyServerRegisteredProcess);
        m_pNetServerService->RegMsgCallback(AFMsg::EGMI_PTWG_PROXY_UNREGISTERED, this, &AFCGameNetModule::OnProxyServerUnRegisteredProcess);
        m_pNetServerService->RegMsgCallback(AFMsg::EGMI_REQ_ENTER_GAME, this, &AFCGameNetModule::OnClienEnterGameProcess);
        m_pNetServerService->RegMsgCallback(AFMsg::EGMI_REQ_LEAVE_GAME, this, &AFCGameNetModule::OnClientLeaveGameProcess);
        m_pNetServerService->RegMsgCallback(AFMsg::EGMI_REQ_ROLE_LIST, this, &AFCGameNetModule::OnReqiureRoleListProcess);
        m_pNetServerService->RegMsgCallback(AFMsg::EGMI_REQ_CREATE_ROLE, this, &AFCGameNetModule::OnCreateRoleGameProcess);
        m_pNetServerService->RegMsgCallback(AFMsg::EGMI_REQ_DELETE_ROLE, this, &AFCGameNetModule::OnDeleteRoleGameProcess);
        m_pNetServerService->RegMsgCallback(AFMsg::EGMI_REQ_RECOVER_ROLE, this, &AFCGameNetModule::OnClienSwapSceneProcess);
        m_pNetServerService->RegMsgCallback(AFMsg::EGMI_REQ_SWAP_SCENE, this, &AFCGameNetModule::OnClienSwapSceneProcess);

        //m_pNetServerService->RegNetEventCallback(this, &AFCGameNetModule::OnSocketEvent);

        return 0;
    }

    bool AFCGameNetModule::PreUpdate()
    {
        int ret = StartClient();
        return (ret == 0);
    }

    int AFCGameNetModule::StartClient()
    {
        //创建所有与对端链接的client
        int ret = m_pNetServiceManagerModule->CreateClusterClients();
        if (ret != 0)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            ARK_ASSERT_NO_EFFECT(0);
            return ret;
        }

        //if need to add a member
        AFINetClientService* pNetClient = m_pNetServiceManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_WORLD);
        if (pNetClient == nullptr)
        {
            return -1;
        }

        //TODO:will add a transfer callback
        //pNetClient->RegMsgCallback(this, &AFCGameNetClientModule::TransPBToProxy);

        return 0;
    }

    void AFCGameNetModule::OnClienEnterGameProcess(const AFNetMsg* msg, const int64_t session_id)
    {
        ////Before enter game, PlayerID means gate fd
        //ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ReqEnterGameServer);
        //const AFGUID& nGateClientID = actor_id;
        //AFGUID nRoleID = AFIMsgModule::PBToGUID(x_msg.id());

        //if (m_pKernelModule->GetEntity(nRoleID))
        //{
        //    m_pKernelModule->DestroyEntity(nRoleID);
        //}

        ////////////////////////////////////////////////////////////////////////////

        //ARK_SHARE_PTR<AFCGameNetModule::GateBaseInfo> pGateInfo = GetPlayerGateInfo(nRoleID);

        //if (nullptr != pGateInfo && RemovePlayerGateInfo(nRoleID))
        //{
        //    ARK_LOG_ERROR("RemovePlayerGateInfo fail, id = {}", nRoleID);
        //}

        //ARK_SHARE_PTR<AFCGameNetModule::GateServerInfo> pGateServereinfo = GetGateServerInfoByClientID(conn_id);

        //if (nullptr == pGateServereinfo)
        //{
        //    return;
        //}

        //int nGateID = pGateServereinfo->xServerData.server_info_.bus_id();
        //if (nGateID <= 0)
        //{
        //    return;
        //}

        //if (!AddPlayerGateInfo(nRoleID, nGateClientID, nGateID))
        //{
        //    return;
        //}

        ////默认1号场景
        //int nSceneID = 1;
        //AFCDataList var;
        //var.AddString("Name");
        //var.AddString(x_msg.name().c_str());

        //var.AddString("GateID");
        //var.AddInt(nGateID);

        //var.AddString("ClientID");
        //var.AddInt64(nGateClientID);

        //ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->CreateEntity(nRoleID, nSceneID, 0, ark::Player::ThisName(), "", var);

        //if (nullptr == pEntity)
        //{
        //    //内存泄漏
        //    //mRoleBaseData
        //    //mRoleFDData
        //    return;
        //}

        //pEntity->SetNodeInt("LoadPropertyFinish", 1);
        //pEntity->SetNodeInt("GateID", nGateID);
        //pEntity->SetNodeInt("GameID", pPluginManager->BusID());

        //m_pKernelModule->DoEvent(pEntity->Self(), ark::Player::ThisName(), ENTITY_EVT_ALL_FINISHED, AFCDataList());

        //AFCDataList varEntry;
        //varEntry << pEntity->Self();
        //varEntry << int32_t(0);
        //varEntry << (int32_t)nSceneID;
        //varEntry << int32_t (-1);
        //m_pKernelModule->DoEvent(pEntity->Self(), AFED_ON_CLIENT_ENTER_SCENE, varEntry);
    }

    void AFCGameNetModule::OnClientLeaveGameProcess(const AFNetMsg* msg, const int64_t session_id)
    {
        //ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ReqLeaveGameServer);

        //if (actor_id.IsNULL())
        //{
        //    return;
        //}

        //if (m_pKernelModule->GetEntity(actor_id))
        //{
        //    m_pKernelModule->DestroyEntity(actor_id);
        //}

        //if (!RemovePlayerGateInfo(actor_id))
        //{
        //    ARK_LOG_ERROR("RemovePlayerGateInfo failed, id = {}", actor_id);
        //}
    }

    int AFCGameNetModule::OnViewDataNodeEnter(const AFIDataList& argVar, const AFGUID& self)
    {
        if (argVar.GetCount() <= 0 || self == 0)
        {
            return 1;
        }

        AFMsg::MultiEntityDataNodeList xPublicMsg;
        ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

        if (nullptr == pEntity)
        {
            return 0;
        }

        ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager = pEntity->GetNodeManager();
        AFFeatureType nFeature;
        nFeature[AFDataNode::PF_PUBLIC] = 1;
        AFIMsgModule::NodeListToPB(self, pNodeManager, *xPublicMsg.add_multi_entity_data_node_list(), nFeature);

        for (size_t i = 0; i < argVar.GetCount(); i++)
        {
            AFGUID identOther = argVar.Int64(i);

            if (self != identOther)
            {
                SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, xPublicMsg, identOther);
            }
        }

        return 0;
    }

    int AFCGameNetModule::OnSelfDataNodeEnter(const AFGUID& self)
    {
        if (self == 0)
        {
            return 1;
        }

        ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

        if (nullptr == pEntity)
        {
            return 1;
        }

        AFMsg::MultiEntityDataNodeList xPrivateMsg;
        ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager = pEntity->GetNodeManager();
        AFFeatureType nFeature = 0;
        nFeature[AFDataNode::PF_PRIVATE] = 1;
        AFIMsgModule::NodeListToPB(self, pNodeManager, *xPrivateMsg.add_multi_entity_data_node_list(), nFeature);

        SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, xPrivateMsg, self);
        return 0;
    }


    int AFCGameNetModule::OnSelfDataTableEnter(const AFGUID& self)
    {
        if (self == 0)
        {
            return 1;
        }

        AFMsg::MultiEntityDataTableList xPrivateMsg;

        ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

        if (nullptr == pEntity)
        {
            return 1;
        }

        AFFeatureType nFeature = 0;
        nFeature[AFDataNode::PF_PRIVATE] = 1;
        ARK_SHARE_PTR<AFIDataTableManager> pTableManager = pEntity->GetTableManager();
        AFIMsgModule::TableListToPB(self, pTableManager, *xPrivateMsg.add_multi_entity_data_table_list(), nFeature);
        SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, xPrivateMsg, self);
        return 0;
    }

    int AFCGameNetModule::OnViewDataTableEnter(const AFIDataList& argVar, const AFGUID& self)
    {
        if (argVar.GetCount() <= 0 || self == 0)
        {
            return 1;
        }

        AFMsg::MultiEntityDataTableList xPublicMsg;

        ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

        if (nullptr == pEntity)
        {
            return 1;
        }

        ARK_SHARE_PTR<AFIDataTableManager> pTableManager = pEntity->GetTableManager();
        AFFeatureType nFeature = 0;
        nFeature[AFDataNode::PF_PUBLIC] = 1;
        AFIMsgModule::TableListToPB(self, pTableManager, *xPublicMsg.add_multi_entity_data_table_list(), nFeature);

        for (size_t i = 0; i < argVar.GetCount(); i++)
        {
            AFGUID identOther = argVar.Int64(i);

            if (self != identOther && xPublicMsg.multi_entity_data_table_list_size() > 0)
            {
                SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, xPublicMsg, identOther);
            }
        }

        return 0;
    }

    bool AFCGameNetModule::ProcessLeaveGroup(const AFGUID& self, int nSceneID, int nOldGroupID)
    {
        if (nOldGroupID <= 0)
        {
            return false;
        }

        AFCDataList valueAllOldObjectList;
        AFCDataList valueAllOldPlayerList;
        m_pMapModule->GetInstEntityList(nSceneID, nOldGroupID, valueAllOldObjectList);

        if (valueAllOldObjectList.GetCount() > 0)
        {
            //自己只需要广播其他玩家
            for (size_t i = 0; i < valueAllOldObjectList.GetCount(); i++)
            {
                AFGUID identBC = valueAllOldObjectList.Int64(i);

                if (valueAllOldObjectList.Int64(i) == self)
                {
                    valueAllOldObjectList.SetInt64(i, 0);
                }

                const std::string strClassName(m_pKernelModule->GetNodeString(identBC, "ClassName"));

                if (ark::Player::ThisName() == strClassName)
                {
                    valueAllOldPlayerList << identBC;
                }
            }

            OnEntityListLeave(valueAllOldPlayerList, AFCDataList() << self);

            //老的全部要广播删除
            OnEntityListLeave(AFCDataList() << self, valueAllOldObjectList);
        }

        m_pKernelModule->DoEvent(self, AFED_ON_CLIENT_LEAVE_SCENE, AFCDataList() << nOldGroupID);

        return true;
    }

    bool AFCGameNetModule::ProcessEnterGroup(const AFGUID& self, int nSceneID, int nNewGroupID)
    {
        //再广播给别人自己出现(层升或者跃层)
        if (nNewGroupID <= 0)
        {
            return false;
        }

        //这里需要把自己从广播中排除
        //////////////////////////////////////////////////////////////////////////
        AFCDataList valueAllObjectList;
        AFCDataList valueAllObjectListNoSelf;
        AFCDataList valuePlayerList;
        AFCDataList valuePlayerListNoSelf;
        m_pMapModule->GetInstEntityList(nSceneID, nNewGroupID, valueAllObjectList);

        for (size_t i = 0; i < valueAllObjectList.GetCount(); i++)
        {
            AFGUID identBC = valueAllObjectList.Int64(i);
            const std::string strClassName(m_pKernelModule->GetNodeString(identBC, "ClassName"));

            if (ark::Player::ThisName() == strClassName)
            {
                valuePlayerList << identBC;

                if (identBC != self)
                {
                    valuePlayerListNoSelf << identBC;
                }
            }

            if (identBC != self)
            {
                valueAllObjectListNoSelf << identBC;
            }
        }

        //广播给别人,自己出现(这里本不应该广播给自己)
        if (valuePlayerListNoSelf.GetCount() > 0)
        {
            OnEntityListEnter(valuePlayerListNoSelf, AFCDataList() << self);
            OnViewDataNodeEnter(valuePlayerListNoSelf, self);
            OnViewDataTableEnter(valuePlayerListNoSelf, self);

            const std::string strSelfClassName(m_pKernelModule->GetNodeString(self, "ClassName"));

            if (strSelfClassName == ark::Player::ThisName())
            {
                OnEntityListEnter(AFCDataList() << self, valueAllObjectListNoSelf);

                for (size_t i = 0; i < valueAllObjectListNoSelf.GetCount(); i++)
                {
                    //此时不用再广播自己的属性给自己
                    //把已经存在的人的属性广播给新来的人
                    AFGUID identOld = valueAllObjectListNoSelf.Int64(i);

                    OnViewDataNodeEnter(AFCDataList() << self, identOld);
                    //把已经存在的人的表广播给新来的人
                    OnViewDataTableEnter(AFCDataList() << self, identOld);
                }
            }
        }

        return true;
    }


    int AFCGameNetModule::OnEntityListEnter(const AFIDataList& self, const AFIDataList& argVar)
    {
        if (self.GetCount() <= 0 || argVar.GetCount() <= 0)
        {
            return 1;
        }

        AFMsg::AckEntityEnterList xEntityEnterList;

        for (size_t i = 0; i < argVar.GetCount(); i++)
        {
            AFGUID identOld = argVar.Int64(i);

            //排除空对象
            if (identOld == 0)
            {
                continue;
            }

            AFMsg::EntityEnterInfo* pEnterInfo = xEntityEnterList.add_entity_list();
            pEnterInfo->set_object_guid(identOld);
            Point3D xPoint;
            xPoint.x = m_pKernelModule->GetNodeFloat(identOld, "x");
            xPoint.y = m_pKernelModule->GetNodeFloat(identOld, "y");
            xPoint.z = m_pKernelModule->GetNodeFloat(identOld, "z");

            *pEnterInfo->mutable_pos() = AFIMsgModule::VecToPB(xPoint);
            pEnterInfo->set_career_type(m_pKernelModule->GetNodeInt(identOld, "Job"));
            pEnterInfo->set_player_state(m_pKernelModule->GetNodeInt(identOld, "State"));
            pEnterInfo->set_config_id(m_pKernelModule->GetNodeString(identOld, "ConfigID"));
            pEnterInfo->set_scene_id(m_pKernelModule->GetNodeInt(identOld, "SceneID"));
            pEnterInfo->set_class_id(m_pKernelModule->GetNodeString(identOld, "ClassName"));
        }

        if (xEntityEnterList.entity_list_size() <= 0)
        {
            return 1;
        }

        for (size_t i = 0; i < self.GetCount(); i++)
        {
            AFGUID ident = self.Int64(i);

            if (!ident != 0)
            {
                //可能在不同的网关呢,得到后者所在的网关FD
                SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_ENTER, xEntityEnterList, ident);
            }
        }

        return 0;
    }

    int AFCGameNetModule::OnEntityListLeave(const AFIDataList& self, const AFIDataList& argVar)
    {
        if (self.GetCount() <= 0 || argVar.GetCount() <= 0)
        {
            return 1;
        }

        AFMsg::AckEntityLeaveList xEntityLeaveInfoList;

        for (size_t i = 0; i < argVar.GetCount(); i++)
        {
            AFGUID identOld = argVar.Int64(i);

            //排除空对象
            if (!identOld != 0)
            {
                xEntityLeaveInfoList.add_entity_list(identOld);
            }
        }

        for (size_t i = 0; i < self.GetCount(); i++)
        {
            AFGUID ident = self.Int64(i);

            if (ident != 0)
            {
                //可能在不同的网关,得到后者所在的网关FD
                SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_LEAVE, xEntityLeaveInfoList, ident);
            }
        }

        return 0;
    }

    int AFCGameNetModule::OnCommonDataNodeEvent(const AFGUID& self, const std::string& name, const AFIData& oldVar, const AFIData& newVar)
    {
        if (IObject::InstanceID() == name)
        {
            //自己还是要知道自己的这个属性变化的,但是别人就不需要知道了
            OnGroupEvent(self, name, oldVar, newVar);
        }

        if (IObject::MapID() == name)
        {
            //自己还是要知道自己的这个属性变化的,但是别人就不需要知道了
            OnContainerEvent(self, name, oldVar, newVar);
        }

        if (ark::Player::ThisName() == std::string(m_pKernelModule->GetNodeString(self, IObject::ClassName())) && (m_pKernelModule->GetNodeInt(self, Player::LoadDataFinish()) <= 0))
        {
            return 0;
        }

        AFCDataList valueBroadCaseList;
        GetNodeBroadcastEntityList(self, name, valueBroadCaseList);

        if (valueBroadCaseList.GetCount() <= 0)
        {
            return 0;
        }

        AFMsg::EntityDataNode xEntityDataNode;
        xEntityDataNode.set_entity_id(self);
        AFMsg::PBNodeData* pData = xEntityDataNode.add_data_node_list();
        AFIMsgModule::DataNodeToPBNode(oldVar, name.c_str(), *pData);

        for (size_t i = 0; i < valueBroadCaseList.GetCount(); i++)
        {
            AFGUID ident = valueBroadCaseList.Int64(i);
            SendMsgPBToGate(AFMsg::EGMI_ACK_NODE_DATA, xEntityDataNode, ident);
        }

        return 0;
    }

    void AFCGameNetModule::CommonDataTableAddEvent(const AFGUID& self, const std::string& strTableName, int nRow, int nCol, const AFCDataList& valueBroadCaseList)
    {
        AFMsg::EntityDataTableAddRow xTableAddRow;
        xTableAddRow.set_entity_id(self);
        xTableAddRow.set_table_name(strTableName);

        AFMsg::DataTableAddRow* pAddRowData = xTableAddRow.add_row_data();
        pAddRowData->set_row(nRow);

        //add row 需要完整的row
        AFDataTable* pTable = m_pKernelModule->FindTable(self, strTableName);
        if (pTable == nullptr)
        {
            return;
        }

        AFCDataList xRowDataList;

        if (pTable->QueryRow(nRow, xRowDataList))
        {
            for (size_t i = 0; i < xRowDataList.GetCount(); i++)
            {
                AFMsg::PBCellData* pAddData = pAddRowData->add_cell_list();
                AFIMsgModule::TableCellToPBCell(xRowDataList, nRow, nCol, *pAddData);
            }

            for (size_t i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                AFGUID identOther = valueBroadCaseList.Int64(i);

                SendMsgPBToGate(AFMsg::EGMI_ACK_ADD_ROW, xTableAddRow, identOther);
            }
        }
    }

    void AFCGameNetModule::CommonDataTableDeleteEvent(const AFGUID& self, const std::string& strTableName, int nRow, const AFCDataList& valueBroadCaseList)
    {
        AFMsg::EntityDataTableRemove xTableRemoveRow;
        xTableRemoveRow.set_entity_id(self);
        xTableRemoveRow.set_table_name(strTableName);
        xTableRemoveRow.add_remove_row(nRow);

        for (size_t i = 0; i < valueBroadCaseList.GetCount(); i++)
        {
            AFGUID identOther = valueBroadCaseList.Int64(i);

            SendMsgPBToGate(AFMsg::EGMI_ACK_REMOVE_ROW, xTableRemoveRow, identOther);
        }
    }

    void AFCGameNetModule::CommonDataTableSwapEvent(const AFGUID& self, const std::string& strTableName, int nRow, int target_row, const AFCDataList& valueBroadCaseList)
    {
        //swap 2 different rows
        AFMsg::EntityDataTableSwap xTableSwap;
        xTableSwap.set_entity_id(self);
        xTableSwap.set_origin_table_name(strTableName);
        xTableSwap.set_target_table_name(strTableName);
        xTableSwap.set_row_origin(nRow);
        xTableSwap.set_row_target(target_row);

        for (size_t i = 0; i < valueBroadCaseList.GetCount(); i++)
        {
            AFGUID identOther = valueBroadCaseList.Int64(i);
            SendMsgPBToGate(AFMsg::EGMI_ACK_SWAP_ROW, xTableSwap, identOther);
        }
    }

    void AFCGameNetModule::CommonDataTableUpdateEvent(const AFGUID& self, const std::string& strTableName, int nRow, int nCol, const AFIData& newVar, const AFCDataList& valueBroadCaseList)
    {
        AFMsg::EntityDataTable xTableChanged;
        xTableChanged.set_entity_id(self);
        xTableChanged.set_table_name(strTableName);
        AFMsg::PBCellData* pCellData = xTableChanged.add_table_cell_list();
        AFIMsgModule::TableCellToPBCell(newVar, nRow, nCol, *pCellData);

        for (size_t i = 0; i < valueBroadCaseList.GetCount(); i++)
        {
            AFGUID identOther = valueBroadCaseList.Int64(i);

            SendMsgPBToGate(AFMsg::EGMI_ACK_TABLE_DATA, xTableChanged, identOther);
        }
    }

    int AFCGameNetModule::OnCommonDataTableEvent(const AFGUID& self, const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar)
    {
        const std::string& strTableName = xEventData.strName.c_str();
        const int nOpType = xEventData.nOpType;
        const int nRow = xEventData.nRow;
        const int nCol = xEventData.nCol;

        int nObjectGroupID = m_pKernelModule->GetNodeInt(self, "GroupID");

        if (nObjectGroupID < 0)
        {
            return 1;
        }

        if (ark::Player::ThisName() == std::string(m_pKernelModule->GetNodeString(self, "ClassName")) && (m_pKernelModule->GetNodeInt(self, "LoadPropertyFinish") <= 0))
        {
            return 1;
        }

        AFCDataList valueBroadCaseList;
        GetTableBroadcastEntityList(self, strTableName, valueBroadCaseList);

        switch (nOpType)
        {
        case AFDataTable::TABLE_ADD:
            CommonDataTableAddEvent(self, strTableName, nRow, nCol, valueBroadCaseList);
            break;

        case AFDataTable::TABLE_DELETE:
            CommonDataTableDeleteEvent(self, strTableName, nRow, valueBroadCaseList);
            break;

        case AFDataTable::TABLE_SWAP:
            CommonDataTableSwapEvent(self, strTableName, nRow, nCol, valueBroadCaseList);
            break;

        case AFDataTable::TABLE_UPDATE:
            CommonDataTableUpdateEvent(self, strTableName, nRow, nCol, newVar, valueBroadCaseList);
            break;

        case AFDataTable::TABLE_COVERAGE:
            //will do something
            break;

        default:
            break;
        }

        return 0;
    }

    int AFCGameNetModule::CommonClassDestoryEvent(const AFGUID& self)
    {
        int nObjectContainerID = m_pKernelModule->GetNodeInt(self, "SceneID");
        int nObjectGroupID = m_pKernelModule->GetNodeInt(self, "GroupID");

        if (nObjectGroupID < 0)
        {
            //容器
            return 0;
        }

        AFCDataList valueAllObjectList;
        AFCDataList valueBroadCaseList;
        AFCDataList valueBroadListNoSelf;
        m_pMapModule->GetInstEntityList(nObjectContainerID, nObjectGroupID, valueAllObjectList);

        for (size_t i = 0; i < valueAllObjectList.GetCount(); i++)
        {
            AFGUID identBC = valueAllObjectList.Int64(i);
            const std::string strIdentClassName(m_pKernelModule->GetNodeString(identBC, "ClassName"));

            if (ark::Player::ThisName() == strIdentClassName)
            {
                valueBroadCaseList << identBC;

                if (identBC != self)
                {
                    valueBroadListNoSelf << identBC;
                }
            }
        }

        //如果是副本的怪，则不需要发送，因为会在离开副本的时候一次性一条消息发送
        OnEntityListLeave(valueBroadListNoSelf, AFCDataList() << self);
        return 0;
    }

    int AFCGameNetModule::OnCommonClassEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var)
    {
        switch (eClassEvent)
        {
        case ENTITY_EVT_DESTROY:
            CommonClassDestoryEvent(self);
            break;

        case ENTITY_EVT_PRE_LOAD_DATA:
            {
                //id和fd,gateid绑定
                ARK_SHARE_PTR<GateBaseInfo> pDataBase = mRoleBaseData.GetElement(self);

                if (nullptr != pDataBase)
                {
                    //回复客户端角色进入游戏世界成功了
                    AFMsg::AckEventResult xMsg;
                    xMsg.set_event_code(AFMsg::EVC_ENTER_GAME_SUCCESS);

                    xMsg.set_event_client(pDataBase->xClientID);
                    xMsg.set_event_object(self);

                    SendMsgPBToGate(AFMsg::EGMI_ACK_ENTER_GAME, xMsg, self);
                }
            }
            break;

        case ARK_ENTITY_EVENT::ENTITY_EVT_LOAD_DATA:
            break;

        case ARK_ENTITY_EVENT::ENTITY_EVT_DATA_FINISHED:
            {
                //自己广播给自己就够了
                if (strClassName == ark::Player::ThisName())
                {
                    OnEntityListEnter(AFCDataList() << self, AFCDataList() << self);

                    OnSelfDataNodeEnter(self);
                    OnSelfDataTableEnter(self);
                }
            }
            break;

        case ARK_ENTITY_EVENT::ENTITY_EVT_ALL_FINISHED:
            break;

        default:
            break;
        }

        return 0;
    }

    int AFCGameNetModule::OnGroupEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar)
    {
        //容器发生变化，只可能从A容器的0层切换到B容器的0层
        //需要注意的是------------任何层改变的时候，此玩家其实还未进入层，因此，层改变的时候获取的玩家列表，目标层是不包含自己的
        int nSceneID = m_pKernelModule->GetNodeInt(self, "SceneID");

        //广播给别人自己离去(层降或者跃层)
        int nOldGroupID = oldVar.GetInt();
        ProcessLeaveGroup(self, nSceneID, nOldGroupID);

        int nNewGroupID = newVar.GetInt();
        ProcessEnterGroup(self, nSceneID, nNewGroupID);

        return 0;
    }

    int AFCGameNetModule::OnContainerEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar)
    {
        //容器发生变化，只可能从A容器的0层切换到B容器的0层
        //需要注意的是------------任何容器改变的时候，玩家必须是0层
        int nOldSceneID = oldVar.GetInt();
        int nNowSceneID = newVar.GetInt();

        ARK_LOG_INFO("Enter Scene, id  = {} scene = {}", self, nNowSceneID);

        //自己消失,玩家不用广播，因为在消失之前，会回到0层，早已广播了玩家
        AFCDataList valueOldAllObjectList;
        AFCDataList valueNewAllObjectList;
        AFCDataList valueAllObjectListNoSelf;
        AFCDataList valuePlayerList;
        AFCDataList valuePlayerNoSelf;

        m_pMapModule->GetInstEntityList(nOldSceneID, 0, valueOldAllObjectList);
        m_pMapModule->GetInstEntityList(nNowSceneID, 0, valueNewAllObjectList);

        for (size_t i = 0; i < valueOldAllObjectList.GetCount(); i++)
        {
            AFGUID identBC = valueOldAllObjectList.Int64(i);

            if (identBC == self)
            {
                valueOldAllObjectList.SetInt64(i, 0);
            }
        }

        for (size_t i = 0; i < valueNewAllObjectList.GetCount(); i++)
        {
            AFGUID identBC = valueNewAllObjectList.Int64(i);
            const std::string strClassName(m_pKernelModule->GetNodeString(identBC, "ClassName"));

            if (ark::Player::ThisName() == strClassName)
            {
                valuePlayerList << identBC;

                if (identBC != self)
                {
                    valuePlayerNoSelf << identBC;
                }
            }

            if (identBC != self)
            {
                valueAllObjectListNoSelf << identBC;
            }
        }

        //////////////////////////////////////////////////////////////////////////

        //但是旧场景0层的NPC需要广播
        OnEntityListLeave(AFCDataList() << self, valueOldAllObjectList);

        //广播给所有人出现对象(如果是玩家，则包括广播给自己)
        //这里广播的都是0层的
        if (valuePlayerList.GetCount() > 0)
        {
            //把self广播给argVar这些人
            OnEntityListEnter(valuePlayerNoSelf, AFCDataList() << self);
        }

        //新层必然是0，把0层NPC广播给自己------------自己广播给自己不在这里广播，因为场景ID在跨场景时会经常变化

        //把valueAllObjectList广播给self
        OnEntityListEnter(AFCDataList() << self, valueAllObjectListNoSelf);

        ////////////////////把已经存在的人的属性广播给新来的人//////////////////////////////////////////////////////
        for (size_t i = 0; i < valueAllObjectListNoSelf.GetCount(); i++)
        {
            AFGUID identOld = valueAllObjectListNoSelf.Int64(i);
            OnViewDataNodeEnter(AFCDataList() << self, identOld);
            ////////////////////把已经存在的人的表广播给新来的人//////////////////////////////////////////////////////
            OnViewDataTableEnter(AFCDataList() << self, identOld);
        }

        //把新来的人的属性广播给周边的人()
        if (valuePlayerNoSelf.GetCount() > 0)
        {
            OnViewDataNodeEnter(valuePlayerNoSelf, self);
            OnViewDataTableEnter(valuePlayerNoSelf, self);
        }

        return 0;
    }

    int AFCGameNetModule::GetNodeBroadcastEntityList(const AFGUID& self, const std::string& name, AFIDataList& valueObject)
    {
        int nObjectContainerID = m_pKernelModule->GetNodeInt(self, "SceneID");
        int nObjectGroupID = m_pKernelModule->GetNodeInt(self, "GroupID");

        //普通场景容器，判断广播属性
        std::string strClassName = m_pKernelModule->GetNodeString(self, "ClassName");
        ARK_SHARE_PTR<AFIDataNodeManager> pClassDataNodeManager = m_pClassModule->GetNodeManager(strClassName);

        if (pClassDataNodeManager == nullptr)
        {
            return -1;
        }

        AFDataNode* pDataNode = pClassDataNodeManager->GetNode(name.c_str());

        if (nullptr == pDataNode)
        {
            return -1;
        }

        if (pDataNode->IsPublic())
        {
            //广播给客户端自己和周边人
            GetBroadcastEntityList(nObjectContainerID, nObjectGroupID, valueObject);
        }

        if (ark::Player::ThisName() == strClassName && pDataNode->IsPrivate())
        {
            valueObject.AddInt64(self);
        }

        return valueObject.GetCount();
    }

    int AFCGameNetModule::GetTableBroadcastEntityList(const AFGUID& self, const std::string& name, AFIDataList& valueObject)
    {
        int nObjectContainerID = m_pKernelModule->GetNodeInt(self, "SceneID");
        int nObjectGroupID = m_pKernelModule->GetNodeInt(self, "GroupID");

        //普通场景容器，判断广播属性
        std::string strClassName = m_pKernelModule->GetNodeString(self, "ClassName");

        ARK_SHARE_PTR<AFIDataTableManager> pClassDataTableManager = m_pClassModule->GetTableManager(strClassName);

        if (pClassDataTableManager == nullptr)
        {
            return -1;
        }

        AFDataTable* pDataTable = pClassDataTableManager->GetTable(name.c_str());

        if (pDataTable == nullptr)
        {
            return -1;
        }

        if (pDataTable->IsPublic())
        {
            //广播给客户端自己和周边人
            GetBroadcastEntityList(nObjectContainerID, nObjectGroupID, valueObject);
        }

        if (ark::Player::ThisName() == strClassName && pDataTable->IsPrivate())
        {
            valueObject.AddInt64(self);
        }

        return valueObject.GetCount();
    }
    int AFCGameNetModule::GetBroadcastEntityList(const int map_id, const int inst_id, AFIDataList& valueObject)
    {
        AFCDataList map_inst_entity_list;
        m_pMapModule->GetInstEntityList(map_id, inst_id, map_inst_entity_list);

        for (size_t i = 0; i < map_inst_entity_list.GetCount(); i++)
        {
            const std::string& strObjClassName = m_pKernelModule->GetNodeString(map_inst_entity_list.Int64(i), "ClassName");

            if (ark::Player::ThisName() == strObjClassName)
            {
                valueObject.AddInt64(map_inst_entity_list.Int64(i));
            }
        }

        return valueObject.GetCount();
    }

    int AFCGameNetModule::OnEntityEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var)
    {
        switch (eClassEvent)
        {
        case ENTITY_EVT_DESTROY:
            //save data to db by yourself logic
            ARK_LOG_INFO("Player Offline, player_id = {}", self);
            break;

        case ENTITY_EVT_LOAD_DATA:
            //load data from db by yourself logic
            ARK_LOG_INFO("Player online, player_id = {}", self);
            break;

        case ENTITY_EVT_ALL_FINISHED:
            m_pKernelModule->AddEventCallBack(self, AFED_ON_OBJECT_ENTER_SCENE_BEFORE, this, &AFCGameNetModule::OnSwapSceneResultEvent);
            break;

        default:
            break;
        }

        return 0;
    }

    int AFCGameNetModule::OnSwapSceneResultEvent(const AFGUID& self, const int nEventID, const AFIDataList& var)
    {
        if (var.GetCount() != 7 ||
                !var.TypeEx(AF_DATA_TYPE::DT_INT64, AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_INT,
                            AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_FLOAT, AF_DATA_TYPE::DT_FLOAT, AF_DATA_TYPE::DT_FLOAT, AF_DATA_TYPE::DT_UNKNOWN)
           )
        {
            return 1;
        }

        int nTargetScene = var.Int(2);
        int nTargetGroupID = var.Int(3);
        Point3D xPos;
        xPos.x = var.Float(4);
        xPos.y = var.Float(5);
        xPos.z = var.Float(6);

        AFMsg::ReqAckSwapScene xSwapScene;
        xSwapScene.set_transfer_type(AFMsg::ReqAckSwapScene::EGameSwapType::ReqAckSwapScene_EGameSwapType_EGST_NARMAL);
        xSwapScene.set_scene_id(nTargetScene);
        xSwapScene.set_line_id(nTargetGroupID);
        xSwapScene.set_x(xPos.x);
        xSwapScene.set_y(xPos.y);
        xSwapScene.set_z(xPos.z);

        SendMsgPBToGate(AFMsg::EGMI_ACK_SWAP_SCENE, xSwapScene, self);

        return 0;
    }

    void AFCGameNetModule::OnReqiureRoleListProcess(const AFNetMsg* msg, const int64_t session_id)
    {
        ////fd
        //ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ReqRoleList);
        //const AFGUID& nGateClientID = actor_id;
        //AFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;

        //if (!m_AccountModule->GetRoleList(x_msg.account(), xAckRoleLiteInfoList))
        //{
        //    ARK_LOG_ERROR("Get role list failed, player_id = {}", nGateClientID.ToString());
        //}

        //m_pNetServerService->SendPBMsg(AFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList, conn_id, nGateClientID);
    }

    void AFCGameNetModule::OnCreateRoleGameProcess(const AFNetMsg* msg, const int64_t session_id)
    {
        //ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ReqCreateRole);
        //const AFGUID& nGateClientID = actor_id;

        //AFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
        //AFCDataList varList;
        //varList.AddInt(x_msg.career());
        //varList.AddInt(x_msg.sex());
        //varList.AddInt(x_msg.race());
        //varList.AddString(x_msg.noob_name().c_str());
        //varList.AddInt(x_msg.game_id());

        //if (!m_AccountModule->CreateRole(x_msg.account(), xAckRoleLiteInfoList, varList))
        //{
        //    ARK_LOG_ERROR("create role failed, player_id = {}", nGateClientID.ToString());
        //}

        // m_pNetServerService->SendPBMsg(AFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList, conn_id, nGateClientID);
    }

    void AFCGameNetModule::OnDeleteRoleGameProcess(const AFNetMsg* msg, const int64_t session_id)
    {
        //ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ReqDeleteRole);

        //AFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;

        //if (!m_AccountModule->DeleteRole(x_msg.account(), xAckRoleLiteInfoList))
        //{
        //    ARK_LOG_ERROR("delete role failed, player_id = {}", actor_id.ToString());
        //}

        //m_pNetServerService->SendPBMsg(AFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList, conn_id, actor_id);
    }

    void AFCGameNetModule::OnClienSwapSceneProcess(const AFNetMsg* msg, const int64_t session_id)
    {
        //ARK_PROCESS_ACTOR_MSG(head, msg_id, msg, msg_len, AFMsg::ReqAckSwapScene);

        //AFCDataList varEntry;
        //varEntry << pEntity->Self();
        //varEntry << int32_t(0);
        //varEntry << x_msg.scene_id();
        //varEntry << int32_t(-1) ;
        //m_pKernelModule->DoEvent(pEntity->Self(), AFED_ON_CLIENT_ENTER_SCENE, varEntry);
    }

    void AFCGameNetModule::OnProxyServerRegisteredProcess(const AFNetMsg* msg, const int64_t session_id)
    {
        //ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ServerInfoReportList);

        //for (int i = 0; i < x_msg.server_list_size(); ++i)
        //{
        //    const AFMsg::ServerInfoReport& xData = x_msg.server_list(i);
        //    ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(xData.bus_id());

        //    if (nullptr == pServerData)
        //    {
        //        pServerData = std::make_shared<GateServerInfo>();
        //        mProxyMap.AddElement(xData.bus_id(), pServerData);
        //    }

        //    //pServerData->xServerData.conn_id_ = conn_id;
        //    //pServerData->xServerData.server_info_ = xData;

        //    //ARK_LOG_INFO("Proxy Registered, server_id = {}", xData.bus_id());
        //}
    }

    void AFCGameNetModule::OnProxyServerUnRegisteredProcess(const AFNetMsg* msg, const int64_t session_id)
    {
        //ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ServerInfoReportList);

        //for (int i = 0; i < x_msg.server_list_size(); ++i)
        //{
        //    const AFMsg::ServerInfoReport& xData = x_msg.server_list(i);
        //    mProxyMap.RemoveElement(xData.bus_id());
        //    ARK_LOG_INFO("Proxy UnRegistered, server_id = {}", xData.bus_id());
        //}
    }

    void AFCGameNetModule::OnRefreshProxyServerInfoProcess(const AFNetMsg* msg, const int64_t session_id)
    {
        //ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ServerInfoReportList);

        //for (int i = 0; i < x_msg.server_list_size(); ++i)
        //{
        //    const AFMsg::ServerInfoReport& xData = x_msg.server_list(i);
        //    ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(xData.bus_id());

        //    if (nullptr == pServerData)
        //    {
        //        pServerData = std::make_shared<GateServerInfo>();
        //        mProxyMap.AddElement(xData.bus_id(), pServerData);
        //    }

        //    //pServerData->xServerData.conn_id_ = conn_id;
        //    //pServerData->xServerData.server_info_ = xData;

        //    //ARK_LOG_INFO("Proxy Registered, server_id  = {}", xData.bus_id());
        //}
    }

    void AFCGameNetModule::SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const AFGUID& self)
    {
        ARK_SHARE_PTR<GateBaseInfo> pData = mRoleBaseData.GetElement(self);

        if (nullptr == pData)
        {
            return;
        }

        //ARK_SHARE_PTR<GateServerInfo> pProxyData = mProxyMap.GetElement(pData->nGateID);

        //if (nullptr != pProxyData)
        //{
        //    m_pNetServerService->SendPBMsg(nMsgID, xMsg, pProxyData->xServerData.conn_id_, pData->xClientID);
        //}
    }

    void AFCGameNetModule::SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const AFGUID& self)
    {
        ARK_SHARE_PTR<GateBaseInfo> pData = mRoleBaseData.GetElement(self);

        if (nullptr == pData)
        {
            return;
        }

        //ARK_SHARE_PTR<GateServerInfo> pProxyData = mProxyMap.GetElement(pData->nGateID);

        //if (nullptr != pProxyData)
        //{
        //    m_pNetServerService->SendMsg(nMsgID, strMsg, pProxyData->xServerData.conn_id_, pData->xClientID);
        //}
    }

    AFINetServerService* AFCGameNetModule::GetNetServerService()
    {
        return m_pNetServerService;
    }

    bool AFCGameNetModule::AddPlayerGateInfo(const AFGUID& nRoleID, const AFGUID& nClientID, const int nGateID)
    {
        if (nGateID <= 0)
        {
            //非法gate
            return false;
        }

        if (nClientID == 0)
        {
            return false;
        }

        ARK_SHARE_PTR<AFCGameNetModule::GateBaseInfo> pBaseData = mRoleBaseData.GetElement(nRoleID);

        if (nullptr != pBaseData)
        {
            //已经存在
            ARK_LOG_ERROR("player is already exist, cannot enter game again, id = {}", nClientID);
            return false;
        }

        ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(nGateID);

        if (nullptr == pServerData)
        {
            return false;
        }

        if (!pServerData->xRoleInfo.insert(std::make_pair(nRoleID, pServerData->xServerData.conn_id_)).second)
        {
            return false;
        }

        if (!mRoleBaseData.AddElement(nRoleID, std::make_shared<GateBaseInfo>(nGateID, nClientID)))
        {
            pServerData->xRoleInfo.erase(nRoleID);
            return false;
        }

        return true;
    }

    bool AFCGameNetModule::RemovePlayerGateInfo(const AFGUID& nRoleID)
    {
        ARK_SHARE_PTR<GateBaseInfo> pBaseData = mRoleBaseData.GetElement(nRoleID);

        if (nullptr == pBaseData)
        {
            return false;
        }

        mRoleBaseData.RemoveElement(nRoleID);

        ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(pBaseData->nGateID);

        if (nullptr == pServerData)
        {
            return false;
        }

        pServerData->xRoleInfo.erase(nRoleID);
        return true;
    }

    ARK_SHARE_PTR<AFIGameNetModule::GateBaseInfo> AFCGameNetModule::GetPlayerGateInfo(const AFGUID& nRoleID)
    {
        return mRoleBaseData.GetElement(nRoleID);
    }

    ARK_SHARE_PTR<AFIGameNetModule::GateServerInfo> AFCGameNetModule::GetGateServerInfo(const int nGateID)
    {
        return mProxyMap.GetElement(nGateID);
    }

    ARK_SHARE_PTR<AFIGameNetModule::GateServerInfo> AFCGameNetModule::GetGateServerInfoByClientID(const AFGUID& nClientID)
    {
        int nGateID = -1;
        ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.First();

        while (nullptr != pServerData)
        {
            if (nClientID == pServerData->xServerData.conn_id_)
            {
                nGateID = pServerData->xServerData.server_info_.bus_id();
                break;
            }

            pServerData = mProxyMap.Next();
        }

        if (nGateID == -1)
        {
            return nullptr;
        }

        return pServerData;
    }

    void AFCGameNetModule::OnTransWorld(const AFNetMsg* msg)
    {
        //ARK_PROCESS_ACTOR_STRING_MSG(head, msg, msg_len);
        //m_pGameServerToWorldModule->SendBySuit(nHasKey, nMsgID, msg, nLen);

        //TODO:transfer to world
        //m_pMsgModule->SendToWorld(nMsgID, msg, nLen);
    }

}