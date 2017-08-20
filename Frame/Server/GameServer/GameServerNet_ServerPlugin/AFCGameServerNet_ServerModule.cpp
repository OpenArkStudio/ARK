// -------------------------------------------------------------------------
//    @FileName         :    AFCGameServerNet_ServerModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCGameServerNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#include "AFCGameServerNet_ServerModule.h"
#include "SDK/Interface/AFIModule.h"
#include "SDK/Proto/NFProtocolDefine.hpp"

bool AFCGameServerNet_ServerModule::Init()
{
    m_pNetModule = ARK_NEW AFINetServerModule(pPluginManager);
    return true;
}

bool AFCGameServerNet_ServerModule::AfterInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pSceneProcessModule = pPluginManager->FindModule<AFISceneProcessModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pUUIDModule = pPluginManager->FindModule<AFIGUIDModule>();
    m_pGameServerToWorldModule = pPluginManager->FindModule<AFIGameServerToWorldModule>();

    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_PTWG_PROXY_REFRESH, this, &AFCGameServerNet_ServerModule::OnRefreshProxyServerInfoProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_PTWG_PROXY_REGISTERED, this, &AFCGameServerNet_ServerModule::OnProxyServerRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_PTWG_PROXY_UNREGISTERED, this, &AFCGameServerNet_ServerModule::OnProxyServerUnRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_ENTER_GAME, this, &AFCGameServerNet_ServerModule::OnClienEnterGameProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_LEAVE_GAME, this, &AFCGameServerNet_ServerModule::OnClienLeaveGameProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_ROLE_LIST, this, &AFCGameServerNet_ServerModule::OnReqiureRoleListProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_CREATE_ROLE, this, &AFCGameServerNet_ServerModule::OnCreateRoleGameProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_DELETE_ROLE, this, &AFCGameServerNet_ServerModule::OnDeleteRoleGameProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_RECOVER_ROLE, this, &AFCGameServerNet_ServerModule::OnClienSwapSceneProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_SWAP_SCENE, this, &AFCGameServerNet_ServerModule::OnClienSwapSceneProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_SEARCH_GUILD, this, &AFCGameServerNet_ServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGEC_REQ_CREATE_CHATGROUP, this, &AFCGameServerNet_ServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGEC_REQ_JOIN_CHATGROUP, this, &AFCGameServerNet_ServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGEC_REQ_LEAVE_CHATGROUP, this, &AFCGameServerNet_ServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGEC_REQ_SUBSCRIPTION_CHATGROUP, this, &AFCGameServerNet_ServerModule::OnTransWorld);

    m_pNetModule->AddEventCallBack(this, &AFCGameServerNet_ServerModule::OnSocketPSEvent);

    m_pKernelModule->RegisterCommonClassEvent(this, &AFCGameServerNet_ServerModule::OnClassCommonEvent);
    m_pKernelModule->RegisterCommonPropertyEvent(this, &AFCGameServerNet_ServerModule::OnPropertyCommonEvent);
    m_pKernelModule->RegisterCommonRecordEvent(this, &AFCGameServerNet_ServerModule::OnRecordCommonEvent);

    m_pKernelModule->AddClassCallBack(NFrame::Player::ThisName(), this, &AFCGameServerNet_ServerModule::OnObjectClassEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetPropertyInt(strConfigName, "Type");
            const int nServerID = m_pElementModule->GetPropertyInt(strConfigName, "ServerID");
            if(nServerType == NF_SERVER_TYPES::NF_ST_GAME && pPluginManager->AppID() == nServerID)
            {
                const int nPort = m_pElementModule->GetPropertyInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetPropertyInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetPropertyInt(strConfigName, "CpuCount");
                const std::string strName(m_pElementModule->GetPropertyString(strConfigName, "Name"));
                const std::string strIP(m_pElementModule->GetPropertyString(strConfigName, "IP"));

                int nRet = m_pNetModule->Initialization(nMaxConnect, nPort, nServerID, nCpus);
                if(nRet < 0)
                {
                    std::ostringstream strLog;
                    strLog << "Cannot init server net, Port = " << nPort;
                    m_pLogModule->LogError(NULL_GUID, strLog, __FUNCTION__, __LINE__);
                    ARK_ASSERT(nRet, "Cannot init server net", __FILE__, __FUNCTION__);
                    exit(0);
                }
            }
        }
    }

    return true;
}

bool AFCGameServerNet_ServerModule::Shut()
{

    return true;
}

bool AFCGameServerNet_ServerModule::Execute()
{
    return m_pNetModule->Execute();
}

void AFCGameServerNet_ServerModule::OnSocketPSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if(eEvent == DISCONNECTED)
    {
        m_pLogModule->LogInfo(xClientID, "NF_NET_EVENT_EOF", "Connection closed", __FUNCTION__, __LINE__);
        OnClientDisconnect(xClientID);
    }
    else  if(eEvent == CONNECTED)
    {
        m_pLogModule->LogInfo(xClientID, "NF_NET_EVENT_CONNECTED", "connected success", __FUNCTION__, __LINE__);
        OnClientConnected(xClientID);
    }
}

void AFCGameServerNet_ServerModule::OnClientDisconnect(const AFGUID& xClientID)
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

void AFCGameServerNet_ServerModule::OnClientConnected(const AFGUID& xClientID)
{

}

