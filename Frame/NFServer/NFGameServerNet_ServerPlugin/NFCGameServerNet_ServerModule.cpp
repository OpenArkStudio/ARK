// -------------------------------------------------------------------------
//    @FileName         :    NFCGameServerNet_ServerModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    NFCGameServerNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#include "NFCGameServerNet_ServerModule.h"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFMessageDefine/NFProtocolDefine.hpp"

bool NFCGameServerNet_ServerModule::Init()
{
    m_pNetModule = NF_NEW NFINetModule(pPluginManager);
    return true;
}

bool NFCGameServerNet_ServerModule::AfterInit()
{
    m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
    m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
    m_pSceneProcessModule = pPluginManager->FindModule<NFISceneProcessModule>();
    m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
    m_pLogModule = pPluginManager->FindModule<NFILogModule>();
    m_pUUIDModule = pPluginManager->FindModule<NFIUUIDModule>();
    m_pGameServerToWorldModule = pPluginManager->FindModule<NFIGameServerToWorldModule>();

    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_PTWG_PROXY_REFRESH, this, &NFCGameServerNet_ServerModule::OnRefreshProxyServerInfoProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_PTWG_PROXY_REGISTERED, this, &NFCGameServerNet_ServerModule::OnProxyServerRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_PTWG_PROXY_UNREGISTERED, this, &NFCGameServerNet_ServerModule::OnProxyServerUnRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_ENTER_GAME, this, &NFCGameServerNet_ServerModule::OnClienEnterGameProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_LEAVE_GAME, this, &NFCGameServerNet_ServerModule::OnClienLeaveGameProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_ROLE_LIST, this, &NFCGameServerNet_ServerModule::OnReqiureRoleListProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_CREATE_ROLE, this, &NFCGameServerNet_ServerModule::OnCreateRoleGameProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_DELETE_ROLE, this, &NFCGameServerNet_ServerModule::OnDeleteRoleGameProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_RECOVER_ROLE, this, &NFCGameServerNet_ServerModule::OnClienSwapSceneProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_SWAP_SCENE, this, &NFCGameServerNet_ServerModule::OnClienSwapSceneProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_SEARCH_GUILD, this, &NFCGameServerNet_ServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGEC_REQ_CREATE_CHATGROUP, this, &NFCGameServerNet_ServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGEC_REQ_JOIN_CHATGROUP, this, &NFCGameServerNet_ServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGEC_REQ_LEAVE_CHATGROUP, this, &NFCGameServerNet_ServerModule::OnTransWorld);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGEC_REQ_SUBSCRIPTION_CHATGROUP, this, &NFCGameServerNet_ServerModule::OnTransWorld);

    m_pNetModule->AddEventCallBack(this, &NFCGameServerNet_ServerModule::OnSocketPSEvent);

    m_pKernelModule->RegisterCommonClassEvent(this, &NFCGameServerNet_ServerModule::OnClassCommonEvent);
    m_pKernelModule->RegisterCommonPropertyEvent(this, &NFCGameServerNet_ServerModule::OnPropertyCommonEvent);
    m_pKernelModule->RegisterCommonRecordEvent(this, &NFCGameServerNet_ServerModule::OnRecordCommonEvent);

    m_pKernelModule->AddClassCallBack(NFrame::Player::ThisName(), this, &NFCGameServerNet_ServerModule::OnObjectClassEvent);

    NF_SHARE_PTR<NFIClass> xLogicClass = m_pClassModule->GetElement("Server");
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
                const std::string& strName = m_pElementModule->GetPropertyString(strConfigName, "Name");
                const std::string& strIP = m_pElementModule->GetPropertyString(strConfigName, "IP");

                int nRet = m_pNetModule->Initialization(nMaxConnect, nPort, nServerID, nCpus);
                if(nRet < 0)
                {
                    std::ostringstream strLog;
                    strLog << "Cannot init server net, Port = " << nPort;
                    m_pLogModule->LogError(NULL_GUID, strLog, __FUNCTION__, __LINE__);
                    NFASSERT(nRet, "Cannot init server net", __FILE__, __FUNCTION__);
                    exit(0);
                }
            }
        }
    }

    return true;
}

bool NFCGameServerNet_ServerModule::Shut()
{

    return true;
}

bool NFCGameServerNet_ServerModule::Execute()
{
    return m_pNetModule->Execute();
}

void NFCGameServerNet_ServerModule::OnSocketPSEvent(const int nSockIndex, const NF_NET_EVENT eEvent, const NFGUID& xClientID, const int nServerID)
{
    if(eEvent & NF_NET_EVENT_EOF)
    {
        m_pLogModule->LogInfo(NFGUID(0, nSockIndex), "NF_NET_EVENT_EOF", "Connection closed", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex);
    }
    else if(eEvent & NF_NET_EVENT_ERROR)
    {
        m_pLogModule->LogInfo(NFGUID(0, nSockIndex), "NF_NET_EVENT_ERROR", "Got an error on the connection", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex);
    }
    else if(eEvent & NF_NET_EVENT_TIMEOUT)
    {
        m_pLogModule->LogInfo(NFGUID(0, nSockIndex), "NF_NET_EVENT_TIMEOUT", "read timeout", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex);
    }
    else  if(eEvent == NF_NET_EVENT_CONNECTED)
    {
        m_pLogModule->LogInfo(NFGUID(0, nSockIndex), "NF_NET_EVENT_CONNECTED", "connected success", __FUNCTION__, __LINE__);
        OnClientConnected(nSockIndex);
    }
}

void NFCGameServerNet_ServerModule::OnClientDisconnect(const int nAddress)
{
    //只可能是网关丢了
    int nServerID = 0;
    NF_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.First();
    while(nullptr != pServerData)
    {
        if(nAddress == pServerData->xServerData.nFD)
        {
            nServerID = pServerData->xServerData.pData->server_id();
            break;
        }

        pServerData = mProxyMap.Next();
    }

    mProxyMap.RemoveElement(nServerID);
}

void NFCGameServerNet_ServerModule::OnClientConnected(const int nAddress)
{

}

void NFCGameServerNet_ServerModule::OnClienEnterGameProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    //在进入游戏之前nPlayerID为其在网关的FD
    NFGUID nClientID;
    NFMsg::ReqEnterGameServer xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nClientID))
    {
        return;
    }

    NFGUID nRoleID = NFINetModule::PBToNF(xMsg.id());

    if(m_pKernelModule->GetObject(nRoleID))
    {
        m_pKernelModule->DestroyObject(nRoleID);
    }

    //////////////////////////////////////////////////////////////////////////

    NF_SHARE_PTR<NFCGameServerNet_ServerModule::GateBaseInfo>  pGateInfo = GetPlayerGateInfo(nRoleID);
    if(nullptr != pGateInfo)
    {
        RemovePlayerGateInfo(nRoleID);
    }

    NF_SHARE_PTR<NFCGameServerNet_ServerModule::GateServerInfo> pGateServereinfo = GetGateServerInfoBySockIndex(nSockIndex);
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
    AFDataList var;
    var.AddString("Name");
    var.AddString(xMsg.name());

    var.AddString("GateID");
    var.AddInt(nGateID);

    var.AddString("ClientID");
    var.AddObject(nClientID);

    NF_SHARE_PTR<NFIObject> pObject = m_pKernelModule->CreateObject(nRoleID, nSceneID, 0, NFrame::Player::ThisName(), "", var);
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

    m_pKernelModule->DoEvent(pObject->Self(), NFrame::Player::ThisName(), CLASS_OBJECT_EVENT::COE_CREATE_FINISH, AFDataList());

    AFDataList varEntry;
    varEntry << pObject->Self();
    varEntry << NFINT64(0);
    varEntry << nSceneID;
    varEntry << -1;
    m_pKernelModule->DoEvent(pObject->Self(), NFED_ON_CLIENT_ENTER_SCENE, varEntry);
}

