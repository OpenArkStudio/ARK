// -------------------------------------------------------------------------
//    @FileName         :    AFCWorldNet_ServerModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCWorldNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#include "AFCWorldNet_ServerModule.h"
#include "NFWorldNet_ServerPlugin.h"
#include "SDK/Proto/NFMsgDefine.h"

bool AFCWorldNet_ServerModule::Init()
{
    m_pNetModule = NF_NEW AFINetModule(pPluginManager);
    return true;
}

bool AFCWorldNet_ServerModule::AfterInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pWorldLogicModule = pPluginManager->FindModule<AFIWorldLogicModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();

    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_PTWG_PROXY_REFRESH, this, &AFCWorldNet_ServerModule::OnRefreshProxyServerInfoProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_PTWG_PROXY_REGISTERED, this, &AFCWorldNet_ServerModule::OnProxyServerRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_PTWG_PROXY_UNREGISTERED, this, &AFCWorldNet_ServerModule::OnProxyServerUnRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_GTW_GAME_REGISTERED, this, &AFCWorldNet_ServerModule::OnGameServerRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_GTW_GAME_UNREGISTERED, this, &AFCWorldNet_ServerModule::OnGameServerUnRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_GTW_GAME_REFRESH, this, &AFCWorldNet_ServerModule::OnRefreshGameServerInfoProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_ACK_ONLINE_NOTIFY, this, &AFCWorldNet_ServerModule::OnOnlineProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_ACK_OFFLINE_NOTIFY, this, &AFCWorldNet_ServerModule::OnOfflineProcess);

    m_pNetModule->AddEventCallBack(this, &AFCWorldNet_ServerModule::OnSocketEvent);

    NF_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetPropertyInt(strConfigName, "Type");
            const int nServerID = m_pElementModule->GetPropertyInt(strConfigName, "ServerID");
            if(nServerType == NF_SERVER_TYPES::NF_ST_WORLD && pPluginManager->AppID() == nServerID)
            {
                const int nPort = m_pElementModule->GetPropertyInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetPropertyInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetPropertyInt(strConfigName, "CpuCount");
                const std::string& strName = m_pElementModule->GetPropertyString(strConfigName, "Name");
                const std::string& strIP = m_pElementModule->GetPropertyString(strConfigName, "IP");

                int nRet = m_pNetModule->Initialization(nMaxConnect, nPort, nCpus, nServerID);
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

bool AFCWorldNet_ServerModule::Shut()
{

    return true;
}

bool AFCWorldNet_ServerModule::Execute()
{
    LogGameServer();

    return m_pNetModule->Execute();
}

void AFCWorldNet_ServerModule::OnGameServerRegisteredProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        NF_SHARE_PTR<ServerData> pServerData =  mGameMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = NF_SHARE_PTR<ServerData>(NF_NEW ServerData());
            mGameMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->nFD = nSockIndex;
        *(pServerData->pData) = xData;

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "GameServerRegistered");
    }

    SynGameToProxy();
}

void AFCWorldNet_ServerModule::OnGameServerUnRegisteredProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        mGameMap.RemoveElement(xData.server_id());

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "GameServerRegistered");
    }
}

void AFCWorldNet_ServerModule::OnRefreshGameServerInfoProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        NF_SHARE_PTR<ServerData> pServerData =  mGameMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = NF_SHARE_PTR<ServerData>(NF_NEW ServerData());
            mGameMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->nFD = nSockIndex;
        *(pServerData->pData) = xData;

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "GameServerRegistered");
    }

    SynGameToProxy();
}

void AFCWorldNet_ServerModule::OnProxyServerRegisteredProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        NF_SHARE_PTR<ServerData> pServerData =  mProxyMap.GetElement(xData.server_id());
        if(!pServerData)
        {
            pServerData = NF_SHARE_PTR<ServerData>(NF_NEW ServerData());
            mProxyMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->nFD = nSockIndex;
        *(pServerData->pData) = xData;

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "Proxy Registered");

        SynGameToProxy(nSockIndex);
    }
}

