// -------------------------------------------------------------------------
//    @FileName         :    AFCLoginNet_ServerModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCLoginNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#include "AFCMasterNet_ServerModule.h"
#include "AFMasterNet_ServerPlugin.h"

bool AFCMasterNet_ServerModule::Init()
{
    m_pNetModule = NF_NEW AFINetServerModule(pPluginManager);
    return true;
}

bool AFCMasterNet_ServerModule::Shut()
{
    return true;
}

void AFCMasterNet_ServerModule::OnWorldRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        NF_SHARE_PTR<ServerData> pServerData =  mWorldMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = NF_SHARE_PTR<ServerData>(NF_NEW ServerData());
            mWorldMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xClient = xClientID;
        *(pServerData->pData) = xData;

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "WorldRegistered");
    }

    SynWorldToLogin();
}

void AFCMasterNet_ServerModule::OnWorldUnRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        mWorldMap.RemoveElement(xData.server_id());


        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "WorldUnRegistered");
    }

    SynWorldToLogin();
}

void AFCMasterNet_ServerModule::OnRefreshWorldInfoProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        NF_SHARE_PTR<ServerData> pServerData =  mWorldMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = NF_SHARE_PTR<ServerData>(NF_NEW ServerData());
            mWorldMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xClient = xClientID;
        *(pServerData->pData) = xData;

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "RefreshWorldInfo");

    }

    SynWorldToLogin();
}

void AFCMasterNet_ServerModule::OnLoginRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        NF_SHARE_PTR<ServerData> pServerData =  mLoginMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = NF_SHARE_PTR<ServerData>(NF_NEW ServerData());
            mLoginMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xClient = xClientID;
        *(pServerData->pData) = xData;

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "LoginRegistered");
    }

    SynWorldToLogin();
}

void AFCMasterNet_ServerModule::OnLoginUnRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        mLoginMap.RemoveElement(xData.server_id());

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "LoginUnRegistered");

    }
}

void AFCMasterNet_ServerModule::OnRefreshLoginInfoProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        NF_SHARE_PTR<ServerData> pServerData =  mLoginMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = NF_SHARE_PTR<ServerData>(NF_NEW ServerData());
            mLoginMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xClient = xClientID;
        *(pServerData->pData) = xData;

        m_pLogModule->LogInfo(AFGUID(0, xData.server_id()), xData.server_name(), "RefreshLoginInfo");

    }
}

void AFCMasterNet_ServerModule::OnSelectWorldProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ReqConnectWorld xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    NF_SHARE_PTR<ServerData> pServerData =  mWorldMap.GetElement(xMsg.world_id());
    if(nullptr == pServerData)
    {
        return;
    }

    //转发送到世界服务器
    m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_REQ_CONNECT_WORLD, xMsg, pServerData->xClient, nPlayerID);
}

bool AFCMasterNet_ServerModule::Execute()
{
    LogGameServer();

    m_pNetModule->Execute();
    return true;
}

void AFCMasterNet_ServerModule::OnSelectServerResultProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::AckConnectWorldResult xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    NF_SHARE_PTR<ServerData> pServerData =  mLoginMap.GetElement(xMsg.login_id());
    if(nullptr == pServerData)
    {
        return;
    }

    //转发送到登录服务器
    m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_CONNECT_WORLD, xMsg, pServerData->xClient, nPlayerID);
}

bool AFCMasterNet_ServerModule::AfterInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();

    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_STS_HEART_BEAT, this, &AFCMasterNet_ServerModule::OnHeartBeat);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_MTL_WORLD_REGISTERED, this, &AFCMasterNet_ServerModule::OnWorldRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_MTL_WORLD_UNREGISTERED, this, &AFCMasterNet_ServerModule::OnWorldUnRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_MTL_WORLD_REFRESH, this, &AFCMasterNet_ServerModule::OnRefreshWorldInfoProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_LTM_LOGIN_REGISTERED, this, &AFCMasterNet_ServerModule::OnLoginRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_LTM_LOGIN_UNREGISTERED, this, &AFCMasterNet_ServerModule::OnLoginUnRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_LTM_LOGIN_REFRESH, this, &AFCMasterNet_ServerModule::OnRefreshLoginInfoProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_CONNECT_WORLD, this, &AFCMasterNet_ServerModule::OnSelectWorldProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_ACK_CONNECT_WORLD, this, &AFCMasterNet_ServerModule::OnSelectServerResultProcess);
    m_pNetModule->AddReceiveCallBack(this, &AFCMasterNet_ServerModule::InvalidMessage);

    m_pNetModule->AddEventCallBack(this, &AFCMasterNet_ServerModule::OnSocketEvent);

    NF_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetPropertyInt(strConfigName, "Type");
            const int nServerID = m_pElementModule->GetPropertyInt(strConfigName, "ServerID");
            if(nServerType == NF_SERVER_TYPES::NF_ST_MASTER && pPluginManager->AppID() == nServerID)
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