void NFCGameServerNet_ServerModule::OnClienLeaveGameProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    NFGUID nPlayerID;
    NFMsg::ReqLeaveGameServer xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
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

int NFCGameServerNet_ServerModule::OnPropertyEnter(const AFDataList& argVar, const NFGUID& self)
{
    if(argVar.GetCount() <= 0 || self.IsNull())
    {
        return 0;
    }

    NFMsg::MultiObjectPropertyList xPublicMsg;
    NFMsg::MultiObjectPropertyList xPrivateMsg;

    //分为自己和外人
    //1.public发送给所有人
    //2.如果自己在列表中，再次发送private数据
    NF_SHARE_PTR<NFIObject> pObject = m_pKernelModule->GetObject(self);
    if(nullptr != pObject)
    {
        NFMsg::ObjectPropertyList* pPublicData = xPublicMsg.add_multi_player_property();
        NFMsg::ObjectPropertyList* pPrivateData = xPrivateMsg.add_multi_player_property();

        *(pPublicData->mutable_player_id()) = NFINetModule::NFToPB(self);
        *(pPrivateData->mutable_player_id()) = NFINetModule::NFToPB(self);

        NF_SHARE_PTR<NFIPropertyManager> pPropertyManager = pObject->GetPropertyManager();
        NF_SHARE_PTR<NFIProperty> pPropertyInfo = pPropertyManager->First();
        while(nullptr != pPropertyInfo)
        {
            if(pPropertyInfo->Changed())
            {
                switch(pPropertyInfo->GetType())
                {
                case TDATA_INT:
                    {
                        if(pPropertyInfo->GetPublic())
                        {
                            NFMsg::PropertyInt* pDataInt = pPublicData->add_property_int_list();
                            pDataInt->set_property_name(pPropertyInfo->GetKey());
                            pDataInt->set_data(pPropertyInfo->GetInt());
                        }

                        if(pPropertyInfo->GetPrivate())
                        {
                            NFMsg::PropertyInt* pDataInt = pPrivateData->add_property_int_list();
                            pDataInt->set_property_name(pPropertyInfo->GetKey());
                            pDataInt->set_data(pPropertyInfo->GetInt());
                        }
                    }
                    break;
                case TDATA_DOUBLE:
                    {
                        if(pPropertyInfo->GetPublic())
                        {
                            NFMsg::PropertyDouble* pDataDouble = pPublicData->add_property_double_list();
                            pDataDouble->set_property_name(pPropertyInfo->GetKey());
                            pDataDouble->set_data(pPropertyInfo->GetDouble());
                        }

                        if(pPropertyInfo->GetPrivate())
                        {
                            NFMsg::PropertyDouble* pDataDouble = pPrivateData->add_property_double_list();
                            pDataDouble->set_property_name(pPropertyInfo->GetKey());
                            pDataDouble->set_data(pPropertyInfo->GetDouble());
                        }
                    }
                    break;
                case TDATA_STRING:
                    {
                        if(pPropertyInfo->GetPublic())
                        {
                            NFMsg::PropertyString* pDataString = pPublicData->add_property_string_list();
                            pDataString->set_property_name(pPropertyInfo->GetKey());
                            pDataString->set_data(pPropertyInfo->GetString());
                        }

                        if(pPropertyInfo->GetPrivate())
                        {
                            NFMsg::PropertyString* pDataString = pPrivateData->add_property_string_list();
                            pDataString->set_property_name(pPropertyInfo->GetKey());
                            pDataString->set_data(pPropertyInfo->GetString());
                        }
                    }
                    break;
                case TDATA_OBJECT:
                    {
                        if(pPropertyInfo->GetPublic())
                        {
                            NFMsg::PropertyObject* pDataObject = pPublicData->add_property_object_list();
                            pDataObject->set_property_name(pPropertyInfo->GetKey());
                            *(pDataObject->mutable_data()) = NFINetModule::NFToPB(pPropertyInfo->GetObject());
                        }

                        if(pPropertyInfo->GetPrivate())
                        {
                            NFMsg::PropertyObject* pDataObject = pPrivateData->add_property_object_list();
                            pDataObject->set_property_name(pPropertyInfo->GetKey());
                            *(pDataObject->mutable_data()) = NFINetModule::NFToPB(pPropertyInfo->GetObject());
                        }
                    }
                    break;
                case TDATA_POINT:
                    {
                        if(pPropertyInfo->GetPublic())
                        {
                            NFMsg::PropertyPoint* pDataPoint = pPublicData->add_property_point_list();
                            pDataPoint->set_property_name(pPropertyInfo->GetKey());
                            *(pDataPoint->mutable_data()) = NFINetModule::NFToPB(pPropertyInfo->GetPoint());
                        }

                        if(pPropertyInfo->GetPrivate())
                        {
                            NFMsg::PropertyPoint* pDataPoint = pPrivateData->add_property_point_list();
                            pDataPoint->set_property_name(pPropertyInfo->GetKey());
                            *(pDataPoint->mutable_data()) = NFINetModule::NFToPB(pPropertyInfo->GetPoint());
                        }
                    }
                    break;
                default:
                    break;
                }
            }

            pPropertyInfo = pPropertyManager->Next();
        }

        for(int i = 0; i < argVar.GetCount(); i++)
        {
            NFGUID identOther = argVar.Object(i);
            if(self == identOther)
            {
                //找到他所在网关的FD
                SendMsgPBToGate(NFMsg::EGMI_ACK_OBJECT_PROPERTY_ENTRY, xPrivateMsg, identOther);
            }
            else
            {
                SendMsgPBToGate(NFMsg::EGMI_ACK_OBJECT_PROPERTY_ENTRY, xPublicMsg, identOther);
            }
        }
    }

    return 0;
}

