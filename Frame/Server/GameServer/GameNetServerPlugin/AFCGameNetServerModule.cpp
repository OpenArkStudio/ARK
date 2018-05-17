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

#include "AFCGameNetServerModule.h"
#include "SDK/Interface/AFIModule.h"
#include "SDK/Proto/ARKDataDefine.hpp"
#include "SDK/Core/AFDataNode.h"
#include "Server/Interface/AFEventDefine.h"

bool AFCGameNetServerModule::Init()
{
    m_pNetModule = ARK_NEW AFINetServerModule(pPluginManager);
    return true;
}

bool AFCGameNetServerModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pSceneProcessModule = pPluginManager->FindModule<AFISceneProcessModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pUUIDModule = pPluginManager->FindModule<AFIGUIDModule>();
    m_pGameServerToWorldModule = pPluginManager->FindModule<AFIGameServerToWorldModule>();
    m_AccountModule = pPluginManager->FindModule<AFIAccountModule>();

    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_PTWG_PROXY_REFRESH, this, &AFCGameNetServerModule::OnRefreshProxyServerInfoProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_PTWG_PROXY_REGISTERED, this, &AFCGameNetServerModule::OnProxyServerRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_PTWG_PROXY_UNREGISTERED, this, &AFCGameNetServerModule::OnProxyServerUnRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_ENTER_GAME, this, &AFCGameNetServerModule::OnClienEnterGameProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_LEAVE_GAME, this, &AFCGameNetServerModule::OnClienLeaveGameProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_ROLE_LIST, this, &AFCGameNetServerModule::OnReqiureRoleListProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_CREATE_ROLE, this, &AFCGameNetServerModule::OnCreateRoleGameProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_DELETE_ROLE, this, &AFCGameNetServerModule::OnDeleteRoleGameProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_RECOVER_ROLE, this, &AFCGameNetServerModule::OnClienSwapSceneProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_SWAP_SCENE, this, &AFCGameNetServerModule::OnClienSwapSceneProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_SEARCH_GUILD, this, &AFCGameNetServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGEC_REQ_CREATE_CHATGROUP, this, &AFCGameNetServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGEC_REQ_JOIN_CHATGROUP, this, &AFCGameNetServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGEC_REQ_LEAVE_CHATGROUP, this, &AFCGameNetServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGEC_REQ_SUBSCRIPTION_CHATGROUP, this, &AFCGameNetServerModule::OnTransWorld);

    m_pNetModule->AddEventCallBack(this, &AFCGameNetServerModule::OnSocketPSEvent);

    m_pKernelModule->RegCommonClassEvent(this, &AFCGameNetServerModule::OnCommonClassEvent);
    m_pKernelModule->RegCommonDataNodeEvent(this, &AFCGameNetServerModule::OnCommonDataNodeEvent);
    m_pKernelModule->RegCommonDataTableEvent(this, &AFCGameNetServerModule::OnCommonDataTableEvent);

    m_pKernelModule->AddClassCallBack(ARK::Player::ThisName(), this, &AFCGameNetServerModule::OnEntityEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr == xLogicClass)
    {
        return false;
    }

    AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
    std::string strConfigName;
    for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
    {
        const int nServerType = m_pElementModule->GetNodeInt(strConfigName, "Type");
        const int nServerID = m_pElementModule->GetNodeInt(strConfigName, "ServerID");
        if(nServerType == ARK_SERVER_TYPE::ARK_ST_GAME && pPluginManager->AppID() == nServerID)
        {
            const int nPort = m_pElementModule->GetNodeInt(strConfigName, "Port");
            const int nMaxConnect = m_pElementModule->GetNodeInt(strConfigName, "MaxOnline");
            const int nCpus = m_pElementModule->GetNodeInt(strConfigName, "CpuCount");
            const std::string strServerName(m_pElementModule->GetNodeString(strConfigName, "Name"));
            const std::string strIP(m_pElementModule->GetNodeString(strConfigName, "IP"));

            int nRet = m_pNetModule->Start(nMaxConnect, strIP, nPort, nServerID, nCpus);
            if(nRet < 0)
            {
                ARK_LOG_ERROR("Cannot init server net, Port = %d", nPort);
                ARK_ASSERT(nRet, "Cannot init server net", __FILE__, __FUNCTION__);
                exit(0);
            }
        }
    }

    return true;
}

bool AFCGameNetServerModule::Shut()
{
    return true;
}

void AFCGameNetServerModule::Update()
{
    m_pNetModule->Update();
}

void AFCGameNetServerModule::OnSocketPSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if(eEvent == DISCONNECTED)
    {
        ARK_LOG_INFO("Connection closed, id = %s", xClientID.ToString().c_str());
        OnClientDisconnect(xClientID);
    }
    else  if(eEvent == CONNECTED)
    {
        ARK_LOG_INFO("Connected success, id = %s", xClientID.ToString().c_str());
        OnClientConnected(xClientID);
    }
}

void AFCGameNetServerModule::OnClientDisconnect(const AFGUID& xClientID)
{
    //只可能是网关丢了
    int nServerID = 0;
    ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.First();
    while(nullptr != pServerData)
    {
        if(xClientID == pServerData->xServerData.xClient)
        {
            nServerID = pServerData->xServerData.pData->server_id();
            break;
        }

        pServerData = mProxyMap.Next();
    }

    mProxyMap.RemoveElement(nServerID);
}

void AFCGameNetServerModule::OnClientConnected(const AFGUID& xClientID)
{

}