void AFCGameServerNet_ServerModule::OnClienEnterGameProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //在进入游戏之前nPlayerID为其在网关的FD
    AFGUID nClientID;
    AFMsg::ReqEnterGameServer xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nClientID))
    {
        return;
    }

    AFGUID nRoleID = AFINetServerModule::PBToNF(xMsg.id());

    if(m_pKernelModule->GetObject(nRoleID))
    {
        m_pKernelModule->DestroyObject(nRoleID);
    }

    //////////////////////////////////////////////////////////////////////////

    ARK_SHARE_PTR<AFCGameServerNet_ServerModule::GateBaseInfo>  pGateInfo = GetPlayerGateInfo(nRoleID);
    if(nullptr != pGateInfo)
    {
        RemovePlayerGateInfo(nRoleID);
    }

    ARK_SHARE_PTR<AFCGameServerNet_ServerModule::GateServerInfo> pGateServereinfo = GetGateServerInfoByClientID(xClientID);
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

    if(!AddPlayerGateInfo(nRoleID, nClientID, nGateID))
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
    var.AddObject(nClientID);

    ARK_SHARE_PTR<AFIObject> pObject = m_pKernelModule->CreateObject(nRoleID, nSceneID, 0, NFrame::Player::ThisName(), "", var);
    if(nullptr == pObject)
    {
        //内存泄漏
        //mRoleBaseData
        //mRoleFDData
        return;
    }

    pObject->SetPropertyInt("LoadPropertyFinish", 1);
    pObject->SetPropertyInt("GateID", nGateID);
    pObject->SetPropertyInt("GameID", pPluginManager->AppID());

    m_pKernelModule->DoEvent(pObject->Self(), NFrame::Player::ThisName(), CLASS_OBJECT_EVENT::COE_CREATE_FINISH, AFCDataList());

    AFCDataList varEntry;
    varEntry << pObject->Self();
    varEntry << int64_t(0);
    varEntry << nSceneID;
    varEntry << -1;
    m_pKernelModule->DoEvent(pObject->Self(), NFED_ON_CLIENT_ENTER_SCENE, varEntry);
}

void AFCGameServerNet_ServerModule::OnClienLeaveGameProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqLeaveGameServer xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    if(nPlayerID.IsNull())
    {
        return;
    }

    if(m_pKernelModule->GetObject(nPlayerID))
    {
        m_pKernelModule->DestroyObject(nPlayerID);
    }

    RemovePlayerGateInfo(nPlayerID);
}

int AFCGameServerNet_ServerModule::OnPropertyEnter(const AFIDataList& argVar, const AFGUID& self)
{
    //if(argVar.GetCount() <= 0 || self.IsNull())
    //{
    //    return 0;
    //}

    //AFMsg::MultiObjectPropertyList xPublicMsg;
    //AFMsg::MultiObjectPropertyList xPrivateMsg;

    ////分为自己和外人
    ////1.public发送给所有人
    ////2.如果自己在列表中，再次发送private数据
    //ARK_SHARE_PTR<AFIObject> pObject = m_pKernelModule->GetObject(self);
    //if(nullptr != pObject)
    //{
    //    AFMsg::ObjectPropertyList* pPublicData = xPublicMsg.add_multi_player_property();
    //    AFMsg::ObjectPropertyList* pPrivateData = xPrivateMsg.add_multi_player_property();

    //    *(pPublicData->mutable_player_id()) = AFINetServerModule::NFToPB(self);
    //    *(pPrivateData->mutable_player_id()) = AFINetServerModule::NFToPB(self);

    //    ARK_SHARE_PTR<AFIPropertyMgr> pPropertyManager = pObject->GetPropertyManager();

    //    for(int i = 0; i < pPropertyManager->GetPropertyCount(); i++)
    //    {
    //        AFProperty* pPropertyInfo = pPropertyManager->GetPropertyByIndex(i);
    //        if(NULL == pPropertyInfo)
    //        {
    //            continue;
    //        }

    //        if(pPropertyInfo->Changed())
    //        {
    //            switch(pPropertyInfo->GetType())
    //            {
    //            case DT_INT:
    //                {
    //                    if(pPropertyInfo->IsPublic())
    //                    {
    //                        AFMsg::PropertyInt* pDataInt = pPublicData->add_property_int_list();
    //                        pDataInt->set_property_name(pPropertyInfo->GetName());
    //                        pDataInt->set_data(pPropertyInfo->GetValue());
    //                    }

    //                    if(pPropertyInfo->IsPrivate())
    //                    {
    //                        AFMsg::PropertyInt* pDataInt = pPrivateData->add_property_int_list();
    //                        pDataInt->set_property_name(pPropertyInfo->GetKey());
    //                        pDataInt->set_data(pPropertyInfo->GetInt());
    //                    }
    //                }
    //                break;
    //            case DT_DOUBLE:
    //                {
    //                    if(pPropertyInfo->IsPublic())
    //                    {
    //                        AFMsg::PropertyDouble* pDataDouble = pPublicData->add_property_double_list();
    //                        pDataDouble->set_property_name(pPropertyInfo->GetKey());
    //                        pDataDouble->set_data(pPropertyInfo->GetDouble());
    //                    }

    //                    if(pPropertyInfo->IsPrivate())
    //                    {
    //                        AFMsg::PropertyDouble* pDataDouble = pPrivateData->add_property_double_list();
    //                        pDataDouble->set_property_name(pPropertyInfo->GetKey());
    //                        pDataDouble->set_data(pPropertyInfo->GetDouble());
    //                    }
    //                }
    //                break;
    //            case DT_STRING:
    //                {
    //                    if(pPropertyInfo->IsPublic())
    //                    {
    //                        AFMsg::PropertyString* pDataString = pPublicData->add_property_string_list();
    //                        pDataString->set_property_name(pPropertyInfo->GetKey());
    //                        pDataString->set_data(pPropertyInfo->GetString());
    //                    }

    //                    if(pPropertyInfo->IsPrivate())
    //                    {
    //                        AFMsg::PropertyString* pDataString = pPrivateData->add_property_string_list();
    //                        pDataString->set_property_name(pPropertyInfo->GetKey());
    //                        pDataString->set_data(pPropertyInfo->GetString());
    //                    }
    //                }
    //                break;
    //            case DT_OBJECT:
    //                {
    //                    if(pPropertyInfo->IsPublic())
    //                    {
    //                        AFMsg::PropertyObject* pDataObject = pPublicData->add_property_object_list();
    //                        pDataObject->set_property_name(pPropertyInfo->GetKey());
    //                        *(pDataObject->mutable_data()) = AFINetServerModule::NFToPB(pPropertyInfo->GetObject());
    //                    }

    //                    if(pPropertyInfo->IsPrivate())
    //                    {
    //                        AFMsg::PropertyObject* pDataObject = pPrivateData->add_property_object_list();
    //                        pDataObject->set_property_name(pPropertyInfo->GetKey());
    //                        *(pDataObject->mutable_data()) = AFINetServerModule::NFToPB(pPropertyInfo->GetObject());
    //                    }
    //                }
    //                break;
    //            /*case TDATA_POINT:
    //            {
    //            if(pPropertyInfo->GetPublic())
    //            {
    //            AFMsg::PropertyPoint* pDataPoint = pPublicData->add_property_point_list();
    //            pDataPoint->set_property_name(pPropertyInfo->GetKey());
    //            *(pDataPoint->mutable_data()) = AFINetServerModule::NFToPB(pPropertyInfo->GetPoint());
    //            }

    //            if(pPropertyInfo->GetPrivate())
    //            {
    //            AFMsg::PropertyPoint* pDataPoint = pPrivateData->add_property_point_list();
    //            pDataPoint->set_property_name(pPropertyInfo->GetKey());
    //            *(pDataPoint->mutable_data()) = AFINetServerModule::NFToPB(pPropertyInfo->GetPoint());
    //            }
    //            }
    //            break;*/
    //            default:
    //                break;
    //            }
    //        }
    //    }

    //    for(int i = 0; i < argVar.GetCount(); i++)
    //    {
    //        AFGUID identOther = argVar.Object(i);
    //        if(self == identOther)
    //        {
    //            //找到他所在网关的FD
    //            SendMsgPBToGate(AFMsg::EGMI_ACK_OBJECT_PROPERTY_ENTRY, xPrivateMsg, identOther);
    //        }
    //        else
    //        {
    //            SendMsgPBToGate(AFMsg::EGMI_ACK_OBJECT_PROPERTY_ENTRY, xPublicMsg, identOther);
    //        }
    //    }

    //}

    return 0;
}