bool OnRecordEnterPack(NF_SHARE_PTR<NFIRecord> pRecord, NFMsg::ObjectRecordBase* pObjectRecordBase)
{
    if(!pRecord || !pObjectRecordBase)
    {
        return false;
    }

    for(int i = 0; i < pRecord->GetRows(); i++)
    {
        if(pRecord->IsUsed(i))
        {
            //不管public还是private都要加上，不然public广播了那不是private就广播不了了
            NFMsg::RecordAddRowStruct* pAddRowStruct = pObjectRecordBase->add_row_struct();
            pAddRowStruct->set_row(i);

            for(int j = 0; j < pRecord->GetCols(); j++)
            {
                //如果是0就不发送了，因为客户端默认是0
                AFDataList valueList;
                TDATA_TYPE eType = pRecord->GetColType(j);
                switch(eType)
                {
                case TDATA_TYPE::TDATA_INT:
                    {
                        int nValue = pRecord->GetInt(i, j);
                        //if ( 0 != nValue )
                        {
                            NFMsg::RecordInt* pAddData = pAddRowStruct->add_record_int_list();
                            pAddData->set_row(i);
                            pAddData->set_col(j);
                            pAddData->set_data(nValue);
                        }
                    }
                    break;
                case TDATA_TYPE::TDATA_DOUBLE:
                    {
                        double dwValue = pRecord->GetDouble(i, j);
                        //if ( dwValue < -0.01f || dwValue > 0.01f )
                        {
                            NFMsg::RecordDouble* pAddData = pAddRowStruct->add_record_double_list();
                            pAddData->set_row(i);
                            pAddData->set_col(j);
                            pAddData->set_data(dwValue);
                        }
                    }
                    break;
                case TDATA_TYPE::TDATA_STRING:
                    {
                        const std::string& strData = pRecord->GetString(i, j);
                        //if ( !strData.empty() )
                        {
                            NFMsg::RecordString* pAddData = pAddRowStruct->add_record_string_list();
                            pAddData->set_row(i);
                            pAddData->set_col(j);
                            pAddData->set_data(strData);
                        }
                    }
                    break;
                case TDATA_TYPE::TDATA_OBJECT:
                    {
                        NFGUID ident = pRecord->GetObject(i, j);
                        //if ( !ident.IsNull() )
                        {
                            NFMsg::RecordObject* pAddData = pAddRowStruct->add_record_object_list();
                            pAddData->set_row(i);
                            pAddData->set_col(j);
                            *(pAddData->mutable_data()) = NFINetModule::NFToPB(ident);
                        }
                    }
                    break;
                case TDATA_TYPE::TDATA_POINT:
                    {
                        const Point3D& xPoint = pRecord->GetPoint(i, j);
                        //if ( !xPoint.IsNull() )
                        {
                            NFMsg::RecordPoint* pAddData = pAddRowStruct->add_record_point_list();
                            pAddData->set_row(i);
                            pAddData->set_col(j);
                            *(pAddData->mutable_data()) = NFINetModule::NFToPB(xPoint);
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }

    return true;
}

int NFCGameServerNet_ServerModule::OnRecordEnter(const AFDataList& argVar, const NFGUID& self)
{
    if(argVar.GetCount() <= 0 || self.IsNull())
    {
        return 0;
    }

    NFMsg::MultiObjectRecordList xPublicMsg;
    NFMsg::MultiObjectRecordList xPrivateMsg;

    NF_SHARE_PTR<NFIObject> pObject = m_pKernelModule->GetObject(self);
    if(nullptr != pObject)
    {
        NFMsg::ObjectRecordList* pPublicData = NULL;
        NFMsg::ObjectRecordList* pPrivateData = NULL;

        NF_SHARE_PTR<NFIRecordManager> pRecordManager = pObject->GetRecordManager();
        NF_SHARE_PTR<NFIRecord> pRecord = pRecordManager->First();
        while(nullptr != pRecord)
        {
            if(!pRecord->GetPublic() && !pRecord->GetPrivate())
            {
                pRecord = pRecordManager->Next();
                continue;
            }

            NFMsg::ObjectRecordBase* pPrivateRecordBase = NULL;
            NFMsg::ObjectRecordBase* pPublicRecordBase = NULL;
            if(pRecord->GetPublic())
            {
                if(!pPublicData)
                {
                    pPublicData = xPublicMsg.add_multi_player_record();
                    *(pPublicData->mutable_player_id()) = NFINetModule::NFToPB(self);
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
                    *(pPrivateData->mutable_player_id()) = NFINetModule::NFToPB(self);
                }
                pPrivateRecordBase = pPrivateData->add_record_list();
                pPrivateRecordBase->set_record_name(pRecord->GetName());

                OnRecordEnterPack(pRecord, pPrivateRecordBase);
            }

            pRecord = pRecordManager->Next();
        }


        for(int i = 0; i < argVar.GetCount(); i++)
        {
            NFGUID identOther = argVar.Object(i);
            if(self == identOther)
            {
                if(xPrivateMsg.multi_player_record_size() > 0)
                {
                    SendMsgPBToGate(NFMsg::EGMI_ACK_OBJECT_RECORD_ENTRY, xPrivateMsg, identOther);
                }
            }
            else
            {
                if(xPublicMsg.multi_player_record_size() > 0)
                {
                    SendMsgPBToGate(NFMsg::EGMI_ACK_OBJECT_RECORD_ENTRY, xPublicMsg, identOther);
                }
            }
        }
    }

    return 0;
}

int NFCGameServerNet_ServerModule::OnObjectListEnter(const AFDataList& self, const AFDataList& argVar)
{
    if(self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 0;
    }

    NFMsg::AckPlayerEntryList xPlayerEntryInfoList;
    for(int i = 0; i < argVar.GetCount(); i++)
    {
        NFGUID identOld = argVar.Object(i);
        //排除空对象
        if(identOld.IsNull())
        {
            continue;
        }

        NFMsg::PlayerEntryInfo* pEntryInfo = xPlayerEntryInfoList.add_object_list();
        *(pEntryInfo->mutable_object_guid()) = NFINetModule::NFToPB(identOld);
        *pEntryInfo->mutable_pos() = NFINetModule::NFToPB(m_pKernelModule->GetPropertyPoint(identOld, "Pos"));
        pEntryInfo->set_career_type(m_pKernelModule->GetPropertyInt(identOld, "Job"));
        pEntryInfo->set_player_state(m_pKernelModule->GetPropertyInt(identOld, "State"));
        pEntryInfo->set_config_id(m_pKernelModule->GetPropertyString(identOld, "ConfigID"));
        pEntryInfo->set_scene_id(m_pKernelModule->GetPropertyInt(identOld, "SceneID"));
        pEntryInfo->set_class_id(m_pKernelModule->GetPropertyString(identOld, "ClassName"));
    }

    if(xPlayerEntryInfoList.object_list_size() <= 0)
    {
        return 0;
    }

    for(int i = 0; i < self.GetCount(); i++)
    {
        NFGUID ident = self.Object(i);
        if(ident.IsNull())
        {
            continue;
        }

        //可能在不同的网关呢,得到后者所在的网关FD
        SendMsgPBToGate(NFMsg::EGMI_ACK_OBJECT_ENTRY, xPlayerEntryInfoList, ident);
    }

    return 1;
}

int NFCGameServerNet_ServerModule::OnObjectListLeave(const AFDataList& self, const AFDataList& argVar)
{
    if(self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 0;
    }

    NFMsg::AckPlayerLeaveList xPlayerLeaveInfoList;
    for(int i = 0; i < argVar.GetCount(); i++)
    {
        NFGUID identOld = argVar.Object(i);
        //排除空对象
        if(identOld.IsNull())
        {
            continue;
        }

        NFMsg::Ident* pIdent = xPlayerLeaveInfoList.add_object_list();
        *pIdent = NFINetModule::NFToPB(argVar.Object(i));
    }

    for(int i = 0; i < self.GetCount(); i++)
    {
        NFGUID ident = self.Object(i);
        if(ident.IsNull())
        {
            continue;
        }
        //可能在不同的网关呢,得到后者所在的网关FD
        SendMsgPBToGate(NFMsg::EGMI_ACK_OBJECT_LEAVE, xPlayerLeaveInfoList, ident);
    }

    return 1;
}

int NFCGameServerNet_ServerModule::OnPropertyCommonEvent(const NFGUID& self, const std::string& strPropertyName, const AFDataList::TData& oldVar, const AFDataList::TData& newVar)
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

        if(NFrame::Player::ThisName() == m_pKernelModule->GetPropertyString(self, "ClassName"))
        {
            if(m_pKernelModule->GetPropertyInt(self, "LoadPropertyFinish") <= 0)
            {
                return 0;
            }
        }
    }

    AFDataList valueBroadCaseList;
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

    switch(oldVar.GetType())
    {
    case TDATA_INT:
        {
            NFMsg::ObjectPropertyInt xPropertyInt;
            NFMsg::Ident* pIdent = xPropertyInt.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            NFMsg::PropertyInt* pDataInt = xPropertyInt.add_property_list();
            pDataInt->set_property_name(strPropertyName);
            pDataInt->set_data(newVar.GetInt());

            for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                NFGUID identOld = valueBroadCaseList.Object(i);

                SendMsgPBToGate(NFMsg::EGMI_ACK_PROPERTY_INT, xPropertyInt, identOld);
            }
        }
        break;

    case TDATA_DOUBLE:
        {
            NFMsg::ObjectPropertyDouble xPropertyDouble;
            NFMsg::Ident* pIdent = xPropertyDouble.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            NFMsg::PropertyDouble* pDataDouble = xPropertyDouble.add_property_list();
            pDataDouble->set_property_name(strPropertyName);
            pDataDouble->set_data(newVar.GetDouble());

            for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                NFGUID identOld = valueBroadCaseList.Object(i);

                SendMsgPBToGate(NFMsg::EGMI_ACK_PROPERTY_DOUBLE, xPropertyDouble, identOld);
            }
        }
        break;

    case TDATA_STRING:
        {
            NFMsg::ObjectPropertyString xPropertyString;
            NFMsg::Ident* pIdent = xPropertyString.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            NFMsg::PropertyString* pDataString = xPropertyString.add_property_list();
            pDataString->set_property_name(strPropertyName);
            pDataString->set_data(newVar.GetString());

            for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                NFGUID identOld = valueBroadCaseList.Object(i);

                SendMsgPBToGate(NFMsg::EGMI_ACK_PROPERTY_STRING, xPropertyString, identOld);
            }
        }
        break;

    case TDATA_OBJECT:
        {
            NFMsg::ObjectPropertyObject xPropertyObject;
            NFMsg::Ident* pIdent = xPropertyObject.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            NFMsg::PropertyObject* pDataObject = xPropertyObject.add_property_list();
            pDataObject->set_property_name(strPropertyName);
            *pDataObject->mutable_data() = NFINetModule::NFToPB(newVar.GetObject());

            for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                NFGUID identOld = valueBroadCaseList.Object(i);

                SendMsgPBToGate(NFMsg::EGMI_ACK_PROPERTY_OBJECT, xPropertyObject, identOld);
            }
        }
        break;
    case TDATA_POINT:
        {
            NFMsg::ObjectPropertyPoint xPropertyPoint;
            NFMsg::Ident* pIdent = xPropertyPoint.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            NFMsg::PropertyPoint* pDataPoint = xPropertyPoint.add_property_list();
            pDataPoint->set_property_name(strPropertyName);
            *pDataPoint->mutable_data() = NFINetModule::NFToPB(newVar.GetPoint());

            for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                NFGUID identOld = valueBroadCaseList.Object(i);

                SendMsgPBToGate(NFMsg::EGMI_ACK_PROPERTY_OBJECT, xPropertyPoint, identOld);
            }
        }
        break;
    default:
        break;
    }

    return 0;
}