void AFCGameNetServerModule::OnClienEnterGameProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //Before enter game, PlayerID means gate fd
    AFGUID nGateClientID;
    AFMsg::ReqEnterGameServer xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nGateClientID))
    {
        return;
    }

    AFGUID nRoleID = AFINetServerModule::PBToGUID(xMsg.id());

    if(m_pKernelModule->GetEntity(nRoleID))
    {
        m_pKernelModule->DestroyEntity(nRoleID);
    }

    //////////////////////////////////////////////////////////////////////////

    ARK_SHARE_PTR<AFCGameNetServerModule::GateBaseInfo>  pGateInfo = GetPlayerGateInfo(nRoleID);
    if(nullptr != pGateInfo)
    {
        if(RemovePlayerGateInfo(nRoleID))
        {
            ARK_LOG_ERROR("RemovePlayerGateInfo fail, id = %s", nRoleID.ToString().c_str());
        }
    }

    ARK_SHARE_PTR<AFCGameNetServerModule::GateServerInfo> pGateServereinfo = GetGateServerInfoByClientID(xClientID);
    if(nullptr == pGateServereinfo)
    {
        return;
    }

    int nGateID = -1;
    if(pGateServereinfo->xServerData.pData)
    {
        nGateID = pGateServereinfo->xServerData.pData->server_id();
    }

    if(nGateID < 0)
    {
        return;
    }

    if(!AddPlayerGateInfo(nRoleID, nGateClientID, nGateID))
    {
        return;
    }

    //默认1号场景
    int nSceneID = 1;
    AFCDataList var;
    var.AddString("Name");
    var.AddString(xMsg.name().c_str());

    var.AddString("GateID");
    var.AddInt(nGateID);

    var.AddString("ClientID");
    var.AddObject(nGateClientID);

    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->CreateEntity(nRoleID, nSceneID, 0, ARK::Player::ThisName(), "", var);
    if(nullptr == pEntity)
    {
        //内存泄漏
        //mRoleBaseData
        //mRoleFDData
        return;
    }

    pEntity->SetNodeInt("LoadPropertyFinish", 1);
    pEntity->SetNodeInt("GateID", nGateID);
    pEntity->SetNodeInt("GameID", pPluginManager->AppID());

    m_pKernelModule->DoEvent(pEntity->Self(), ARK::Player::ThisName(), ENTITY_EVT_ALL_FINISHED, AFCDataList());

    AFCDataList varEntry;
    varEntry << pEntity->Self();
    varEntry << int32_t(0);
    varEntry << (int32_t)nSceneID;
    varEntry << int32_t (-1);
    m_pKernelModule->DoEvent(pEntity->Self(), AFED_ON_CLIENT_ENTER_SCENE, varEntry);
}

void AFCGameNetServerModule::OnClienLeaveGameProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqLeaveGameServer xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    if(nPlayerID.IsNULL())
    {
        return;
    }

    if(m_pKernelModule->GetEntity(nPlayerID))
    {
        m_pKernelModule->DestroyEntity(nPlayerID);
    }

    if(!RemovePlayerGateInfo(nPlayerID))
    {
        ARK_LOG_ERROR("RemovePlayerGateInfo failed, id = %s", nPlayerID.ToString().c_str());
    }
}

int AFCGameNetServerModule::OnDataNodeEnter(const AFIDataList& argVar, const AFGUID& self)
{
    if(argVar.GetCount() <= 0 || self.IsNULL())
    {
        return 0;
    }

    AFMsg::MultiObjectPropertyList xPublicMsg;
    AFMsg::MultiObjectPropertyList xPrivateMsg;

    //分为自己和外人
    //1.public发送给所有人
    //2.如果自己在列表中，再次发送private数据
    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);
    if (nullptr == pEntity)
    {
        return 0;
    }

    AFMsg::ObjectPropertyList* pPublicData = xPublicMsg.add_multi_player_property();
    AFMsg::ObjectPropertyList* pPrivateData = xPrivateMsg.add_multi_player_property();

    *(pPublicData->mutable_player_id()) = AFINetServerModule::GUIDToPB(self);
    *(pPrivateData->mutable_player_id()) = AFINetServerModule::GUIDToPB(self);

    ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager = pEntity->GetNodeManager();

    for(size_t i = 0; i < pNodeManager->GetNodeCount(); i++)
    {
        AFDataNode* pNode = pNodeManager->GetNodeByIndex(i);
        if(pNode == nullptr)
        {
            continue;
        }

        if(pNode->Changed())
        {
            if(pNode->IsPublic())
            {
                AFMsg::PropertyPBData* pDataInt = pPublicData->add_property_data_list();
                AFINetServerModule::DataToPBProperty(pNode->GetValue(), pNode->GetName().c_str(), *pDataInt);
            }

            if(pNode->IsPrivate())
            {
                AFMsg::PropertyPBData* pDataInt = pPrivateData->add_property_data_list();
                AFINetServerModule::DataToPBProperty(pNode->GetValue(), pNode->GetName().c_str(), *pDataInt);
            }
        }
    }

    for(size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOther = argVar.Object(i);
        if(self == identOther)
        {
            //找到他所在网关的FD
            SendMsgPBToGate(AFMsg::EGMI_ACK_OBJECT_PROPERTY_ENTRY, xPrivateMsg, identOther);
        }
        else
        {
            SendMsgPBToGate(AFMsg::EGMI_ACK_OBJECT_PROPERTY_ENTRY, xPublicMsg, identOther);
        }
    }

    return 0;
}

bool OnEnterPackDataTable(AFDataTable* pTable, AFMsg::ObjectRecordBase* pEntityTableBase)
{
    if(pTable == nullptr || pEntityTableBase == nullptr)
    {
        return false;
    }

    for(size_t i = 0; i < pTable->GetRowCount(); i++)
    {
        AFMsg::RecordAddRowStruct* pAddRowStruct = pEntityTableBase->add_row_struct();
        pAddRowStruct->set_row(i);
        for(size_t j = 0; j < pTable->GetColCount(); j++)
        {
            AFMsg::RecordPBData* pAddData = pAddRowStruct->add_record_data_list();

            AFCData xRowColData;
            if(!pTable->GetValue(i, j, xRowColData))
            {
                ARK_ASSERT(0, "Get record value failed, please check", __FILE__, __FUNCTION__);
                continue;
            }

            AFINetServerModule::RecordToPBRecord(xRowColData, i, j, *pAddData);
        }
    }

    return true;
}