bool OnRecordEnterPack(ARK_SHARE_PTR<AFIRecord> pRecord, AFMsg::ObjectRecordBase* pObjectRecordBase)
{
    //if(!pRecord || !pObjectRecordBase)
    //{
    //    return false;
    //}

    //for(int i = 0; i < pRecord->GetRows(); i++)
    //{
    //    if(pRecord->IsUsed(i))
    //    {
    //        //不管public还是private都要加上，不然public广播了那不是private就广播不了了
    //        AFMsg::RecordAddRowStruct* pAddRowStruct = pObjectRecordBase->add_row_struct();
    //        pAddRowStruct->set_row(i);

    //        for(int j = 0; j < pRecord->GetCols(); j++)
    //        {
    //            //如果是0就不发送了，因为客户端默认是0
    //            AFIDataList valueList;
    //            AF_DATA_TYPE eType = pRecord->GetColType(j);
    //            switch(eType)
    //            {
    //            case AF_DATA_TYPE::DT_INT:
    //                {
    //                    int nValue = pRecord->GetInt(i, j);
    //                    //if ( 0 != nValue )
    //                    {
    //                        AFMsg::RecordInt* pAddData = pAddRowStruct->add_record_int_list();
    //                        pAddData->set_row(i);
    //                        pAddData->set_col(j);
    //                        pAddData->set_data(nValue);
    //                    }
    //                }
    //                break;
    //            case AF_DATA_TYPE::DT_DOUBLE:
    //                {
    //                    double dwValue = pRecord->GetDouble(i, j);
    //                    //if ( dwValue < -0.01f || dwValue > 0.01f )
    //                    {
    //                        AFMsg::RecordDouble* pAddData = pAddRowStruct->add_record_double_list();
    //                        pAddData->set_row(i);
    //                        pAddData->set_col(j);
    //                        pAddData->set_data(dwValue);
    //                    }
    //                }
    //                break;
    //            case AF_DATA_TYPE::DT_STRING:
    //                {
    //                    const std::string& strData = pRecord->GetString(i, j);
    //                    //if ( !strData.empty() )
    //                    {
    //                        AFMsg::RecordString* pAddData = pAddRowStruct->add_record_string_list();
    //                        pAddData->set_row(i);
    //                        pAddData->set_col(j);
    //                        pAddData->set_data(strData);
    //                    }
    //                }
    //                break;
    //            case AF_DATA_TYPE::DT_OBJECT:
    //                {
    //                    AFGUID ident = pRecord->GetObject(i, j);
    //                    //if ( !ident.IsNull() )
    //                    {
    //                        AFMsg::RecordObject* pAddData = pAddRowStruct->add_record_object_list();
    //                        pAddData->set_row(i);
    //                        pAddData->set_col(j);
    //                        *(pAddData->mutable_data()) = AFINetServerModule::NFToPB(ident);
    //                    }
    //                }
    //                break;
    //            //case AF_DATA_TYPE::TDATA_POINT:
    //            //    {
    //            //        const Point3D& xPoint = pRecord->GetPoint(i, j);
    //            //        //if ( !xPoint.IsNull() )
    //            //        {
    //            //            AFMsg::RecordPoint* pAddData = pAddRowStruct->add_record_point_list();
    //            //            pAddData->set_row(i);
    //            //            pAddData->set_col(j);
    //            //            *(pAddData->mutable_data()) = AFINetServerModule::NFToPB(xPoint);
    //            //        }
    //            //    }
    //            //    break;
    //            default:
    //                break;
    //            }
    //        }
    //    }
    //}

    return true;
}