int NFCGameServerNet_ServerModule::OnRecordCommonEvent(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFDataList::TData& oldVar, const AFDataList::TData& newVar)
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

    if(NFrame::Player::ThisName() == m_pKernelModule->GetPropertyString(self, "ClassName"))
    {
        if(m_pKernelModule->GetPropertyInt(self, "LoadPropertyFinish") <= 0)
        {
            return 0;
        }
    }

    AFDataList valueBroadCaseList;
    GetBroadCastObject(self, strRecordName, true, valueBroadCaseList);

    switch(nOpType)
    {
    case NFIRecord::RecordOptype::Add:
        {
            NFMsg::ObjectRecordAddRow xAddRecordRow;
            NFMsg::Ident* pIdent = xAddRecordRow.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            xAddRecordRow.set_record_name(strRecordName);

            NFMsg::RecordAddRowStruct* pAddRowData = xAddRecordRow.add_row_data();
            pAddRowData->set_row(nRow);

            //add row 需要完整的row
            NF_SHARE_PTR<NFIRecord> xRecord = m_pKernelModule->FindRecord(self, strRecordName);
            if(xRecord)
            {
                AFDataList xRowDataList;
                if(xRecord->QueryRow(nRow, xRowDataList))
                {
                    for(int i = 0; i < xRowDataList.GetCount(); i++)
                    {
                        switch(xRowDataList.Type(i))
                        {
                        case TDATA_INT:
                            {
                                //添加的时候数据要全s
                                int nValue = xRowDataList.Int(i);
                                //if ( 0 != nValue )
                                {
                                    NFMsg::RecordInt* pAddData = pAddRowData->add_record_int_list();
                                    pAddData->set_col(i);
                                    pAddData->set_row(nRow);
                                    pAddData->set_data(nValue);
                                }
                            }
                            break;
                        case TDATA_DOUBLE:
                            {
                                double dValue = xRowDataList.Double(i);
                                //if ( fValue > 0.001f  || fValue < -0.001f )
                                {
                                    NFMsg::RecordDouble* pAddData = pAddRowData->add_record_double_list();
                                    pAddData->set_col(i);
                                    pAddData->set_row(nRow);
                                    pAddData->set_data(dValue);
                                }
                            }
                            break;
                        case TDATA_STRING:
                            {
                                const std::string& str = xRowDataList.String(i);
                                //if (!str.empty())
                                {
                                    NFMsg::RecordString* pAddData = pAddRowData->add_record_string_list();
                                    pAddData->set_col(i);
                                    pAddData->set_row(nRow);
                                    pAddData->set_data(str);
                                }
                            }
                            break;
                        case TDATA_OBJECT:
                            {
                                NFGUID identValue = xRowDataList.Object(i);
                                //if (!identValue.IsNull())
                                {
                                    NFMsg::RecordObject* pAddData = pAddRowData->add_record_object_list();
                                    pAddData->set_col(i);
                                    pAddData->set_row(nRow);
                                    *pAddData->mutable_data() = NFINetModule::NFToPB(identValue);
                                }
                            }
                            break;
                        case TDATA_TYPE::TDATA_POINT:
                            {
                                const Point3D& xPoint = xRowDataList.Point(i);
                                //if ( !xPoint.IsNull() )
                                {
                                    NFMsg::RecordPoint* pAddData = pAddRowData->add_record_point_list();
                                    pAddData->set_row(i);
                                    pAddData->set_col(nRow);
                                    *(pAddData->mutable_data()) = NFINetModule::NFToPB(xPoint);
                                }
                            }
                            break;
                        default:
                            break;
                        }
                    }

                    for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
                    {
                        NFGUID identOther = valueBroadCaseList.Object(i);

                        SendMsgPBToGate(NFMsg::EGMI_ACK_ADD_ROW, xAddRecordRow, identOther);
                    }
                }
            }
        }
        break;
    case NFIRecord::RecordOptype::Del:
        {
            NFMsg::ObjectRecordRemove xReoveRecordRow;

            NFMsg::Ident* pIdent = xReoveRecordRow.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            xReoveRecordRow.set_record_name(strRecordName);
            xReoveRecordRow.add_remove_row(nRow);

            for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                NFGUID identOther = valueBroadCaseList.Object(i);

                SendMsgPBToGate(NFMsg::EGMI_ACK_REMOVE_ROW, xReoveRecordRow, identOther);
            }
        }
        break;
    case NFIRecord::RecordOptype::Swap:
        {
            //其实是2个row交换
            NFMsg::ObjectRecordSwap xSwapRecord;
            *xSwapRecord.mutable_player_id() = NFINetModule::NFToPB(self);

            xSwapRecord.set_origin_record_name(strRecordName);
            xSwapRecord.set_target_record_name(strRecordName);   // 暂时没用
            xSwapRecord.set_row_origin(nRow);
            xSwapRecord.set_row_target(nCol);

            for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
            {
                NFGUID identOther = valueBroadCaseList.Object(i);

                SendMsgPBToGate(NFMsg::EGMI_ACK_SWAP_ROW, xSwapRecord, identOther);
            }
        }
        break;
    case NFIRecord::RecordOptype::Update:
        {
            switch(oldVar.GetType())
            {
            case TDATA_INT:
                {
                    NFMsg::ObjectRecordInt xRecordChanged;
                    *xRecordChanged.mutable_player_id() = NFINetModule::NFToPB(self);

                    xRecordChanged.set_record_name(strRecordName);
                    NFMsg::RecordInt* recordProperty = xRecordChanged.add_record_list();
                    recordProperty->set_row(nRow);
                    recordProperty->set_col(nCol);
                    int nData = newVar.GetInt();
                    recordProperty->set_data(nData);

                    for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
                    {
                        NFGUID identOther = valueBroadCaseList.Object(i);

                        SendMsgPBToGate(NFMsg::EGMI_ACK_RECORD_INT, xRecordChanged, identOther);
                    }
                }
                break;
            case TDATA_DOUBLE:
                {
                    NFMsg::ObjectRecordDouble xRecordChanged;
                    *xRecordChanged.mutable_player_id() = NFINetModule::NFToPB(self);

                    xRecordChanged.set_record_name(strRecordName);
                    NFMsg::RecordDouble* recordProperty = xRecordChanged.add_record_list();
                    recordProperty->set_row(nRow);
                    recordProperty->set_col(nCol);
                    recordProperty->set_data(newVar.GetDouble());

                    for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
                    {
                        NFGUID identOther = valueBroadCaseList.Object(i);

                        SendMsgPBToGate(NFMsg::EGMI_ACK_RECORD_DOUBLE, xRecordChanged, identOther);
                    }
                }
                break;
            case TDATA_STRING:
                {
                    NFMsg::ObjectRecordString xRecordChanged;
                    *xRecordChanged.mutable_player_id() = NFINetModule::NFToPB(self);

                    xRecordChanged.set_record_name(strRecordName);
                    NFMsg::RecordString* recordProperty = xRecordChanged.add_record_list();
                    recordProperty->set_row(nRow);
                    recordProperty->set_col(nCol);
                    recordProperty->set_data(newVar.GetString());

                    for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
                    {
                        NFGUID identOther = valueBroadCaseList.Object(i);

                        SendMsgPBToGate(NFMsg::EGMI_ACK_RECORD_STRING, xRecordChanged, identOther);
                    }
                }
                break;
            case TDATA_OBJECT:
                {
                    NFMsg::ObjectRecordObject xRecordChanged;
                    *xRecordChanged.mutable_player_id() = NFINetModule::NFToPB(self);

                    xRecordChanged.set_record_name(strRecordName);
                    NFMsg::RecordObject* recordProperty = xRecordChanged.add_record_list();
                    recordProperty->set_row(nRow);
                    recordProperty->set_col(nCol);
                    *recordProperty->mutable_data() = NFINetModule::NFToPB(newVar.GetObject());

                    for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
                    {
                        NFGUID identOther = valueBroadCaseList.Object(i);

                        SendMsgPBToGate(NFMsg::EGMI_ACK_RECORD_OBJECT, xRecordChanged, identOther);
                    }
                }
                break;
            case TDATA_POINT:
                {
                    NFMsg::ObjectRecordPoint xRecordChanged;
                    *xRecordChanged.mutable_player_id() = NFINetModule::NFToPB(self);

                    xRecordChanged.set_record_name(strRecordName);
                    NFMsg::RecordPoint* recordProperty = xRecordChanged.add_record_list();
                    recordProperty->set_row(nRow);
                    recordProperty->set_col(nCol);
                    *recordProperty->mutable_data() = NFINetModule::NFToPB(newVar.GetPoint());

                    for(int i = 0; i < valueBroadCaseList.GetCount(); i++)
                    {
                        NFGUID identOther = valueBroadCaseList.Object(i);

                        SendMsgPBToGate(NFMsg::EGMI_ACK_RECORD_OBJECT, xRecordChanged, identOther);
                    }
                }
                break;
            default:
                return 0;
                break;
            }
        }
        break;
    case NFIRecord::RecordOptype::Create:
        break;
    case NFIRecord::RecordOptype::Cleared:
        break;
    default:
        break;
    }

    return 0;
}