int AFCGameNetServerModule::OnDataTableEnter(const AFIDataList& argVar, const AFGUID& self)
{
    if(argVar.GetCount() <= 0 || self.IsNULL())
    {
        return 0;
    }

    AFMsg::MultiObjectRecordList xPublicMsg;
    AFMsg::MultiObjectRecordList xPrivateMsg;

    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);
    if(nullptr == pEntity)
    {
        return 0;
    }

    AFMsg::ObjectRecordList* pPublicData = nullptr;
    AFMsg::ObjectRecordList* pPrivateData = nullptr;

    ARK_SHARE_PTR<AFIDataTableManager> pTableManager = pEntity->GetTableManager();

    size_t nTableCount = pTableManager->GetCount();
    for(size_t i = 0; i < nTableCount; ++i)
    {
        AFDataTable* pTable = pTableManager->GetTableByIndex(i);
        if(pTable == nullptr)
        {
            continue;
        }

        if(!pTable->IsPublic() && !pTable->IsPrivate())
        {
            continue;
        }

        AFMsg::ObjectRecordBase* pPrivateTableBase = nullptr;
        AFMsg::ObjectRecordBase* pPublicTableBase = nullptr;
        if(pTable->IsPublic())
        {
            if(!pPublicData)
            {
                pPublicData = xPublicMsg.add_multi_player_record();
                *(pPublicData->mutable_player_id()) = AFINetServerModule::GUIDToPB(self);
            }
            pPublicTableBase = pPublicData->add_record_list();
            pPublicTableBase->set_record_name(pTable->GetName());

            if(!OnEnterPackDataTable(pTable, pPublicTableBase))
            {
                ARK_LOG_ERROR("OnEnterPackDataTable failed, id = %s", self.ToString().c_str());
                return -1;
            }
        }

        if(pTable->IsPrivate())
        {
            if(!pPrivateData)
            {
                pPrivateData = xPrivateMsg.add_multi_player_record();
                *(pPrivateData->mutable_player_id()) = AFINetServerModule::GUIDToPB(self);
            }
            pPrivateTableBase = pPrivateData->add_record_list();
            pPrivateTableBase->set_record_name(pTable->GetName());

            if(OnEnterPackDataTable(pTable, pPrivateTableBase))
            {
                ARK_LOG_ERROR("OnRecordEnterPack failed, id = %s", self.ToString().c_str());
                return -1;
            }
        }
    }

    for(size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOther = argVar.Object(i);
        if(self == identOther)
        {
            if(xPrivateMsg.multi_player_record_size() > 0)
            {
                SendMsgPBToGate(AFMsg::EGMI_ACK_OBJECT_RECORD_ENTRY, xPrivateMsg, identOther);
            }
        }
        else
        {
            if(xPublicMsg.multi_player_record_size() > 0)
            {
                SendMsgPBToGate(AFMsg::EGMI_ACK_OBJECT_RECORD_ENTRY, xPublicMsg, identOther);
            }
        }
    }

    return 0;
}

int AFCGameNetServerModule::OnEntityListEnter(const AFIDataList& self, const AFIDataList& argVar)
{
    if(self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 0;
    }

    AFMsg::AckPlayerEntryList xPlayerEntryInfoList;
    for(size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOld = argVar.Object(i);
        //排除空对象
        if(identOld.IsNULL())
        {
            continue;
        }

        AFMsg::PlayerEntryInfo* pEntryInfo = xPlayerEntryInfoList.add_object_list();
        *(pEntryInfo->mutable_object_guid()) = AFINetServerModule::GUIDToPB(identOld);
        Point3D xPoint;
        xPoint.x = m_pKernelModule->GetNodeFloat(identOld, "x");
        xPoint.y = m_pKernelModule->GetNodeFloat(identOld, "y");
        xPoint.z = m_pKernelModule->GetNodeFloat(identOld, "z");

        *pEntryInfo->mutable_pos() = AFINetServerModule::VecToPB(xPoint);
        pEntryInfo->set_career_type(m_pKernelModule->GetNodeInt(identOld, "Job"));
        pEntryInfo->set_player_state(m_pKernelModule->GetNodeInt(identOld, "State"));
        pEntryInfo->set_config_id(m_pKernelModule->GetNodeString(identOld, "ConfigID"));
        pEntryInfo->set_scene_id(m_pKernelModule->GetNodeInt(identOld, "SceneID"));
        pEntryInfo->set_class_id(m_pKernelModule->GetNodeString(identOld, "ClassName"));
    }

    if(xPlayerEntryInfoList.object_list_size() <= 0)
    {
        return 0;
    }

    for(size_t i = 0; i < self.GetCount(); i++)
    {
        AFGUID ident = self.Object(i);
        if(ident.IsNULL())
        {
            continue;
        }

        //可能在不同的网关呢,得到后者所在的网关FD
        SendMsgPBToGate(AFMsg::EGMI_ACK_OBJECT_ENTRY, xPlayerEntryInfoList, ident);
    }

    return 1;
}

int AFCGameNetServerModule::OnEntityListLeave(const AFIDataList& self, const AFIDataList& argVar)
{
    if(self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 0;
    }

    AFMsg::AckPlayerLeaveList xPlayerLeaveInfoList;
    for(size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOld = argVar.Object(i);
        //排除空对象
        if(identOld.IsNULL())
        {
            continue;
        }

        AFMsg::Ident* pIdent = xPlayerLeaveInfoList.add_object_list();
        *pIdent = AFINetServerModule::GUIDToPB(argVar.Object(i));
    }

    for(size_t i = 0; i < self.GetCount(); i++)
    {
        AFGUID ident = self.Object(i);
        if(ident.IsNULL())
        {
            continue;
        }
        //可能在不同的网关呢,得到后者所在的网关FD
        SendMsgPBToGate(AFMsg::EGMI_ACK_OBJECT_LEAVE, xPlayerLeaveInfoList, ident);
    }

    return 1;
}