int AFCGameServerNet_ServerModule::OnRecordEnter(const AFIDataList& argVar, const AFGUID& self)
{
    if(argVar.GetCount() <= 0 || self.IsNull())
    {
        return 0;
    }

    AFMsg::MultiObjectRecordList xPublicMsg;
    AFMsg::MultiObjectRecordList xPrivateMsg;

    ARK_SHARE_PTR<AFIObject> pObject = m_pKernelModule->GetObject(self);
    if(nullptr != pObject)
    {
        AFMsg::ObjectRecordList* pPublicData = NULL;
        AFMsg::ObjectRecordList* pPrivateData = NULL;

        ARK_SHARE_PTR<AFIRecordManager> pRecordManager = pObject->GetRecordManager();
        ARK_SHARE_PTR<AFIRecord> pRecord = pRecordManager->First();
        while(nullptr != pRecord)
        {
            if(!pRecord->GetPublic() && !pRecord->GetPrivate())
            {
                pRecord = pRecordManager->Next();
                continue;
            }

            AFMsg::ObjectRecordBase* pPrivateRecordBase = NULL;
            AFMsg::ObjectRecordBase* pPublicRecordBase = NULL;
            if(pRecord->GetPublic())
            {
                if(!pPublicData)
                {
                    pPublicData = xPublicMsg.add_multi_player_record();
                    *(pPublicData->mutable_player_id()) = AFINetServerModule::NFToPB(self);
                }
                pPublicRecordBase = pPublicData->add_record_list();
                pPublicRecordBase->set_record_name(pRecord->GetName());

                OnRecordEnterPack(pRecord, pPublicRecordBase);
            }

            if(pRecord->GetPrivate())
            {
                if(!pPrivateData)
                {
                    pPrivateData = xPrivateMsg.add_multi_player_record();
                    *(pPrivateData->mutable_player_id()) = AFINetServerModule::NFToPB(self);
                }
                pPrivateRecordBase = pPrivateData->add_record_list();
                pPrivateRecordBase->set_record_name(pRecord->GetName());

                OnRecordEnterPack(pRecord, pPrivateRecordBase);
            }

            pRecord = pRecordManager->Next();
        }


        for(int i = 0; i < argVar.GetCount(); i++)
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
    }

    return 0;
}

int AFCGameServerNet_ServerModule::OnObjectListEnter(const AFIDataList& self, const AFIDataList& argVar)
{
    //if(self.GetCount() <= 0 || argVar.GetCount() <= 0)
    //{
    //    return 0;
    //}

    //AFMsg::AckPlayerEntryList xPlayerEntryInfoList;
    //for(int i = 0; i < argVar.GetCount(); i++)
    //{
    //    AFGUID identOld = argVar.Object(i);
    //    //排除空对象
    //    if(identOld.IsNull())
    //    {
    //        continue;
    //    }

    //    AFMsg::PlayerEntryInfo* pEntryInfo = xPlayerEntryInfoList.add_object_list();
    //    *(pEntryInfo->mutable_object_guid()) = AFINetServerModule::NFToPB(identOld);
    //    *pEntryInfo->mutable_pos() = AFINetServerModule::NFToPB(m_pKernelModule->GetPropertyPoint(identOld, "Pos"));
    //    pEntryInfo->set_career_type(m_pKernelModule->GetPropertyInt(identOld, "Job"));
    //    pEntryInfo->set_player_state(m_pKernelModule->GetPropertyInt(identOld, "State"));
    //    pEntryInfo->set_config_id(m_pKernelModule->GetPropertyString(identOld, "ConfigID"));
    //    pEntryInfo->set_scene_id(m_pKernelModule->GetPropertyInt(identOld, "SceneID"));
    //    pEntryInfo->set_class_id(m_pKernelModule->GetPropertyString(identOld, "ClassName"));
    //}

    //if(xPlayerEntryInfoList.object_list_size() <= 0)
    //{
    //    return 0;
    //}

    //for(int i = 0; i < self.GetCount(); i++)
    //{
    //    AFGUID ident = self.Object(i);
    //    if(ident.IsNull())
    //    {
    //        continue;
    //    }

    //    //可能在不同的网关呢,得到后者所在的网关FD
    //    SendMsgPBToGate(AFMsg::EGMI_ACK_OBJECT_ENTRY, xPlayerEntryInfoList, ident);
    //}

    return 1;
}

int AFCGameServerNet_ServerModule::OnObjectListLeave(const AFIDataList& self, const AFIDataList& argVar)
{
    if(self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 0;
    }

    AFMsg::AckPlayerLeaveList xPlayerLeaveInfoList;
    for(int i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOld = argVar.Object(i);
        //排除空对象
        if(identOld.IsNull())
        {
            continue;
        }

        AFMsg::Ident* pIdent = xPlayerLeaveInfoList.add_object_list();
        *pIdent = AFINetServerModule::NFToPB(argVar.Object(i));
    }

    for(int i = 0; i < self.GetCount(); i++)
    {
        AFGUID ident = self.Object(i);
        if(ident.IsNull())
        {
            continue;
        }
        //可能在不同的网关呢,得到后者所在的网关FD
        SendMsgPBToGate(AFMsg::EGMI_ACK_OBJECT_LEAVE, xPlayerLeaveInfoList, ident);
    }

    return 1;
}