void AFCMasterNet_ServerModule::OnSocketEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    //std::cout << "OnSocketEvent::thread id=" << GetCurrentThreadId() << std::endl;

    if(eEvent == DISCONNECTED)
    {
        m_pLogModule->LogInfo(xClientID, "NF_NET_EVENT_EOF", "Connection closed", __FUNCTION__, __LINE__);
        OnClientDisconnect(xClientID);
    }
    else  if(eEvent == CONNECTED)
    {
        m_pLogModule->LogInfo(xClientID, "NF_NET_EVENT_CONNECTED", "connectioned success", __FUNCTION__, __LINE__);
        OnClientConnected(xClientID);
    }
}

void AFCMasterNet_ServerModule::OnClientDisconnect(const AFGUID& xClientID)
{
    //不管是login还是world都要找出来,替他反注册
    NF_SHARE_PTR<ServerData> pServerData =  mWorldMap.First();
    while(nullptr != pServerData)
    {
        if(xClientID == pServerData->xClient)
        {
            pServerData->pData->set_server_state(NFMsg::EST_CRASH);
            pServerData->xClient = AFGUID(0);

            SynWorldToLogin();
            return;
        }

        pServerData = mWorldMap.Next();
    }

    //////////////////////////////////////////////////////////////////////////

    int nServerID = 0;
    pServerData =  mLoginMap.First();
    while(nullptr != pServerData)
    {
        if(xClientID == pServerData->xClient)
        {
            nServerID = pServerData->pData->server_id();
            break;
        }

        pServerData = mLoginMap.Next();
    }

    mLoginMap.RemoveElement(nServerID);

}

void AFCMasterNet_ServerModule::OnClientConnected(const AFGUID& xClientID)
{
    //连接上来啥都不做
}

void AFCMasterNet_ServerModule::SynWorldToLogin()
{
    NFMsg::ServerInfoReportList xData;

    NF_SHARE_PTR<ServerData> pServerData =  mWorldMap.First();
    while(nullptr != pServerData)
    {
        NFMsg::ServerInfoReport* pData = xData.add_server_list();
        *pData = *(pServerData->pData);

        pServerData = mWorldMap.Next();
    }

    //广播给所有loginserver
    pServerData =  mLoginMap.First();
    while(nullptr != pServerData)
    {
        m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_STS_NET_INFO, xData, pServerData->xClient, AFGUID(0));

        pServerData = mLoginMap.Next();
    }
}

void AFCMasterNet_ServerModule::LogGameServer()
{
    if(mnLastLogTime + 10 > GetPluginManager()->GetNowTime())
    {
        return;
    }

    mnLastLogTime = GetPluginManager()->GetNowTime();

    //////////////////////////////////////////////////////////////////////////

    m_pLogModule->LogInfo(AFGUID(), "Begin Log WorldServer Info", "");

    NF_SHARE_PTR<ServerData> pGameData = mWorldMap.First();
    while(pGameData)
    {
        std::ostringstream stream;
        stream << "Type: " << pGameData->pData->server_type() << " ID: " << pGameData->pData->server_id() << " State: " <<  NFMsg::EServerState_Name(pGameData->pData->server_state()) << " IP: " << pGameData->pData->server_ip() << " xClient: " << pGameData->xClient.n64Value;
        m_pLogModule->LogInfo(AFGUID(), stream);

        pGameData = mWorldMap.Next();
    }

    m_pLogModule->LogInfo(AFGUID(), "End Log WorldServer Info", "");

    m_pLogModule->LogInfo(AFGUID(), "Begin Log LoginServer Info", "");

    //////////////////////////////////////////////////////////////////////////
    pGameData = mLoginMap.First();
    while(pGameData)
    {
        std::ostringstream stream;
        stream << "Type: " << pGameData->pData->server_type() << " ID: " << pGameData->pData->server_id() << " State: " <<  NFMsg::EServerState_Name(pGameData->pData->server_state()) << " IP: " << pGameData->pData->server_ip() << " xClient: " << pGameData->xClient.n64Value;
        m_pLogModule->LogInfo(AFGUID(), stream);

        pGameData = mLoginMap.Next();
    }

    m_pLogModule->LogInfo(AFGUID(), "End Log LoginServer Info", "");

}

void AFCMasterNet_ServerModule::OnHeartBeat(const int nMsgID, const char * msg, const uint32_t nLen, const AFGUID& xClientID)
{
}

void AFCMasterNet_ServerModule::InvalidMessage(const int nMsgID, const char * msg, const uint32_t nLen, const AFGUID& xClientID)
{
    printf("NFNet || 非法消息:unMsgID=%d\n", nMsgID);
}