int AFCGameNetServerModule::OnCommonDataNodeEvent(const AFGUID& self, const std::string& name, const AFIData& oldVar, const AFIData& newVar)
{
    //if ( ARK::Player::ThisName() == m_pKernelModule->GetNodeString( self, "ClassName" ) )
    {
        if("GroupID" == name)
        {
            //自己还是要知道自己的这个属性变化的,但是别人就不需要知道了
            OnGroupEvent(self, name, oldVar, newVar);
        }

        if("SceneID" == name)
        {
            //自己还是要知道自己的这个属性变化的,但是别人就不需要知道了
            OnContainerEvent(self, name, oldVar, newVar);
        }

        if(ARK::Player::ThisName() == std::string(m_pKernelModule->GetNodeString(self, "ClassName")))
        {
            if(m_pKernelModule->GetNodeInt(self, "LoadPropertyFinish") <= 0)
            {
                return 0;
            }
        }
    }

    AFCDataList valueBroadCaseList;
    int nCount = 0;//argVar.GetCount() ;
    if(nCount <= 0)
    {
        nCount = GetBroadcastEntityList(self, name, false, valueBroadCaseList);
    }
    else
    {
        //传入的参数是要广播的对象列表
        //valueBroadCaseList = argVar;
    }

    if(valueBroadCaseList.GetCount() <= 0)
    {
        return 0;
    }

    AFMsg::ObjectPropertyPBData xPropertyData;
    AFMsg::Ident* pIdent = xPropertyData.mutable_player_id();
    *pIdent = AFINetServerModule::GUIDToPB(self);
    AFMsg::PropertyPBData* pData = xPropertyData.add_property_list();
    AFINetServerModule::DataToPBProperty(oldVar, name.c_str(), *pData);

    for(size_t i = 0; i < valueBroadCaseList.GetCount(); i++)
    {
        AFGUID identOld = valueBroadCaseList.Object(i);

        SendMsgPBToGate(AFMsg::EGMI_ACK_PROPERTY_DATA, xPropertyData, identOld);
    }

    return 0;
}

int AFCGameNetServerModule::OnCommonDataTableEvent(const AFGUID& self, const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar)
{
    const std::string& strTableName = xEventData.strName.c_str();
    const int nOpType = xEventData.nOpType;
    const int nRow = xEventData.nRow;
    const int nCol = xEventData.nCol;

    int nObjectContainerID = m_pKernelModule->GetNodeInt(self, "SceneID");
    int nObjectGroupID = m_pKernelModule->GetNodeInt(self, "GroupID");

    if(nObjectGroupID < 0)
    {
        //容器
        return 0;
    }

    if(ARK::Player::ThisName() == std::string(m_pKernelModule->GetNodeString(self, "ClassName")))
    {
        if(m_pKernelModule->GetNodeInt(self, "LoadPropertyFinish") <= 0)
        {
            return 0;
        }
    }

    AFCDataList valueBroadCaseList;
    GetBroadcastEntityList(self, strTableName, true, valueBroadCaseList);

    switch(nOpType)
    {
    case AFDataTable::TABLE_ADD:
        {
            AFMsg::ObjectRecordAddRow xAddRecordRow;
            AFMsg::Ident* pIdent = xAddRecordRow.mutable_player_id();
            *pIdent = AFINetServerModule::GUIDToPB(self);

            xAddRecordRow.set_record_name(strTableName);

            AFMsg::RecordAddRowStruct* pAddRowData = xAddRecordRow.add_row_data();
            pAddRowData->set_row(nRow);

            //add row 需要完整的row
            AFDataTable* pTable = m_pKernelModule->FindTable(self, strTableName);
            if(pTable != nullptr)
            {
                AFCDataList xRowDataList;
                if(pTable->QueryRow(nRow, xRowDataList))
                {
                    for(size_t i = 0; i < xRowDataList.GetCount(); i++)
                    {
                        AFMsg::RecordPBData* pAddData = pAddRowData->add_record_data_list();
                        AFINetServerModule::RecordToPBRecord(xRowDataList, nRow, nCol, *pAddData);
                    }

                    for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
                    {
                        AFGUID identOther = valueBroadCaseList.Object(i);

                        SendMsgPBToGate(AFMsg::EGMI_ACK_ADD_ROW, xAddRecordRow, identOther);
                    }
                }
            }
        }
        break;
    case AFDataTable::TABLE_DELETE:
        {
            AFMsg::ObjectRecordRemove xReoveRecordRow;

            AFMsg::Ident* pIdent = xReoveRecordRow.mutable_player_id();
            *pIdent = AFINetServerModule::GUIDToPB(self);

            xReoveRecordRow.set_record_name(strTableName);
            xReoveRecordRow.add_remove_row(nRow);

            for(size_t i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                AFGUID identOther = valueBroadCaseList.Object(i);

                SendMsgPBToGate(AFMsg::EGMI_ACK_REMOVE_ROW, xReoveRecordRow, identOther);
            }
        }
        break;
    case AFDataTable::TABLE_SWAP:
        {
            //其实是2个row交换
            AFMsg::ObjectRecordSwap xSwapRecord;
            *xSwapRecord.mutable_player_id() = AFINetServerModule::GUIDToPB(self);

            xSwapRecord.set_origin_record_name(strTableName);
            xSwapRecord.set_target_record_name(strTableName);   // 暂时没用
            xSwapRecord.set_row_origin(nRow);
            xSwapRecord.set_row_target(nCol);

            for(size_t i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                AFGUID identOther = valueBroadCaseList.Object(i);

                SendMsgPBToGate(AFMsg::EGMI_ACK_SWAP_ROW, xSwapRecord, identOther);
            }
        }
        break;
    case AFDataTable::TABLE_UPDATE:
        {
            AFMsg::ObjectRecordPBData xRecordChanged;
            *xRecordChanged.mutable_player_id() = AFINetServerModule::GUIDToPB(self);
            xRecordChanged.set_record_name(strTableName);
            AFMsg::RecordPBData* recordProperty = xRecordChanged.add_record_list();
            AFINetServerModule::RecordToPBRecord(newVar, nRow, nCol, *recordProperty);

            for(size_t i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                AFGUID identOther = valueBroadCaseList.Object(i);

                SendMsgPBToGate(AFMsg::EGMI_ACK_RECORD_DATA, xRecordChanged, identOther);
            }
        }
        break;
    case AFDataTable::TABLE_COVERAGE:
        //TODO:
        break;
    default:
        break;
    }

    return 0;
}