int AFCGameServerNet_ServerModule::OnPropertyCommonEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar)
{
    //if ( NFrame::Player::ThisName() == m_pKernelModule->GetPropertyString( self, "ClassName" ) )
    {
        if("GroupID" == strPropertyName)
        {
            //自己还是要知道自己的这个属性变化的,但是别人就不需要知道了
            OnGroupEvent(self, strPropertyName, oldVar, newVar);
        }

        if("SceneID" == strPropertyName)
        {
            //自己还是要知道自己的这个属性变化的,但是别人就不需要知道了
            OnContainerEvent(self, strPropertyName, oldVar, newVar);
        }

        if(NFrame::Player::ThisName() == std::string(m_pKernelModule->GetPropertyString(self, "ClassName")))
        {
            if(m_pKernelModule->GetPropertyInt(self, "LoadPropertyFinish") <= 0)
            {
                return 0;
            }
        }
    }

    AFCDataList valueBroadCaseList;
    int nCount = 0;//argVar.GetCount() ;
    if(nCount <= 0)
    {
        nCount = GetBroadCastObject(self, strPropertyName, false, valueBroadCaseList);
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
    *pIdent = AFINetServerModule::NFToPB(self);
    AFMsg::PropertyPBData* pData = xPropertyData.add_property_list();
    AFINetServerModule::DataToPBProperty(oldVar, strPropertyName.c_str(), *pData);

    for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
    {
        AFGUID identOld = valueBroadCaseList.Object(i);

        SendMsgPBToGate(AFMsg::EGMI_ACK_PROPERTY_DATA, xPropertyData, identOld);
    }

    return 0;
}

int AFCGameServerNet_ServerModule::OnRecordCommonEvent(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar)
{
    const std::string& strRecordName = xEventData.strRecordName;
    const int nOpType = xEventData.nOpType;
    const int nRow = xEventData.nRow;
    const int nCol = xEventData.nCol;

    int nObjectContainerID = m_pKernelModule->GetPropertyInt(self, "SceneID");
    int nObjectGroupID = m_pKernelModule->GetPropertyInt(self, "GroupID");

    if(nObjectGroupID < 0)
    {
        //容器
        return 0;
    }

    if(NFrame::Player::ThisName() == std::string(m_pKernelModule->GetPropertyString(self, "ClassName")))
    {
        if(m_pKernelModule->GetPropertyInt(self, "LoadPropertyFinish") <= 0)
        {
            return 0;
        }
    }

    AFCDataList valueBroadCaseList;
    GetBroadCastObject(self, strRecordName, true, valueBroadCaseList);

    switch(nOpType)
    {
    case AFIRecord::RecordOptype::Add:
        {
            AFMsg::ObjectRecordAddRow xAddRecordRow;
            AFMsg::Ident* pIdent = xAddRecordRow.mutable_player_id();
            *pIdent = AFINetServerModule::NFToPB(self);

            xAddRecordRow.set_record_name(strRecordName);

            AFMsg::RecordAddRowStruct* pAddRowData = xAddRecordRow.add_row_data();
            pAddRowData->set_row(nRow);

            //add row 需要完整的row
            ARK_SHARE_PTR<AFIRecord> xRecord = m_pKernelModule->FindRecord(self, strRecordName);
            if(xRecord)
            {
                AFCDataList xRowDataList;
                if(xRecord->QueryRow(nRow, xRowDataList))
                {
                    for(int i = 0; i < xRowDataList.GetCount(); i++)
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
    case AFIRecord::RecordOptype::Del:
        {
            AFMsg::ObjectRecordRemove xReoveRecordRow;

            AFMsg::Ident* pIdent = xReoveRecordRow.mutable_player_id();
            *pIdent = AFINetServerModule::NFToPB(self);

            xReoveRecordRow.set_record_name(strRecordName);
            xReoveRecordRow.add_remove_row(nRow);

            for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                AFGUID identOther = valueBroadCaseList.Object(i);

                SendMsgPBToGate(AFMsg::EGMI_ACK_REMOVE_ROW, xReoveRecordRow, identOther);
            }
        }
        break;
    case AFIRecord::RecordOptype::Swap:
        {
            //其实是2个row交换
            AFMsg::ObjectRecordSwap xSwapRecord;
            *xSwapRecord.mutable_player_id() = AFINetServerModule::NFToPB(self);

            xSwapRecord.set_origin_record_name(strRecordName);
            xSwapRecord.set_target_record_name(strRecordName);   // 暂时没用
            xSwapRecord.set_row_origin(nRow);
            xSwapRecord.set_row_target(nCol);

            for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                AFGUID identOther = valueBroadCaseList.Object(i);

                SendMsgPBToGate(AFMsg::EGMI_ACK_SWAP_ROW, xSwapRecord, identOther);
            }
        }
        break;
    case AFIRecord::RecordOptype::Update:
        {
            AFMsg::ObjectRecordPBData xRecordChanged;
            *xRecordChanged.mutable_player_id() = AFINetServerModule::NFToPB(self);
            xRecordChanged.set_record_name(strRecordName);
            AFMsg::RecordPBData* recordProperty = xRecordChanged.add_record_list();
            AFINetServerModule::RecordToPBRecord(newVar, nRow, nCol, *recordProperty);

            for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                AFGUID identOther = valueBroadCaseList.Object(i);

                SendMsgPBToGate(AFMsg::EGMI_ACK_RECORD_DATA, xRecordChanged, identOther);
            }
        }
        break;
    case AFIRecord::RecordOptype::Create:
        break;
    case AFIRecord::RecordOptype::Cleared:
        break;
    default:
        break;
    }

    return 0;
}

int AFCGameServerNet_ServerModule::OnClassCommonEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& var)
{
    ////////////1:广播给已经存在的人//////////////////////////////////////////////////////////////
    if(CLASS_OBJECT_EVENT::COE_DESTROY == eClassEvent)
    {
        //删除在线标志

        //////////////////////////////////////////////////////////////////////////

        int nObjectContainerID = m_pKernelModule->GetPropertyInt(self, "SceneID");
        int nObjectGroupID = m_pKernelModule->GetPropertyInt(self, "GroupID");

        if(nObjectGroupID < 0)
        {
            //容器
            return 0;
        }

        AFCDataList valueAllObjectList;
        AFCDataList valueBroadCaseList;
        AFCDataList valueBroadListNoSelf;
        m_pKernelModule->GetGroupObjectList(nObjectContainerID, nObjectGroupID, valueAllObjectList);

        for(int i = 0; i < valueAllObjectList.GetCount(); i++)
        {
            AFGUID identBC = valueAllObjectList.Object(i);
            const std::string strClassName(m_pKernelModule->GetPropertyString(identBC, "ClassName"));
            if(NFrame::Player::ThisName() == strClassName)
            {
                valueBroadCaseList << identBC;
                if(identBC != self)
                {
                    valueBroadListNoSelf << identBC;
                }
            }
        }

        //如果是副本的怪，则不需要发送，因为会在离开副本的时候一次性一条消息发送
        OnObjectListLeave(valueBroadListNoSelf, AFCDataList() << self);
    }

    else if(CLASS_OBJECT_EVENT::COE_CREATE_NODATA == eClassEvent)
    {
        //id和fd,gateid绑定
        ARK_SHARE_PTR<GateBaseInfo> pDataBase = mRoleBaseData.GetElement(self);
        if(nullptr != pDataBase)
        {
            //回复客户端角色进入游戏世界成功了
            AFMsg::AckEventResult xMsg;
            xMsg.set_event_code(AFMsg::EGEC_ENTER_GAME_SUCCESS);

            *xMsg.mutable_event_client() = AFINetServerModule::NFToPB(pDataBase->xClientID);
            *xMsg.mutable_event_object() = AFINetServerModule::NFToPB(self);

            SendMsgPBToGate(AFMsg::EGMI_ACK_ENTER_GAME, xMsg, self);
        }
    }
    else if(CLASS_OBJECT_EVENT::COE_CREATE_LOADDATA == eClassEvent)
    {
    }
    else if(CLASS_OBJECT_EVENT::COE_CREATE_HASDATA == eClassEvent)
    {
        //自己广播给自己就够了
        if(strClassName == NFrame::Player::ThisName())
        {
            OnObjectListEnter(AFCDataList() << self, AFCDataList() << self);

            OnPropertyEnter(AFCDataList() << self, self);
            OnRecordEnter(AFCDataList() << self, self);
        }
    }
    else if(CLASS_OBJECT_EVENT::COE_CREATE_FINISH == eClassEvent)
    {

    }
    return 0;
}