int NFCGameServerNet_ServerModule::OnClassCommonEvent(const NFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFDataList& var)
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

        AFDataList valueAllObjectList;
        AFDataList valueBroadCaseList;
        AFDataList valueBroadListNoSelf;
        m_pKernelModule->GetGroupObjectList(nObjectContainerID, nObjectGroupID, valueAllObjectList);

        for(int i = 0; i < valueAllObjectList.GetCount(); i++)
        {
            NFGUID identBC = valueAllObjectList.Object(i);
            const std::string& strClassName = m_pKernelModule->GetPropertyString(identBC, "ClassName");
            if(NFrame::Player::ThisName() == strClassName)
            {
                valueBroadCaseList.Add(identBC);
                if(identBC != self)
                {
                    valueBroadListNoSelf.Add(identBC);
                }
            }
        }

        //如果是副本的怪，则不需要发送，因为会在离开副本的时候一次性一条消息发送
        OnObjectListLeave(valueBroadListNoSelf, AFDataList() << self);
    }

    else if(CLASS_OBJECT_EVENT::COE_CREATE_NODATA == eClassEvent)
    {
        //id和fd,gateid绑定
        NF_SHARE_PTR<GateBaseInfo> pDataBase = mRoleBaseData.GetElement(self);
        if(nullptr != pDataBase)
        {
            //回复客户端角色进入游戏世界成功了
            NFMsg::AckEventResult xMsg;
            xMsg.set_event_code(NFMsg::EGEC_ENTER_GAME_SUCCESS);

            *xMsg.mutable_event_client() = NFINetModule::NFToPB(pDataBase->xClientID);
            *xMsg.mutable_event_object() = NFINetModule::NFToPB(self);

            SendMsgPBToGate(NFMsg::EGMI_ACK_ENTER_GAME, xMsg, self);
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
            OnObjectListEnter(AFDataList() << self, AFDataList() << self);

            OnPropertyEnter(AFDataList() << self, self);
            OnRecordEnter(AFDataList() << self, self);
        }
    }
    else if(CLASS_OBJECT_EVENT::COE_CREATE_FINISH == eClassEvent)
    {

    }
    return 0;
}