int AFCGameNetServerModule::OnCommonClassEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var)
{
    ////////////1:广播给已经存在的人//////////////////////////////////////////////////////////////
    if(ENTITY_EVT_DESTROY == eClassEvent)
    {
        //删除在线标志

        //////////////////////////////////////////////////////////////////////////

        int nObjectContainerID = m_pKernelModule->GetNodeInt(self, "SceneID");
        int nObjectGroupID = m_pKernelModule->GetNodeInt(self, "GroupID");

        if(nObjectGroupID < 0)
        {
            //容器
            return 0;
        }

        AFCDataList valueAllObjectList;
        AFCDataList valueBroadCaseList;
        AFCDataList valueBroadListNoSelf;
        m_pKernelModule->GetGroupEntityList(nObjectContainerID, nObjectGroupID, valueAllObjectList);

        for(size_t i = 0; i < valueAllObjectList.GetCount(); i++)
        {
            AFGUID identBC = valueAllObjectList.Object(i);
            const std::string strIdentClassName(m_pKernelModule->GetNodeString(identBC, "ClassName"));
            if(ARK::Player::ThisName() == strIdentClassName)
            {
                valueBroadCaseList << identBC;
                if(identBC != self)
                {
                    valueBroadListNoSelf << identBC;
                }
            }
        }

        //如果是副本的怪，则不需要发送，因为会在离开副本的时候一次性一条消息发送
        OnEntityListLeave(valueBroadListNoSelf, AFCDataList() << self);
    }

    else if(ARK_ENTITY_EVENT::ENTITY_EVT_PRE_LOAD_DATA == eClassEvent)
    {
        //id和fd,gateid绑定
        ARK_SHARE_PTR<GateBaseInfo> pDataBase = mRoleBaseData.GetElement(self);
        if(nullptr != pDataBase)
        {
            //回复客户端角色进入游戏世界成功了
            AFMsg::AckEventResult xMsg;
            xMsg.set_event_code(AFMsg::EGEC_ENTER_GAME_SUCCESS);

            *xMsg.mutable_event_client() = AFINetServerModule::GUIDToPB(pDataBase->xClientID);
            *xMsg.mutable_event_object() = AFINetServerModule::GUIDToPB(self);

            SendMsgPBToGate(AFMsg::EGMI_ACK_ENTER_GAME, xMsg, self);
        }
    }
    else if(ARK_ENTITY_EVENT::ENTITY_EVT_LOAD_DATA == eClassEvent)
    {
    }
    else if(ARK_ENTITY_EVENT::ENTITY_EVT_DATA_FINISHED == eClassEvent)
    {
        //自己广播给自己就够了
        if(strClassName == ARK::Player::ThisName())
        {
            OnEntityListEnter(AFCDataList() << self, AFCDataList() << self);

            OnDataNodeEnter(AFCDataList() << self, self);
            OnDataTableEnter(AFCDataList() << self, self);
        }
    }
    else if(ARK_ENTITY_EVENT::ENTITY_EVT_ALL_FINISHED == eClassEvent)
    {

    }
    return 0;
}