int AFCGameServerNet_ServerModule::OnGroupEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar)
{
    //容器发生变化，只可能从A容器的0层切换到B容器的0层
    //需要注意的是------------任何层改变的时候，此玩家其实还未进入层，因此，层改变的时候获取的玩家列表，目标层是不包含自己的
    int nSceneID = m_pKernelModule->GetPropertyInt(self, "SceneID");

    //广播给别人自己离去(层降或者跃层)
    int nOldGroupID = oldVar.GetInt();
    if(nOldGroupID > 0)
    {
        AFCDataList valueAllOldObjectList;
        AFCDataList valueAllOldPlayerList;
        m_pKernelModule->GetGroupObjectList(nSceneID, nOldGroupID, valueAllOldObjectList);
        if(valueAllOldObjectList.GetCount() > 0)
        {
            //自己只需要广播其他玩家
            for(int i = 0; i < valueAllOldObjectList.GetCount(); i++)
            {
                AFGUID identBC = valueAllOldObjectList.Object(i);

                if(valueAllOldObjectList.Object(i) == self)
                {
                    valueAllOldObjectList.SetObject(i, AFGUID());
                }

                const std::string strClassName(m_pKernelModule->GetPropertyString(identBC, "ClassName"));
                if(NFrame::Player::ThisName() == strClassName)
                {
                    valueAllOldPlayerList << identBC;
                }
            }

            OnObjectListLeave(valueAllOldPlayerList, AFCDataList() << self);

            //老的全部要广播删除
            OnObjectListLeave(AFCDataList() << self, valueAllOldObjectList);
        }

        m_pKernelModule->DoEvent(self, NFED_ON_CLIENT_LEAVE_SCENE, AFCDataList() << nOldGroupID);
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
        m_pKernelModule->GetGroupObjectList(nSceneID, nNewGroupID, valueAllObjectList);
        for(int i = 0; i < valueAllObjectList.GetCount(); i++)
        {
            AFGUID identBC = valueAllObjectList.Object(i);
            const std::string strClassName(m_pKernelModule->GetPropertyString(identBC, "ClassName"));
            if(NFrame::Player::ThisName() == strClassName)
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
            OnObjectListEnter(valuePlayerListNoSelf, AFCDataList() << self);
        }

        const std::string strSelfClassName(m_pKernelModule->GetPropertyString(self, "ClassName"));

        //广播给自己,所有的别人出现
        if(valueAllObjectListNoSelf.GetCount() > 0)
        {
            if(strSelfClassName == NFrame::Player::ThisName())
            {
                OnObjectListEnter(AFCDataList() << self, valueAllObjectListNoSelf);
            }
        }

        if(strSelfClassName == NFrame::Player::ThisName())
        {
            for(int i = 0; i < valueAllObjectListNoSelf.GetCount(); i++)
            {
                //此时不用再广播自己的属性给自己
                //把已经存在的人的属性广播给新来的人
                AFGUID identOld = valueAllObjectListNoSelf.Object(i);

                OnPropertyEnter(AFCDataList() << self, identOld);
                //把已经存在的人的表广播给新来的人
                OnRecordEnter(AFCDataList() << self, identOld);
            }
        }

        //把新来的人的属性广播给周边的人
        if(valuePlayerListNoSelf.GetCount() > 0)
        {
            OnPropertyEnter(valuePlayerListNoSelf, self);
            OnRecordEnter(valuePlayerListNoSelf, self);
        }
    }

    return 0;
}