int NFCGameServerNet_ServerModule::OnGroupEvent(const NFGUID& self, const std::string& strPropertyName, const AFDataList::TData& oldVar, const AFDataList::TData& newVar)
{
    //容器发生变化，只可能从A容器的0层切换到B容器的0层
    //需要注意的是------------任何层改变的时候，此玩家其实还未进入层，因此，层改变的时候获取的玩家列表，目标层是不包含自己的
    int nSceneID = m_pKernelModule->GetPropertyInt(self, "SceneID");

    //广播给别人自己离去(层降或者跃层)
    int nOldGroupID = oldVar.GetInt();
    if(nOldGroupID > 0)
    {
        AFDataList valueAllOldObjectList;
        AFDataList valueAllOldPlayerList;
        m_pKernelModule->GetGroupObjectList(nSceneID, nOldGroupID, valueAllOldObjectList);
        if(valueAllOldObjectList.GetCount() > 0)
        {
            //自己只需要广播其他玩家
            for(int i = 0; i < valueAllOldObjectList.GetCount(); i++)
            {
                NFGUID identBC = valueAllOldObjectList.Object(i);

                if(valueAllOldObjectList.Object(i) == self)
                {
                    valueAllOldObjectList.Set(i, NFGUID());
                }

                const std::string& strClassName = m_pKernelModule->GetPropertyString(identBC, "ClassName");
                if(NFrame::Player::ThisName() == strClassName)
                {
                    valueAllOldPlayerList.Add(identBC);
                }
            }

            OnObjectListLeave(valueAllOldPlayerList, AFDataList() << self);

            //老的全部要广播删除
            OnObjectListLeave(AFDataList() << self, valueAllOldObjectList);
        }

        m_pKernelModule->DoEvent(self, NFED_ON_CLIENT_LEAVE_SCENE, AFDataList() << nOldGroupID);
    }

    //再广播给别人自己出现(层升或者跃层)
    int nNewGroupID = newVar.GetInt();
    if(nNewGroupID > 0)
    {
        //这里需要把自己从广播中排除
        //////////////////////////////////////////////////////////////////////////
        AFDataList valueAllObjectList;
        AFDataList valueAllObjectListNoSelf;
        AFDataList valuePlayerList;
        AFDataList valuePlayerListNoSelf;
        m_pKernelModule->GetGroupObjectList(nSceneID, nNewGroupID, valueAllObjectList);
        for(int i = 0; i < valueAllObjectList.GetCount(); i++)
        {
            NFGUID identBC = valueAllObjectList.Object(i);
            const std::string& strClassName = m_pKernelModule->GetPropertyString(identBC, "ClassName");
            if(NFrame::Player::ThisName() == strClassName)
            {
                valuePlayerList.Add(identBC);
                if(identBC != self)
                {
                    valuePlayerListNoSelf.Add(identBC);
                }
            }

            if(identBC != self)
            {
                valueAllObjectListNoSelf.Add(identBC);
            }
        }

        //广播给别人,自己出现(这里本不应该广播给自己)
        if(valuePlayerListNoSelf.GetCount() > 0)
        {
            OnObjectListEnter(valuePlayerListNoSelf, AFDataList() << self);
        }

        const std::string& strSelfClassName = m_pKernelModule->GetPropertyString(self, "ClassName");

        //广播给自己,所有的别人出现
        if(valueAllObjectListNoSelf.GetCount() > 0)
        {
            if(strSelfClassName == NFrame::Player::ThisName())
            {
                OnObjectListEnter(AFDataList() << self, valueAllObjectListNoSelf);
            }
        }

        if(strSelfClassName == NFrame::Player::ThisName())
        {
            for(int i = 0; i < valueAllObjectListNoSelf.GetCount(); i++)
            {
                //此时不用再广播自己的属性给自己
                //把已经存在的人的属性广播给新来的人
                NFGUID identOld = valueAllObjectListNoSelf.Object(i);

                OnPropertyEnter(AFDataList() << self, identOld);
                //把已经存在的人的表广播给新来的人
                OnRecordEnter(AFDataList() << self, identOld);
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

int NFCGameServerNet_ServerModule::OnContainerEvent(const NFGUID& self, const std::string& strPropertyName, const AFDataList::TData& oldVar, const AFDataList::TData& newVar)
{
    //容器发生变化，只可能从A容器的0层切换到B容器的0层
    //需要注意的是------------任何容器改变的时候，玩家必须是0层
    int nOldSceneID = oldVar.GetInt();
    int nNowSceneID = newVar.GetInt();

    m_pLogModule->LogInfo(self, "Enter Scene:", nNowSceneID);

    //自己消失,玩家不用广播，因为在消失之前，会回到0层，早已广播了玩家
    AFDataList valueOldAllObjectList;
    AFDataList valueNewAllObjectList;
    AFDataList valueAllObjectListNoSelf;
    AFDataList valuePlayerList;
    AFDataList valuePlayerNoSelf;

    m_pKernelModule->GetGroupObjectList(nOldSceneID, 0, valueOldAllObjectList);
    m_pKernelModule->GetGroupObjectList(nNowSceneID, 0, valueNewAllObjectList);

    for(int i = 0; i < valueOldAllObjectList.GetCount(); i++)
    {
        NFGUID identBC = valueOldAllObjectList.Object(i);
        if(identBC == self)
        {
            valueOldAllObjectList.Set(i, NFGUID());
        }
    }

    for(int i = 0; i < valueNewAllObjectList.GetCount(); i++)
    {
        NFGUID identBC = valueNewAllObjectList.Object(i);
        const std::string& strClassName = m_pKernelModule->GetPropertyString(identBC, "ClassName");
        if(NFrame::Player::ThisName() == strClassName)
        {
            valuePlayerList.Add(identBC);
            if(identBC != self)
            {
                valuePlayerNoSelf.Add(identBC);
            }
        }

        if(identBC != self)
        {
            valueAllObjectListNoSelf.Add(identBC);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    //但是旧场景0层的NPC需要广播
    OnObjectListLeave(AFDataList() << self, valueOldAllObjectList);

    //广播给所有人出现对象(如果是玩家，则包括广播给自己)
    //这里广播的都是0层的
    if(valuePlayerList.GetCount() > 0)
    {
        //把self广播给argVar这些人
        OnObjectListEnter(valuePlayerNoSelf, AFDataList() << self);
    }

    //新层必然是0，把0层NPC广播给自己------------自己广播给自己不在这里广播，因为场景ID在跨场景时会经常变化

    //把valueAllObjectList广播给self
    OnObjectListEnter(AFDataList() << self, valueAllObjectListNoSelf);

    ////////////////////把已经存在的人的属性广播给新来的人//////////////////////////////////////////////////////
    for(int i = 0; i < valueAllObjectListNoSelf.GetCount(); i++)
    {
        NFGUID identOld = valueAllObjectListNoSelf.Object(i);
        OnPropertyEnter(AFDataList() << self, identOld);
        ////////////////////把已经存在的人的表广播给新来的人//////////////////////////////////////////////////////
        OnRecordEnter(AFDataList() << self, identOld);
    }

    //把新来的人的属性广播给周边的人()
    if(valuePlayerNoSelf.GetCount() > 0)
    {
        OnPropertyEnter(valuePlayerNoSelf, self);
        OnRecordEnter(valuePlayerNoSelf, self);
    }

    return 0;
}

int NFCGameServerNet_ServerModule::GetBroadCastObject(const NFGUID& self, const std::string& strPropertyName, const bool bTable, AFDataList& valueObject)
{
    int nObjectContainerID = m_pKernelModule->GetPropertyInt(self, "SceneID");
    int nObjectGroupID = m_pKernelModule->GetPropertyInt(self, "GroupID");

    //普通场景容器，判断广播属性
    std::string strClassName = m_pKernelModule->GetPropertyString(self, "ClassName");
    NF_SHARE_PTR<NFIRecordManager> pClassRecordManager = m_pClassModule->GetClassRecordManager(strClassName);
    NF_SHARE_PTR<NFIPropertyManager> pClassPropertyManager = m_pClassModule->GetClassPropertyManager(strClassName);

    NF_SHARE_PTR<NFIRecord> pRecord;
    NF_SHARE_PTR<NFIProperty> pProperty;
    if(bTable)
    {
        if(nullptr == pClassRecordManager)
        {
            return -1;
        }

        pRecord = pClassRecordManager->GetElement(strPropertyName);
        if(nullptr == pRecord)
        {
            return -1;
        }
    }
    else
    {
        if(nullptr == pClassPropertyManager)
        {
            return -1;
        }

        pProperty = pClassPropertyManager->GetElement(strPropertyName);
        if(nullptr == pProperty)
        {
            return -1;
        }
    }

    if(NFrame::Player::ThisName() == strClassName)
    {
        if(bTable)
        {
            if(pRecord->GetPublic())
            {
                GetBroadCastObject(nObjectContainerID, nObjectGroupID, valueObject);
            }
            else if(pRecord->GetPrivate())
            {
                valueObject.Add(self);
            }
        }
        else
        {
            if(pProperty->GetPublic())
            {
                GetBroadCastObject(nObjectContainerID, nObjectGroupID, valueObject);
            }
            else if(pProperty->GetPrivate())
            {
                valueObject.Add(self);
            }
        }
        //一个玩家都不广播
        return valueObject.GetCount();
    }

    //不是玩家,NPC和怪物类等
    if(bTable)
    {
        if(pRecord->GetPublic())
        {
            //广播给客户端自己和周边人
            GetBroadCastObject(nObjectContainerID, nObjectGroupID, valueObject);
        }
    }
    else
    {
        if(pProperty->GetPublic())
        {
            //广播给客户端自己和周边人
            GetBroadCastObject(nObjectContainerID, nObjectGroupID, valueObject);
        }
    }

    return valueObject.GetCount();
}

int NFCGameServerNet_ServerModule::GetBroadCastObject(const int nObjectContainerID, const int nGroupID, AFDataList& valueObject)
{
    AFDataList valContainerObjectList;
    m_pKernelModule->GetGroupObjectList(nObjectContainerID, nGroupID, valContainerObjectList);
    for(int i = 0; i < valContainerObjectList.GetCount(); i++)
    {
        const std::string& strObjClassName = m_pKernelModule->GetPropertyString(valContainerObjectList.Object(i), "ClassName");
        if(NFrame::Player::ThisName() == strObjClassName)
        {
            valueObject.Add(valContainerObjectList.Object(i));
        }
    }

    return valueObject.GetCount();
}

int NFCGameServerNet_ServerModule::OnObjectClassEvent(const NFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFDataList& var)
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
        m_pKernelModule->AddEventCallBack(self, NFED_ON_OBJECT_ENTER_SCENE_BEFORE, this, &NFCGameServerNet_ServerModule::OnSwapSceneResultEvent);
    }

    return 0;
}

int NFCGameServerNet_ServerModule::OnSwapSceneResultEvent(const NFGUID& self, const int nEventID, const AFDataList& var)
{
    if(var.GetCount() != 5 ||
            !var.TypeEx(TDATA_TYPE::TDATA_OBJECT, TDATA_TYPE::TDATA_INT, TDATA_TYPE::TDATA_INT,
                        TDATA_TYPE::TDATA_INT, TDATA_TYPE::TDATA_POINT, TDATA_TYPE::TDATA_UNKNOWN)
      )
    {
        return 1;
    }

    NFGUID ident = var.Object(0);
    int nType = var.Int(1);
    int nTargetScene = var.Int(2);
    int nTargetGroupID = var.Int(3);
    const Point3D& xPos = var.Point(4);

    NFMsg::ReqAckSwapScene xSwapScene;
    xSwapScene.set_transfer_type(NFMsg::ReqAckSwapScene::EGameSwapType::ReqAckSwapScene_EGameSwapType_EGST_NARMAL);
    xSwapScene.set_scene_id(nTargetScene);
    xSwapScene.set_line_id(nTargetGroupID);
    xSwapScene.set_x(xPos.x);
    xSwapScene.set_y(xPos.y);
    xSwapScene.set_z(xPos.z);

    SendMsgPBToGate(NFMsg::EGMI_ACK_SWAP_SCENE, xSwapScene, self);

    return 0;
}

void NFCGameServerNet_ServerModule::OnReqiureRoleListProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    //fd
    NFGUID nClientID;
    NFMsg::ReqRoleList xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nClientID))
    {
        return;
    }

    NFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
    m_pNetModule->SendMsgPB(NFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList, nSockIndex, nClientID);
}

void NFCGameServerNet_ServerModule::OnCreateRoleGameProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    NFGUID nClientID;
    NFMsg::ReqCreateRole xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nClientID))
    {
        return;
    }

    NFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
    NFMsg::RoleLiteInfo* pData = xAckRoleLiteInfoList.add_char_data();
    pData->mutable_id()->CopyFrom(NFINetModule::NFToPB(m_pUUIDModule->CreateGUID()));
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

    m_pNetModule->SendMsgPB(NFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList, nSockIndex, nClientID);
}