int AFCGameNetServerModule::OnGroupEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar)
{
    //容器发生变化，只可能从A容器的0层切换到B容器的0层
    //需要注意的是------------任何层改变的时候，此玩家其实还未进入层，因此，层改变的时候获取的玩家列表，目标层是不包含自己的
    int nSceneID = m_pKernelModule->GetNodeInt(self, "SceneID");

    //广播给别人自己离去(层降或者跃层)
    int nOldGroupID = oldVar.GetInt();
    if(nOldGroupID > 0)
    {
        AFCDataList valueAllOldObjectList;
        AFCDataList valueAllOldPlayerList;
        m_pKernelModule->GetGroupEntityList(nSceneID, nOldGroupID, valueAllOldObjectList);
        if(valueAllOldObjectList.GetCount() > 0)
        {
            //自己只需要广播其他玩家
            for(size_t i = 0; i < valueAllOldObjectList.GetCount(); i++)
            {
                AFGUID identBC = valueAllOldObjectList.Object(i);

                if(valueAllOldObjectList.Object(i) == self)
                {
                    valueAllOldObjectList.SetObject(i, AFGUID());
                }

                const std::string strClassName(m_pKernelModule->GetNodeString(identBC, "ClassName"));
                if(ARK::Player::ThisName() == strClassName)
                {
                    valueAllOldPlayerList << identBC;
                }
            }

            OnEntityListLeave(valueAllOldPlayerList, AFCDataList() << self);

            //老的全部要广播删除
            OnEntityListLeave(AFCDataList() << self, valueAllOldObjectList);
        }

        m_pKernelModule->DoEvent(self, AFED_ON_CLIENT_LEAVE_SCENE, AFCDataList() << nOldGroupID);
    }

    //再广播给别人自己出现(层升或者跃层)
    int nNewGroupID = newVar.GetInt();
    if(nNewGroupID > 0)
    {
        //这里需要把自己从广播中排除
        //////////////////////////////////////////////////////////////////////////
        AFCDataList valueAllObjectList;
        AFCDataList valueAllObjectListNoSelf;
        AFCDataList valuePlayerList;
        AFCDataList valuePlayerListNoSelf;
        m_pKernelModule->GetGroupEntityList(nSceneID, nNewGroupID, valueAllObjectList);
        for(size_t i = 0; i < valueAllObjectList.GetCount(); i++)
        {
            AFGUID identBC = valueAllObjectList.Object(i);
            const std::string strClassName(m_pKernelModule->GetNodeString(identBC, "ClassName"));
            if(ARK::Player::ThisName() == strClassName)
            {
                valuePlayerList << identBC;
                if(identBC != self)
                {
                    valuePlayerListNoSelf << identBC;
                }
            }

            if(identBC != self)
            {
                valueAllObjectListNoSelf << identBC;
            }
        }

        //广播给别人,自己出现(这里本不应该广播给自己)
        if(valuePlayerListNoSelf.GetCount() > 0)
        {
            OnEntityListEnter(valuePlayerListNoSelf, AFCDataList() << self);
        }

        const std::string strSelfClassName(m_pKernelModule->GetNodeString(self, "ClassName"));

        //广播给自己,所有的别人出现
        if(valueAllObjectListNoSelf.GetCount() > 0)
        {
            if(strSelfClassName == ARK::Player::ThisName())
            {
                OnEntityListEnter(AFCDataList() << self, valueAllObjectListNoSelf);
            }
        }

        if(strSelfClassName == ARK::Player::ThisName())
        {
            for(size_t i = 0; i < valueAllObjectListNoSelf.GetCount(); i++)
            {
                //此时不用再广播自己的属性给自己
                //把已经存在的人的属性广播给新来的人
                AFGUID identOld = valueAllObjectListNoSelf.Object(i);

                OnDataNodeEnter(AFCDataList() << self, identOld);
                //把已经存在的人的表广播给新来的人
                OnDataTableEnter(AFCDataList() << self, identOld);
            }
        }

        //把新来的人的属性广播给周边的人
        if(valuePlayerListNoSelf.GetCount() > 0)
        {
            OnDataNodeEnter(valuePlayerListNoSelf, self);
            OnDataTableEnter(valuePlayerListNoSelf, self);
        }
    }

    return 0;
}