int AFCGameServerNet_ServerModule::OnContainerEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar)
{
    //容器发生变化，只可能从A容器的0层切换到B容器的0层
    //需要注意的是------------任何容器改变的时候，玩家必须是0层
    int nOldSceneID = oldVar.GetInt();
    int nNowSceneID = newVar.GetInt();

    m_pLogModule->LogInfo(self, "Enter Scene:", nNowSceneID);

    //自己消失,玩家不用广播，因为在消失之前，会回到0层，早已广播了玩家
    AFCDataList valueOldAllObjectList;
    AFCDataList valueNewAllObjectList;
    AFCDataList valueAllObjectListNoSelf;
    AFCDataList valuePlayerList;
    AFCDataList valuePlayerNoSelf;

    m_pKernelModule->GetGroupObjectList(nOldSceneID, 0, valueOldAllObjectList);
    m_pKernelModule->GetGroupObjectList(nNowSceneID, 0, valueNewAllObjectList);

    for(int i = 0; i < valueOldAllObjectList.GetCount(); i++)
    {
        AFGUID identBC = valueOldAllObjectList.Object(i);
        if(identBC == self)
        {
            valueOldAllObjectList.SetObject(i, AFGUID());
        }
    }

    for(int i = 0; i < valueNewAllObjectList.GetCount(); i++)
    {
        AFGUID identBC = valueNewAllObjectList.Object(i);
        const std::string strClassName(m_pKernelModule->GetPropertyString(identBC, "ClassName"));
        if(NFrame::Player::ThisName() == strClassName)
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
    OnObjectListLeave(AFCDataList() << self, valueOldAllObjectList);

    //广播给所有人出现对象(如果是玩家，则包括广播给自己)
    //这里广播的都是0层的
    if(valuePlayerList.GetCount() > 0)
    {
        //把self广播给argVar这些人
        OnObjectListEnter(valuePlayerNoSelf, AFCDataList() << self);
    }

    //新层必然是0，把0层NPC广播给自己------------自己广播给自己不在这里广播，因为场景ID在跨场景时会经常变化

    //把valueAllObjectList广播给self
    OnObjectListEnter(AFCDataList() << self, valueAllObjectListNoSelf);

    ////////////////////把已经存在的人的属性广播给新来的人//////////////////////////////////////////////////////
    for(int i = 0; i < valueAllObjectListNoSelf.GetCount(); i++)
    {
        AFGUID identOld = valueAllObjectListNoSelf.Object(i);
        OnPropertyEnter(AFCDataList() << self, identOld);
        ////////////////////把已经存在的人的表广播给新来的人//////////////////////////////////////////////////////
        OnRecordEnter(AFCDataList() << self, identOld);
    }

    //把新来的人的属性广播给周边的人()
    if(valuePlayerNoSelf.GetCount() > 0)
    {
        OnPropertyEnter(valuePlayerNoSelf, self);
        OnRecordEnter(valuePlayerNoSelf, self);
    }

    return 0;
}

int AFCGameServerNet_ServerModule::GetBroadCastObject(const AFGUID& self, const std::string& strPropertyName, const bool bTable, AFIDataList& valueObject)
{
    //int nObjectContainerID = m_pKernelModule->GetPropertyInt(self, "SceneID");
    //int nObjectGroupID = m_pKernelModule->GetPropertyInt(self, "GroupID");

    ////普通场景容器，判断广播属性
    //std::string strClassName = m_pKernelModule->GetPropertyString(self, "ClassName");
    //ARK_SHARE_PTR<AFIRecordManager> pClassRecordManager = m_pClassModule->GetClassRecordManager(strClassName);
    //ARK_SHARE_PTR<AFIPropertyManager> pClassPropertyManager = m_pClassModule->GetClassPropertyManager(strClassName);

    //ARK_SHARE_PTR<AFIRecord> pRecord;
    //ARK_SHARE_PTR<AFIProperty> pProperty;
    //if(bTable)
    //{
    //    if(nullptr == pClassRecordManager)
    //    {
    //        return -1;
    //    }

    //    pRecord = pClassRecordManager->GetElement(strPropertyName);
    //    if(nullptr == pRecord)
    //    {
    //        return -1;
    //    }
    //}
    //else
    //{
    //    if(nullptr == pClassPropertyManager)
    //    {
    //        return -1;
    //    }

    //    pProperty = pClassPropertyManager->GetElement(strPropertyName);
    //    if(nullptr == pProperty)
    //    {
    //        return -1;
    //    }
    //}

    //if(NFrame::Player::ThisName() == strClassName)
    //{
    //    if(bTable)
    //    {
    //        if(pRecord->GetPublic())
    //        {
    //            GetBroadCastObject(nObjectContainerID, nObjectGroupID, valueObject);
    //        }
    //        else if(pRecord->GetPrivate())
    //        {
    //            valueObject.Add(self);
    //        }
    //    }
    //    else
    //    {
    //        if(pProperty->GetPublic())
    //        {
    //            GetBroadCastObject(nObjectContainerID, nObjectGroupID, valueObject);
    //        }
    //        else if(pProperty->GetPrivate())
    //        {
    //            valueObject.Add(self);
    //        }
    //    }
    //    //一个玩家都不广播
    //    return valueObject.GetCount();
    //}

    ////不是玩家,NPC和怪物类等
    //if(bTable)
    //{
    //    if(pRecord->GetPublic())
    //    {
    //        //广播给客户端自己和周边人
    //        GetBroadCastObject(nObjectContainerID, nObjectGroupID, valueObject);
    //    }
    //}
    //else
    //{
    //    if(pProperty->GetPublic())
    //    {
    //        //广播给客户端自己和周边人
    //        GetBroadCastObject(nObjectContainerID, nObjectGroupID, valueObject);
    //    }
    //}

    return valueObject.GetCount();
}

int AFCGameServerNet_ServerModule::GetBroadCastObject(const int nObjectContainerID, const int nGroupID, AFIDataList& valueObject)
{
    /* AFCDataList valContainerObjectList;
     m_pKernelModule->GetGroupObjectList(nObjectContainerID, nGroupID, valContainerObjectList);
     for(int i = 0; i < valContainerObjectList.GetCount(); i++)
     {
         const std::string& strObjClassName = m_pKernelModule->GetPropertyString(valContainerObjectList.Object(i), "ClassName");
         if(NFrame::Player::ThisName() == strObjClassName)
         {
             valueObject.Add(valContainerObjectList.Object(i));
         }
     }*/

    return valueObject.GetCount();
}

int AFCGameServerNet_ServerModule::OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& var)
{
    if(CLASS_OBJECT_EVENT::COE_DESTROY == eClassEvent)
    {
        //SaveDataToNoSql( self, true );
        m_pLogModule->LogInfo(self, "Player Offline", "");
    }
    else if(CLASS_OBJECT_EVENT::COE_CREATE_LOADDATA == eClassEvent)
    {
        //LoadDataFormNoSql( self );
    }
    else if(CLASS_OBJECT_EVENT::COE_CREATE_FINISH == eClassEvent)
    {
        m_pKernelModule->AddEventCallBack(self, NFED_ON_OBJECT_ENTER_SCENE_BEFORE, this, &AFCGameServerNet_ServerModule::OnSwapSceneResultEvent);
    }

    return 0;
}