void AFCWorldNet_ServerModule::OnProxyServerUnRegisteredProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        mGameMap.RemoveElement(xData.server_id());

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "Proxy UnRegistered");
    }
}

void AFCWorldNet_ServerModule::OnRefreshProxyServerInfoProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        NF_SHARE_PTR<ServerData> pServerData =  mProxyMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = NF_SHARE_PTR<ServerData>(NF_NEW ServerData());
            mGameMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->nFD = nSockIndex;
        *(pServerData->pData) = xData;

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "Proxy Registered");

        SynGameToProxy(nSockIndex);
    }
}

int AFCWorldNet_ServerModule::OnLeaveGameProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{

    return 0;
}

void AFCWorldNet_ServerModule::OnSocketEvent(const int nSockIndex, const NF_NET_EVENT eEvent, const AFGUID& xClientID, const int nServerID)
{
    if(eEvent & NF_NET_EVENT_EOF)
    {
        m_pLogModule->LogInfo(AFGUID(0, nSockIndex), "NF_NET_EVENT_EOF", "Connection closed", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex);
    }
    else if(eEvent & NF_NET_EVENT_ERROR)
    {
        m_pLogModule->LogInfo(AFGUID(0, nSockIndex), "NF_NET_EVENT_ERROR", "Got an error on the connection", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex);
    }
    else if(eEvent & NF_NET_EVENT_TIMEOUT)
    {
        m_pLogModule->LogInfo(AFGUID(0, nSockIndex), "NF_NET_EVENT_TIMEOUT", "read timeout", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex);
    }
    else  if(eEvent == NF_NET_EVENT_CONNECTED)
    {
        m_pLogModule->LogInfo(AFGUID(0, nSockIndex), "NF_NET_EVENT_CONNECTED", "connectioned success", __FUNCTION__, __LINE__);
        OnClientConnected(nSockIndex);
    }
}

void AFCWorldNet_ServerModule::SynGameToProxy()
{
    NFMsg::ServerInfoReportList xData;

    NF_SHARE_PTR<ServerData> pServerData =  mProxyMap.First();
    while(nullptr != pServerData)
    {
        SynGameToProxy(pServerData->nFD);

        pServerData = mProxyMap.Next();
    }
}

void AFCWorldNet_ServerModule::SynGameToProxy(const int nFD)
{
    NFMsg::ServerInfoReportList xData;

    NF_SHARE_PTR<ServerData> pServerData =  mGameMap.First();
    while(nullptr != pServerData)
    {
        NFMsg::ServerInfoReport* pData = xData.add_server_list();
        *pData = *(pServerData->pData);

        pServerData = mGameMap.Next();
    }

    m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_STS_NET_INFO, xData, nFD);
}

void AFCWorldNet_ServerModule::OnClientDisconnect(const int nAddress)
{
    //不管是game还是proxy都要找出来,替他反注册
    NF_SHARE_PTR<ServerData> pServerData =  mGameMap.First();
    while(nullptr != pServerData)
    {
        if(nAddress == pServerData->nFD)
        {
            pServerData->pData->set_server_state(NFMsg::EST_CRASH);
            pServerData->nFD = 0;

            SynGameToProxy();
            return;
        }

        pServerData = mGameMap.Next();
    }

    //////////////////////////////////////////////////////////////////////////

    int nServerID = 0;
    pServerData =  mProxyMap.First();
    while(pServerData)
    {
        if(nAddress == pServerData->nFD)
        {
            nServerID = pServerData->pData->server_id();
            break;
        }

        pServerData = mProxyMap.Next();
    }

    mProxyMap.RemoveElement(nServerID);
}

void AFCWorldNet_ServerModule::OnClientConnected(const int nAddress)
{


}