int AFCGameNetServerModule::OnContainerEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar)
{
    //容器发生变化，只可能从A容器的0层切换到B容器的0层
    //需要注意的是------------任何容器改变的时候，玩家必须是0层
    int nOldSceneID = oldVar.GetInt();
    int nNowSceneID = newVar.GetInt();

    ARK_LOG_INFO("Enter Scene, id = %s scene = %d", self.ToString().c_str(), nNowSceneID);

    //自己消失,玩家不用广播，因为在消失之前，会回到0层，早已广播了玩家
    AFCDataList valueOldAllObjectList;
    AFCDataList valueNewAllObjectList;
    AFCDataList valueAllObjectListNoSelf;
    AFCDataList valuePlayerList;
    AFCDataList valuePlayerNoSelf;

    m_pKernelModule->GetGroupEntityList(nOldSceneID, 0, valueOldAllObjectList);
    m_pKernelModule->GetGroupEntityList(nNowSceneID, 0, valueNewAllObjectList);

    for(size_t i = 0; i < valueOldAllObjectList.GetCount(); i++)
    {
        AFGUID identBC = valueOldAllObjectList.Object(i);
        if(identBC == self)
        {
            valueOldAllObjectList.SetObject(i, AFGUID());
        }
    }

    for(size_t i = 0; i < valueNewAllObjectList.GetCount(); i++)
    {
        AFGUID identBC = valueNewAllObjectList.Object(i);
        const std::string strClassName(m_pKernelModule->GetNodeString(identBC, "ClassName"));
        if(ARK::Player::ThisName() == strClassName)
        {
            valuePlayerList << identBC;
            if(identBC != self)
            {
                valuePlayerNoSelf << identBC;
            }
        }

        if(identBC != self)
        {
            valueAllObjectListNoSelf << identBC;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    //但是旧场景0层的NPC需要广播
    OnEntityListLeave(AFCDataList() << self, valueOldAllObjectList);

    //广播给所有人出现对象(如果是玩家，则包括广播给自己)
    //这里广播的都是0层的
    if(valuePlayerList.GetCount() > 0)
    {
        //把self广播给argVar这些人
        OnEntityListEnter(valuePlayerNoSelf, AFCDataList() << self);
    }

    //新层必然是0，把0层NPC广播给自己------------自己广播给自己不在这里广播，因为场景ID在跨场景时会经常变化

    //把valueAllObjectList广播给self
    OnEntityListEnter(AFCDataList() << self, valueAllObjectListNoSelf);

    ////////////////////把已经存在的人的属性广播给新来的人//////////////////////////////////////////////////////
    for(size_t i = 0; i < valueAllObjectListNoSelf.GetCount(); i++)
    {
        AFGUID identOld = valueAllObjectListNoSelf.Object(i);
        OnDataNodeEnter(AFCDataList() << self, identOld);
        ////////////////////把已经存在的人的表广播给新来的人//////////////////////////////////////////////////////
        OnDataTableEnter(AFCDataList() << self, identOld);
    }

    //把新来的人的属性广播给周边的人()
    if(valuePlayerNoSelf.GetCount() > 0)
    {
        OnDataNodeEnter(valuePlayerNoSelf, self);
        OnDataTableEnter(valuePlayerNoSelf, self);
    }

    return 0;
}

int AFCGameNetServerModule::GetBroadcastEntityList(const AFGUID& self, const std::string& name, const bool bTable, AFIDataList& valueObject)
{
    int nObjectContainerID = m_pKernelModule->GetNodeInt(self, "SceneID");
    int nObjectGroupID = m_pKernelModule->GetNodeInt(self, "GroupID");

    //普通场景容器，判断广播属性
    std::string strClassName = m_pKernelModule->GetNodeString(self, "ClassName");

    ARK_SHARE_PTR<AFIDataNodeManager> pClassDataNodeManager = m_pClassModule->GetNodeManager(strClassName);
    ARK_SHARE_PTR<AFIDataTableManager> pClassDataTableManager = m_pClassModule->GetTableManager(strClassName);

    AFDataNode* pDataNode = nullptr;
    AFDataTable* pDataTable = nullptr;
    if(bTable)
    {
        if(pClassDataTableManager == nullptr)
        {
            return -1;
        }

        pDataTable = pClassDataTableManager->GetTable(name.c_str());
        if(pDataTable == nullptr)
        {
            return -1;
        }
    }
    else
    {
        if(pClassDataNodeManager == nullptr)
        {
            return -1;
        }

        pDataNode = pClassDataNodeManager->GetNode(name.c_str());
        if(nullptr == pDataNode)
        {
            return -1;
        }
    }

    if(ARK::Player::ThisName() == strClassName)
    {
        if(bTable)
        {
            if(pDataTable->IsPublic())
            {
                int nCount = GetBroadcastEntityList(nObjectContainerID, nObjectGroupID, valueObject);
                if(nCount < 0)
                {
                    //TODO:log
                }
            }
            else if(pDataTable->IsPrivate())
            {
                valueObject.AddObject(self);
            }
        }
        else
        {
            if(pDataNode->IsPublic())
            {
                GetBroadcastEntityList(nObjectContainerID, nObjectGroupID, valueObject);
            }
            else if(pDataNode->IsPrivate())
            {
                valueObject.AddObject(self);
            }
        }
        //一个玩家都不广播
        return valueObject.GetCount();
    }

    //不是玩家,NPC和怪物类等
    if(bTable)
    {
        if(pDataTable->IsPublic())
        {
            //广播给客户端自己和周边人
            GetBroadcastEntityList(nObjectContainerID, nObjectGroupID, valueObject);
        }
    }
    else
    {
        if(pDataNode->IsPublic())
        {
            //广播给客户端自己和周边人
            GetBroadcastEntityList(nObjectContainerID, nObjectGroupID, valueObject);
        }
    }

    return valueObject.GetCount();
}

int AFCGameNetServerModule::GetBroadcastEntityList(const int nObjectContainerID, const int nGroupID, AFIDataList& valueObject)
{
    AFCDataList valContainerObjectList;
    m_pKernelModule->GetGroupEntityList(nObjectContainerID, nGroupID, valContainerObjectList);
    for(size_t i = 0; i < valContainerObjectList.GetCount(); i++)
    {
        const std::string& strObjClassName = m_pKernelModule->GetNodeString(valContainerObjectList.Object(i), "ClassName");
        if(ARK::Player::ThisName() == strObjClassName)
        {
            valueObject.AddObject(valContainerObjectList.Object(i));
        }
    }

    return valueObject.GetCount();
}

int AFCGameNetServerModule::OnEntityEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var)
{
    if(ENTITY_EVT_DESTROY == eClassEvent)
    {
        //SaveDataToNoSql( self, true );
        ARK_LOG_INFO("Player Offline, player_id = %s", self.ToString().c_str());
    }
    else if(ENTITY_EVT_LOAD_DATA == eClassEvent)
    {
        //LoadDataFormNoSql( self );
    }
    else if(ENTITY_EVT_ALL_FINISHED == eClassEvent)
    {
        m_pKernelModule->AddEventCallBack(self, AFED_ON_OBJECT_ENTER_SCENE_BEFORE, this, &AFCGameNetServerModule::OnSwapSceneResultEvent);
    }

    return 0;
}

int AFCGameNetServerModule::OnSwapSceneResultEvent(const AFGUID& self, const int nEventID, const AFIDataList& var)
{
    if(var.GetCount() != 7 ||
            !var.TypeEx(AF_DATA_TYPE::DT_OBJECT, AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_INT,
                        AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_FLOAT, AF_DATA_TYPE::DT_FLOAT, AF_DATA_TYPE::DT_FLOAT, AF_DATA_TYPE::DT_UNKNOWN)
      )
    {
        return 1;
    }

    AFGUID ident = var.Object(0);
    int nType = var.Int(1);
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

void AFCGameNetServerModule::OnReqiureRoleListProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //fd
    AFGUID nGateClientID;
    AFMsg::ReqRoleList xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nGateClientID))
    {
        return;
    }
    AFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
    if(!m_AccountModule->GetRoleList(xMsg.account(), xAckRoleLiteInfoList))
    {
        ARK_LOG_ERROR("Get role list failed, player_id = %s", nGateClientID.ToString().c_str());
    }

    m_pNetModule->SendMsgPB(AFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList, xClientID, nGateClientID);
}

void AFCGameNetServerModule::OnCreateRoleGameProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nGateClientID;
    AFMsg::ReqCreateRole xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nGateClientID))
    {
        return;
    }

    AFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
    AFCDataList varList;
    varList.AddInt(xMsg.career());
    varList.AddInt(xMsg.sex());
    varList.AddInt(xMsg.race());
    varList.AddString(xMsg.noob_name().c_str());
    varList.AddInt(xMsg.game_id());
    if(!m_AccountModule->CreateRole(xMsg.account(), xAckRoleLiteInfoList, varList))
    {
        ARK_LOG_ERROR("create role failed, player_id = %s", nGateClientID.ToString().c_str());
    }

    m_pNetModule->SendMsgPB(AFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList, xClientID, nGateClientID);
}

void AFCGameNetServerModule::OnDeleteRoleGameProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqDeleteRole xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    AFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
    if(!m_AccountModule->DeleteRole(xMsg.account(), xAckRoleLiteInfoList))
    {
        ARK_LOG_ERROR("delete role failed, player_id = %s", nPlayerID.ToString().c_str());
    }

    m_pNetModule->SendMsgPB(AFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList, xClientID, nPlayerID);
}