int AFCGameServerNet_ServerModule::OnSwapSceneResultEvent(const AFGUID& self, const int nEventID, const AFIDataList& var)
{
    /*  if(var.GetCount() != 5 ||
              !var.TypeEx(AF_DATA_TYPE::DT_OBJECT, AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_INT,
                          AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::TDATA_POINT, AF_DATA_TYPE::TDATA_UNKNOWN)
        )
      {
          return 1;
      }

      AFGUID ident = var.Object(0);
      int nType = var.Int(1);
      int nTargetScene = var.Int(2);
      int nTargetGroupID = var.Int(3);
      const Point3D& xPos = var.Point(4);

      AFMsg::ReqAckSwapScene xSwapScene;
      xSwapScene.set_transfer_type(AFMsg::ReqAckSwapScene::EGameSwapType::ReqAckSwapScene_EGameSwapType_EGST_NARMAL);
      xSwapScene.set_scene_id(nTargetScene);
      xSwapScene.set_line_id(nTargetGroupID);
      xSwapScene.set_x(xPos.x);
      xSwapScene.set_y(xPos.y);
      xSwapScene.set_z(xPos.z);

      SendMsgPBToGate(AFMsg::EGMI_ACK_SWAP_SCENE, xSwapScene, self);*/

    return 0;
}

void AFCGameServerNet_ServerModule::OnReqiureRoleListProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //fd
    AFGUID nClientID;
    AFMsg::ReqRoleList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nClientID))
    {
        return;
    }

    AFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
    m_pNetModule->SendMsgPB(AFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList, nClientID, AFGUID(0));
}

void AFCGameServerNet_ServerModule::OnCreateRoleGameProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nClientID;
    AFMsg::ReqCreateRole xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nClientID))
    {
        return;
    }

    AFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
    AFMsg::RoleLiteInfo* pData = xAckRoleLiteInfoList.add_char_data();
    pData->mutable_id()->CopyFrom(AFINetServerModule::NFToPB(m_pUUIDModule->CreateGUID()));
    pData->set_career(xMsg.career());
    pData->set_sex(xMsg.sex());
    pData->set_race(xMsg.race());
    pData->set_noob_name(xMsg.noob_name());
    pData->set_game_id(xMsg.game_id());
    pData->set_role_level(1);
    pData->set_delete_time(0);
    pData->set_reg_time(0);
    pData->set_last_offline_time(0);
    pData->set_last_offline_ip(0);
    pData->set_view_record("");

    m_pNetModule->SendMsgPB(AFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList,   nClientID, AFGUID(0));
}

void AFCGameServerNet_ServerModule::OnDeleteRoleGameProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqDeleteRole xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }


    AFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
    m_pNetModule->SendMsgPB(AFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList, xClientID,   nPlayerID);
}

void AFCGameServerNet_ServerModule::OnClienSwapSceneProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    CLIENT_MSG_PROCESS(nMsgID, msg, nLen, AFMsg::ReqAckSwapScene);

    AFCDataList varEntry;
    varEntry << pObject->Self();
    varEntry << 0;
    varEntry << xMsg.scene_id();
    varEntry << -1;
    m_pKernelModule->DoEvent(pObject->Self(), NFED_ON_CLIENT_ENTER_SCENE, varEntry);
}

void AFCGameServerNet_ServerModule::OnProxyServerRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = ARK_SHARE_PTR<GateServerInfo>(ARK_NEW GateServerInfo());
            mProxyMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xServerData.xClient = xClientID;
        *(pServerData->xServerData.pData) = xData;

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "Proxy Registered");
    }

    return;
}

void AFCGameServerNet_ServerModule::OnProxyServerUnRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        mProxyMap.RemoveElement(xData.server_id());


        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "Proxy UnRegistered");
    }

    return;
}

void AFCGameServerNet_ServerModule::OnRefreshProxyServerInfoProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = ARK_SHARE_PTR<GateServerInfo>(ARK_NEW GateServerInfo());
            mProxyMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xServerData.xClient = xClientID;
        *(pServerData->xServerData.pData) = xData;

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "Proxy Registered");
    }

    return;
}

void AFCGameServerNet_ServerModule::SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const AFGUID& self)
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

void AFCGameServerNet_ServerModule::SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const AFGUID& self)
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

AFINetServerModule* AFCGameServerNet_ServerModule::GetNetModule()
{
    return m_pNetModule;
}

bool AFCGameServerNet_ServerModule::AddPlayerGateInfo(const AFGUID& nRoleID, const AFGUID& nClientID, const int nGateID)
{
    if(nGateID <= 0)
    {
        //非法gate
        return false;
    }

    if(nClientID.IsNull())
    {
        return false;
    }

    ARK_SHARE_PTR<AFCGameServerNet_ServerModule::GateBaseInfo> pBaseData = mRoleBaseData.GetElement(nRoleID);
    if(nullptr != pBaseData)
    {
        // 已经存在
        m_pLogModule->LogError(nClientID, "player is exist, cannot enter game", "", __FUNCTION__, __LINE__);
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

    if(!mRoleBaseData.AddElement(nRoleID, ARK_SHARE_PTR<GateBaseInfo>(ARK_NEW GateBaseInfo(nGateID, nClientID))))
    {
        pServerData->xRoleInfo.erase(nRoleID);
        return false;
    }

    return true;
}

bool AFCGameServerNet_ServerModule::RemovePlayerGateInfo(const AFGUID& nRoleID)
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

ARK_SHARE_PTR<AFIGameServerNet_ServerModule::GateBaseInfo> AFCGameServerNet_ServerModule::GetPlayerGateInfo(const AFGUID& nRoleID)
{
    return mRoleBaseData.GetElement(nRoleID);
}

ARK_SHARE_PTR<AFIGameServerNet_ServerModule::GateServerInfo> AFCGameServerNet_ServerModule::GetGateServerInfo(const int nGateID)
{
    return mProxyMap.GetElement(nGateID);
}

ARK_SHARE_PTR<AFIGameServerNet_ServerModule::GateServerInfo> AFCGameServerNet_ServerModule::GetGateServerInfoByClientID(const AFGUID& nClientID)
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

void AFCGameServerNet_ServerModule::OnTransWorld(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    std::string strMsg;
    AFGUID nPlayer;
    int nHasKey = 0;
    if(AFINetServerModule::ReceivePB(nMsgID, msg, nLen, strMsg, nPlayer))
    {
        nHasKey = nPlayer.nIdent;
    }

    m_pGameServerToWorldModule->SendBySuit(nHasKey, nMsgID, msg, nLen);
}