void AFCWorldNet_ServerModule::LogGameServer()
{
    if(mnLastCheckTime + 10 > GetPluginManager()->GetNowTime())
    {
        return;
    }

    mnLastCheckTime = GetPluginManager()->GetNowTime();

    m_pLogModule->LogInfo(AFGUID(), "Begin Log GameServer Info", "");

    NF_SHARE_PTR<ServerData> pGameData = mGameMap.First();
    while(pGameData)
    {
        std::ostringstream stream;
        stream << "Type: " << pGameData->pData->server_type() << " ID: " << pGameData->pData->server_id() << " State: " <<  NFMsg::EServerState_Name(pGameData->pData->server_state()) << " IP: " << pGameData->pData->server_ip() << " FD: " << pGameData->nFD;

        m_pLogModule->LogInfo(AFGUID(), stream);

        pGameData = mGameMap.Next();
    }

    m_pLogModule->LogInfo(AFGUID(), "End Log GameServer Info", "");

    m_pLogModule->LogInfo(AFGUID(), "Begin Log ProxyServer Info", "");

    pGameData = mProxyMap.First();
    while(pGameData)
    {
        std::ostringstream stream;
        stream << "Type: " << pGameData->pData->server_type() << " ID: " << pGameData->pData->server_id() << " State: " <<  NFMsg::EServerState_Name(pGameData->pData->server_state()) << " IP: " << pGameData->pData->server_ip() << " FD: " << pGameData->nFD;

        m_pLogModule->LogInfo(AFGUID(), stream);

        pGameData = mProxyMap.Next();
    }

    m_pLogModule->LogInfo(AFGUID(), "End Log ProxyServer Info", "");
}


void AFCWorldNet_ServerModule::OnOnlineProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    CLIENT_MSG_PROCESS_NO_OBJECT(nSockIndex, nMsgID, msg, nLen, NFMsg::RoleOnlineNotify);

}

void AFCWorldNet_ServerModule::OnOfflineProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    CLIENT_MSG_PROCESS_NO_OBJECT(nSockIndex, nMsgID, msg, nLen, NFMsg::RoleOfflineNotify);
}

bool AFCWorldNet_ServerModule::SendMsgToGame(const int nGameID, const NFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer)
{
    NF_SHARE_PTR<ServerData> pData = mGameMap.GetElement(nGameID);
    if(nullptr != pData)
    {
        const int nFD = pData->nFD;
        m_pNetModule->SendMsgPB(eMsgID, xData, nFD, nPlayer);
    }

    return true;
}

bool AFCWorldNet_ServerModule::SendMsgToGame(const AFIDataList& argObjectVar, const AFIDataList& argGameID, const NFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData)
{
    if(argGameID.GetCount() != argObjectVar.GetCount())
    {
        return false;
    }

    for(int i = 0; i < argObjectVar.GetCount(); i++)
    {
        const AFGUID& identOther = argObjectVar.Object(i);
        const AFINT64  nGameID = argGameID.Int(i);

        SendMsgToGame(nGameID, eMsgID, xData, identOther);
    }

    return true;
}

bool AFCWorldNet_ServerModule::SendMsgToPlayer(const NFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer)
{
    int nGameID = GetPlayerGameID(nPlayer);
    if(nGameID < 0)
    {
        return false;
    }

    return SendMsgToGame(nGameID, eMsgID, xData, nPlayer);
}

int AFCWorldNet_ServerModule::OnObjectListEnter(const AFIDataList& self, const AFIDataList& argVar)
{
    if(self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 0;
    }

    NFMsg::AckPlayerEntryList xPlayerEntryInfoList;
    for(int i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOld = argVar.Object(i);
        //排除空对象
        if(identOld.IsNull())
        {
            continue;
        }

        NFMsg::PlayerEntryInfo* pEntryInfo = xPlayerEntryInfoList.add_object_list();
        *(pEntryInfo->mutable_object_guid()) = AFINetModule::NFToPB(identOld);
        *pEntryInfo->mutable_pos() = AFINetModule::NFToPB(m_pKernelModule->GetPropertyPoint(identOld, "Pos"));
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
        AFGUID ident = self.Object(i);
        if(ident.IsNull())
        {
            continue;
        }

        //可能在不同的网关呢,得到后者所在的网关FD
        SendMsgToPlayer(NFMsg::EGMI_ACK_OBJECT_ENTRY, xPlayerEntryInfoList, ident);
    }

    return 1;
}