void NFCGameServerNet_ServerModule::OnDeleteRoleGameProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    NFGUID nPlayerID;
    NFMsg::ReqDeleteRole xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }


    NFMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
    m_pNetModule->SendMsgPB(NFMsg::EGMI_ACK_ROLE_LIST, xAckRoleLiteInfoList, nSockIndex, nPlayerID);
}

void NFCGameServerNet_ServerModule::OnClienSwapSceneProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    CLIENT_MSG_PROCESS(nSockIndex, nMsgID, msg, nLen, NFMsg::ReqAckSwapScene);

    AFDataList varEntry;
    varEntry << pObject->Self();
    varEntry << 0;
    varEntry << xMsg.scene_id();
    varEntry << -1;
    m_pKernelModule->DoEvent(pObject->Self(), NFED_ON_CLIENT_ENTER_SCENE, varEntry);
}

void NFCGameServerNet_ServerModule::OnProxyServerRegisteredProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    NFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        NF_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = NF_SHARE_PTR<GateServerInfo>(NF_NEW GateServerInfo());
            mProxyMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xServerData.nFD = nSockIndex;
        *(pServerData->xServerData.pData) = xData;

        m_pLogModule->LogInfo(NFGUID(0, xData.server_id()), xData.server_name(), "Proxy Registered");
    }

    return;
}