void AFCGameNetServerModule::OnClienSwapSceneProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    CLIENT_MSG_PROCESS(nMsgID, msg, nLen, AFMsg::ReqAckSwapScene);

    AFCDataList varEntry;
    varEntry << pEntity->Self();
    varEntry << int32_t(0);
    varEntry << xMsg.scene_id();
    varEntry << int32_t(-1) ;
    m_pKernelModule->DoEvent(pEntity->Self(), AFED_ON_CLIENT_ENTER_SCENE, varEntry);
}

void AFCGameNetServerModule::OnProxyServerRegisteredProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = std::make_shared<GateServerInfo>();
            mProxyMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xServerData.xClient = xClientID;
        *(pServerData->xServerData.pData) = xData;

        ARK_LOG_INFO("Proxy Registered, server_id = %d server_name = %s", xData.server_id(), xData.server_name().c_str());
    }

    return;
}

void AFCGameNetServerModule::OnProxyServerUnRegisteredProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        mProxyMap.RemoveElement(xData.server_id());
        ARK_LOG_INFO("Proxy UnRegistered, server_id = %d server_name = %s", xData.server_id(), xData.server_name().c_str());
    }

    return;
}

void AFCGameNetServerModule::OnRefreshProxyServerInfoProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = std::make_shared<GateServerInfo>();
            mProxyMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xServerData.xClient = xClientID;
        *(pServerData->xServerData.pData) = xData;

        ARK_LOG_INFO("Proxy Registered, server_id = %d server_name = %s", xData.server_id(), xData.server_name().c_str());
    }

    return;
}

void AFCGameNetServerModule::SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const AFGUID& self)
{
    ARK_SHARE_PTR<GateBaseInfo> pData = mRoleBaseData.GetElement(self);
    if(nullptr != pData)
    {
        ARK_SHARE_PTR<GateServerInfo> pProxyData = mProxyMap.GetElement(pData->nGateID);
        if(nullptr != pProxyData)
        {
            m_pNetModule->SendMsgPB(nMsgID, xMsg, pProxyData->xServerData.xClient, pData->xClientID);
        }
    }
}

void AFCGameNetServerModule::SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const AFGUID& self)
{
    ARK_SHARE_PTR<GateBaseInfo> pData = mRoleBaseData.GetElement(self);
    if(nullptr != pData)
    {
        ARK_SHARE_PTR<GateServerInfo> pProxyData = mProxyMap.GetElement(pData->nGateID);
        if(nullptr != pProxyData)
        {
            m_pNetModule->SendMsgPB(nMsgID, strMsg, pProxyData->xServerData.xClient, pData->xClientID);
        }
    }
}

AFINetServerModule* AFCGameNetServerModule::GetNetModule()
{
    return m_pNetModule;
}

bool AFCGameNetServerModule::AddPlayerGateInfo(const AFGUID& nRoleID, const AFGUID& nClientID, const int nGateID)
{
    if(nGateID <= 0)
    {
        //非法gate
        return false;
    }

    if(nClientID.IsNULL())
    {
        return false;
    }

    ARK_SHARE_PTR<AFCGameNetServerModule::GateBaseInfo> pBaseData = mRoleBaseData.GetElement(nRoleID);
    if(nullptr != pBaseData)
    {
        //已经存在
        ARK_LOG_ERROR("player is already exist, cannot enter game again, id = %s", nClientID.ToString().c_str());
        return false;
    }

    ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(nGateID);
    if(nullptr == pServerData)
    {
        return false;
    }

    if(!pServerData->xRoleInfo.insert(std::make_pair(nRoleID, pServerData->xServerData.xClient)).second)
    {
        return false;
    }

    if(!mRoleBaseData.AddElement(nRoleID, std::make_shared<GateBaseInfo>(nGateID, nClientID)))
    {
        pServerData->xRoleInfo.erase(nRoleID);
        return false;
    }

    return true;
}

bool AFCGameNetServerModule::RemovePlayerGateInfo(const AFGUID& nRoleID)
{
    ARK_SHARE_PTR<GateBaseInfo> pBaseData = mRoleBaseData.GetElement(nRoleID);
    if(nullptr == pBaseData)
    {
        return false;
    }

    mRoleBaseData.RemoveElement(nRoleID);

    ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(pBaseData->nGateID);
    if(nullptr == pServerData)
    {
        return false;
    }

    pServerData->xRoleInfo.erase(nRoleID);
    return true;
}

ARK_SHARE_PTR<AFIGameNetServerModule::GateBaseInfo> AFCGameNetServerModule::GetPlayerGateInfo(const AFGUID& nRoleID)
{
    return mRoleBaseData.GetElement(nRoleID);
}

ARK_SHARE_PTR<AFIGameNetServerModule::GateServerInfo> AFCGameNetServerModule::GetGateServerInfo(const int nGateID)
{
    return mProxyMap.GetElement(nGateID);
}

ARK_SHARE_PTR<AFIGameNetServerModule::GateServerInfo> AFCGameNetServerModule::GetGateServerInfoByClientID(const AFGUID& nClientID)
{
    int nGateID = -1;
    ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.First();
    while(nullptr != pServerData)
    {
        if(nClientID == pServerData->xServerData.xClient)
        {
            nGateID = pServerData->xServerData.pData->server_id();
            break;
        }

        pServerData = mProxyMap.Next();
    }

    if(nGateID == -1)
    {
        return nullptr;
    }

    return pServerData;
}

void AFCGameNetServerModule::OnTransWorld(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    std::string strMsg;
    AFGUID nPlayer;
    int nHasKey = 0;
    if(AFINetServerModule::ReceivePB(xHead, nMsgID, msg, nLen, strMsg, nPlayer))
    {
        nHasKey = nPlayer.nLow;
    }

    m_pGameServerToWorldModule->SendBySuit(nHasKey, nMsgID, msg, nLen);
}