int AFCWorldNet_ServerModule::OnObjectListLeave(const AFIDataList& self, const AFIDataList& argVar)
{
    if(self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 0;
    }

    NFMsg::AckPlayerLeaveList xPlayerLeaveInfoList;
    for(int i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOld = argVar.Object(i);
        //排除空对象
        if(identOld.IsNull())
        {
            continue;
        }

        NFMsg::Ident* pIdent = xPlayerLeaveInfoList.add_object_list();
        *pIdent = AFINetModule::NFToPB(argVar.Object(i));
    }

    for(int i = 0; i < self.GetCount(); i++)
    {
        AFGUID ident = self.Object(i);
        if(ident.IsNull())
        {
            continue;
        }
        //可能在不同的网关呢,得到后者所在的网关FD
        SendMsgToPlayer(NFMsg::EGMI_ACK_OBJECT_LEAVE, xPlayerLeaveInfoList, ident);
    }

    return 1;
}


int AFCWorldNet_ServerModule::OnRecordEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self)
{
    if(argVar.GetCount() <= 0 || self.IsNull())
    {
        return 0;
    }

    if(argVar.GetCount() != argGameID.GetCount())
    {
        return 1;
    }

    NFMsg::MultiObjectRecordList xPublicMsg;
    NFMsg::MultiObjectRecordList xPrivateMsg;

    NF_SHARE_PTR<AFIObject> pObject = m_pKernelModule->GetObject(self);
    if(nullptr != pObject)
    {
        NFMsg::ObjectRecordList* pPublicData = NULL;
        NFMsg::ObjectRecordList* pPrivateData = NULL;

        NF_SHARE_PTR<AFIRecordManager> pRecordManager = pObject->GetRecordManager();
        NF_SHARE_PTR<AFIRecord> pRecord = pRecordManager->First();
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
                    *(pPublicData->mutable_player_id()) = AFINetModule::NFToPB(self);
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
                    *(pPrivateData->mutable_player_id()) = AFINetModule::NFToPB(self);
                }
                pPrivateRecordBase = pPrivateData->add_record_list();
                pPrivateRecordBase->set_record_name(pRecord->GetName());

                OnRecordEnterPack(pRecord, pPrivateRecordBase);
            }

            pRecord = pRecordManager->Next();
        }


        for(int i = 0; i < argVar.GetCount(); i++)
        {
            const AFGUID& identOther = argVar.Object(i);
            const AFINT64 nGameID = argGameID.Int(i);
            if(self == identOther)
            {
                if(xPrivateMsg.multi_player_record_size() > 0)
                {
                    SendMsgToGame(nGameID, NFMsg::EGMI_ACK_OBJECT_RECORD_ENTRY, xPrivateMsg, identOther);
                }
            }
            else
            {
                if(xPublicMsg.multi_player_record_size() > 0)
                {
                    SendMsgToGame(nGameID, NFMsg::EGMI_ACK_OBJECT_RECORD_ENTRY, xPublicMsg, identOther);
                }
            }
        }
    }

    return 0;
}