void NFCGameServerNet_ServerModule::OnProxyServerUnRegisteredProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    NFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        mProxyMap.RemoveElement(xData.server_id());


        m_pLogModule->LogInfo(NFGUID(0, xData.server_id()), xData.server_name(), "Proxy UnRegistered");
    }

    return;
}

void NFCGameServerNet_ServerModule::OnRefreshProxyServerInfoProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    NFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        NF_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = NF_SHARE_PTR<GateServerInfo>(NF_NEW GateServerInfo());
            mProxyMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xServerData.nFD = nSockIndex;
        *(pServerData->xServerData.pData) = xData;

        m_pLogModule->LogInfo(NFGUID(0, xData.server_id()), xData.server_name(), "Proxy Registered");
    }

    return;
}

void NFCGameServerNet_ServerModule::SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const NFGUID& self)
{
    NF_SHARE_PTR<GateBaseInfo> pData = mRoleBaseData.GetElement(self);
    if(nullptr != pData)
    {
        NF_SHARE_PTR<GateServerInfo> pProxyData = mProxyMap.GetElement(pData->nGateID);
        if(nullptr != pProxyData)
        {
            m_pNetModule->SendMsgPB(nMsgID, xMsg, pProxyData->xServerData.nFD, pData->xClientID);
        }
    }
}

void NFCGameServerNet_ServerModule::SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const NFGUID& self)
{
    NF_SHARE_PTR<GateBaseInfo> pData = mRoleBaseData.GetElement(self);
    if(nullptr != pData)
    {
        NF_SHARE_PTR<GateServerInfo> pProxyData = mProxyMap.GetElement(pData->nGateID);
        if(nullptr != pProxyData)
        {
            m_pNetModule->SendMsgPB(nMsgID, strMsg, pProxyData->xServerData.nFD, pData->xClientID);
        }
    }
}

NFINetModule* NFCGameServerNet_ServerModule::GetNetModule()
{
    return m_pNetModule;
}

bool NFCGameServerNet_ServerModule::AddPlayerGateInfo(const NFGUID& nRoleID, const NFGUID& nClientID, const int nGateID)
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

    NF_SHARE_PTR<NFCGameServerNet_ServerModule::GateBaseInfo> pBaseData = mRoleBaseData.GetElement(nRoleID);
    if(nullptr != pBaseData)
    {
        // 已经存在
        m_pLogModule->LogError(nClientID, "player is exist, cannot enter game", "", __FUNCTION__, __LINE__);
        return false;
    }

    NF_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(nGateID);
    if(nullptr == pServerData)
    {
        return false;
    }

    if(!pServerData->xRoleInfo.insert(std::make_pair(nRoleID, pServerData->xServerData.nFD)).second)
    {
        return false;
    }

    if(!mRoleBaseData.AddElement(nRoleID, NF_SHARE_PTR<GateBaseInfo>(NF_NEW GateBaseInfo(nGateID, nClientID))))
    {
        pServerData->xRoleInfo.erase(nRoleID);
        return false;
    }

    return true;
}

bool NFCGameServerNet_ServerModule::RemovePlayerGateInfo(const NFGUID& nRoleID)
{
    NF_SHARE_PTR<GateBaseInfo> pBaseData = mRoleBaseData.GetElement(nRoleID);
    if(nullptr == pBaseData)
    {
        return false;
    }

    mRoleBaseData.RemoveElement(nRoleID);

    NF_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.GetElement(pBaseData->nGateID);
    if(nullptr == pServerData)
    {
        return false;
    }

    pServerData->xRoleInfo.erase(nRoleID);
    return true;
}

NF_SHARE_PTR<NFIGameServerNet_ServerModule::GateBaseInfo> NFCGameServerNet_ServerModule::GetPlayerGateInfo(const NFGUID& nRoleID)
{
    return mRoleBaseData.GetElement(nRoleID);
}

NF_SHARE_PTR<NFIGameServerNet_ServerModule::GateServerInfo> NFCGameServerNet_ServerModule::GetGateServerInfo(const int nGateID)
{
    return mProxyMap.GetElement(nGateID);
}

NF_SHARE_PTR<NFIGameServerNet_ServerModule::GateServerInfo> NFCGameServerNet_ServerModule::GetGateServerInfoBySockIndex(const int nSockIndex)
{
    int nGateID = -1;
    NF_SHARE_PTR<GateServerInfo> pServerData = mProxyMap.First();
    while(nullptr != pServerData)
    {
        if(nSockIndex == pServerData->xServerData.nFD)
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

void NFCGameServerNet_ServerModule::OnTransWorld(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    std::string strMsg;
    NFGUID nPlayer;
    int nHasKey = 0;
    if(NFINetModule::ReceivePB(nSockIndex, nMsgID, msg, nLen, strMsg, nPlayer))
    {
        nHasKey = nPlayer.nData64;
    }

    m_pGameServerToWorldModule->SendBySuit(nHasKey, nMsgID, msg, nLen);
}