bool AFCWorldNet_ServerModule::OnRecordEnterPack(NF_SHARE_PTR<AFIRecord> pRecord, NFMsg::ObjectRecordBase* pObjectRecordBase)
{
    if(!pRecord || !pObjectRecordBase)
    {
        return false;
    }

    for(int i = 0; i < pRecord->GetRows(); i ++)
    {
        if(pRecord->IsUsed(i))
        {
            //不管public还是private都要加上，不然public广播了那不是private就广播不了了
            NFMsg::RecordAddRowStruct* pAddRowStruct = pObjectRecordBase->add_row_struct();
            pAddRowStruct->set_row(i);

            for(int j = 0; j < pRecord->GetCols(); j++)
            {
                //如果是0就不发送了，因为客户端默认是0
                AFIDataList valueList;
                TDATA_TYPE eType = pRecord->GetColType(j);
                switch(eType)
                {
                case TDATA_INT:
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
                case TDATA_DOUBLE:
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
                case TDATA_STRING:
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
                case TDATA_OBJECT:
                    {
                        AFGUID ident = pRecord->GetObject(i, j);
                        //if ( !ident.IsNull() )
                        {
                            NFMsg::RecordObject* pAddData = pAddRowStruct->add_record_object_list();
                            pAddData->set_row(i);
                            pAddData->set_col(j);
                            *(pAddData->mutable_data()) = AFINetModule::NFToPB(ident);
                        }
                    }
                    break;
                case TDATA_POINT:
                    {
                        const Point3D& xPoint = pRecord->GetPoint(i, j);
                        //if ( !xPoint.IsNull() )
                        {
                            NFMsg::RecordPoint* pAddData = pAddRowStruct->add_record_point_list();
                            pAddData->set_row(i);
                            pAddData->set_col(j);
                            *(pAddData->mutable_data()) = AFINetModule::NFToPB(xPoint);
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

int AFCWorldNet_ServerModule::OnPropertyEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self)
{
    if(argVar.GetCount() <= 0 || self.IsNull())
    {
        return 0;
    }

    if(argVar.GetCount() != argGameID.GetCount())
    {
        return 1;
    }

    NFMsg::MultiObjectPropertyList xPublicMsg;
    NFMsg::MultiObjectPropertyList xPrivateMsg;

    //分为自己和外人
    //1.public发送给所有人
    //2.如果自己在列表中，再次发送private数据
    NF_SHARE_PTR<AFIObject> pObject = m_pKernelModule->GetObject(self);
    if(nullptr != pObject)
    {
        NFMsg::ObjectPropertyList* pPublicData = xPublicMsg.add_multi_player_property();
        NFMsg::ObjectPropertyList* pPrivateData = xPrivateMsg.add_multi_player_property();

        *(pPublicData->mutable_player_id()) = AFINetModule::NFToPB(self);
        *(pPrivateData->mutable_player_id()) = AFINetModule::NFToPB(self);

        NF_SHARE_PTR<AFIPropertyManager> pPropertyManager = pObject->GetPropertyManager();
        NF_SHARE_PTR<AFIProperty> pPropertyInfo = pPropertyManager->First();
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
                            *(pDataObject->mutable_data()) = AFINetModule::NFToPB(pPropertyInfo->GetObject());
                        }

                        if(pPropertyInfo->GetPrivate())
                        {
                            NFMsg::PropertyObject* pDataObject = pPrivateData->add_property_object_list();
                            pDataObject->set_property_name(pPropertyInfo->GetKey());
                            *(pDataObject->mutable_data()) = AFINetModule::NFToPB(pPropertyInfo->GetObject());
                        }
                    }
                    break;
                case TDATA_POINT:
                    {
                        if(pPropertyInfo->GetPublic())
                        {
                            NFMsg::PropertyPoint* pDataPoint = pPublicData->add_property_point_list();
                            pDataPoint->set_property_name(pPropertyInfo->GetKey());
                            *(pDataPoint->mutable_data()) = AFINetModule::NFToPB(pPropertyInfo->GetPoint());
                        }

                        if(pPropertyInfo->GetPrivate())
                        {
                            NFMsg::PropertyPoint* pDataPoint = pPrivateData->add_property_point_list();
                            pDataPoint->set_property_name(pPropertyInfo->GetKey());
                            *(pDataPoint->mutable_data()) = AFINetModule::NFToPB(pPropertyInfo->GetPoint());
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
            const AFGUID& identOther = argVar.Object(i);
            const AFINT64 nGameID = argGameID.Int(i);
            if(self == identOther)
            {
                //找到他所在网关的FD
                SendMsgToGame(nGameID, NFMsg::EGMI_ACK_OBJECT_PROPERTY_ENTRY, xPrivateMsg, identOther);
            }
            else
            {
                SendMsgToGame(nGameID, NFMsg::EGMI_ACK_OBJECT_PROPERTY_ENTRY, xPublicMsg, identOther);
            }

        }
    }

    return 0;
}

NF_SHARE_PTR<ServerData> AFCWorldNet_ServerModule::GetSuitProxyForEnter()
{
    return mProxyMap.First();
}

AFINetModule* AFCWorldNet_ServerModule::GetNetModule()
{
    return m_pNetModule;
}

int AFCWorldNet_ServerModule::GetPlayerGameID(const AFGUID self)
{
    //to do
    return -1;
}
