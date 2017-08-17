// -------------------------------------------------------------------------
//    @FileName         :    AFCLoginNet_ServerModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCLoginNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#include "AFCLoginNet_ServerModule.h"
#include "AFLoginNetServerPlugin.h"

const std::string PROPERTY_ACCOUNT = "Account";
const std::string PROPERTY_VERIFIED = "Verified";

bool AFCLoginNet_ServerModule::Init()
{
    m_pNetModule = ARK_NEW AFINetServerModule(pPluginManager);
    return true;
}

bool AFCLoginNet_ServerModule::Shut()
{
    return true;
}

bool AFCLoginNet_ServerModule::BeforeShut()
{
    return true;
}

bool AFCLoginNet_ServerModule::AfterInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pLoginLogicModule = pPluginManager->FindModule<AFILoginLogicModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pLoginToMasterModule = pPluginManager->FindModule<AFILoginToMasterModule>();
    m_pUUIDModule = pPluginManager->FindModule<AFIGUIDModule>();


    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_STS_HEART_BEAT, this, &AFCLoginNet_ServerModule::OnHeartBeat);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_LOGIN, this, &AFCLoginNet_ServerModule::OnLoginProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_LOGOUT, this, &AFCLoginNet_ServerModule::OnLogOut);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_CONNECT_WORLD, this, &AFCLoginNet_ServerModule::OnSelectWorldProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_WORLD_LIST, this, &AFCLoginNet_ServerModule::OnViewWorldProcess);
    m_pNetModule->AddReceiveCallBack(this, &AFCLoginNet_ServerModule::InvalidMessage);

    m_pNetModule->AddEventCallBack(this, &AFCLoginNet_ServerModule::OnSocketClientEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetPropertyInt(strConfigName, "Type");
            const int nServerID = m_pElementModule->GetPropertyInt(strConfigName, "ServerID");
            if(nServerType == NF_SERVER_TYPES::NF_ST_LOGIN && pPluginManager->AppID() == nServerID)
            {
                const int nPort = m_pElementModule->GetPropertyInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetPropertyInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetPropertyInt(strConfigName, "CpuCount");

                m_pUUIDModule->SetWorkerAndDatacenter(nServerID, nServerID);

                int nRet = m_pNetModule->Initialization(nMaxConnect, nPort, nCpus, nServerID);
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

int AFCLoginNet_ServerModule::OnSelectWorldResultsProcess(const int nWorldID, const AFGUID xSenderID, const int nLoginID, const std::string& strAccount, const std::string& strWorldIP, const int nWorldPort, const std::string& strWorldKey)
{
    ARK_SHARE_PTR<SessionData> pSessionData = mmClientSessionData.GetElement(xSenderID);
    if(pSessionData)
    {
        NFMsg::AckConnectWorldResult xMsg;
        xMsg.set_world_id(nWorldID);
        xMsg.mutable_sender()->CopyFrom(AFINetServerModule::NFToPB(xSenderID));
        xMsg.set_login_id(nLoginID);
        xMsg.set_account(strAccount);
        xMsg.set_world_ip(strWorldIP);
        xMsg.set_world_port(nWorldPort);
        xMsg.set_world_key(strWorldKey);

        m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_CONNECT_WORLD, xMsg, pSessionData->mnClientID, xSenderID);
    }

    return 0;
}

bool AFCLoginNet_ServerModule::Execute()
{
    return m_pNetModule->Execute();
}

void AFCLoginNet_ServerModule::OnClientConnected(const AFGUID& xClientID)
{
    ARK_SHARE_PTR<SessionData> pSessionData(ARK_NEW SessionData());

    pSessionData->mnClientID = xClientID;
    mmClientSessionData.AddElement(xClientID, pSessionData);
}

void AFCLoginNet_ServerModule::OnClientDisconnect(const AFGUID& xClientID)
{
    mmClientSessionData.RemoveElement(xClientID);
}

void AFCLoginNet_ServerModule::OnLoginProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ReqAccountLogin xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    ARK_SHARE_PTR<SessionData> pSession = mmClientSessionData.GetElement(xClientID);
    if(pSession)
    {
        //还没有登录过
        if(pSession->mnLogicState == 0)
        {
            int nState = m_pLoginLogicModule->OnLoginProcess(pSession->mnClientID, xMsg.account(), xMsg.password());
            if(0 != nState)
            {
                std::ostringstream strLog;
                strLog << "Check password failed, Account = " << xMsg.account() << " Password = " << xMsg.password();
                m_pLogModule->LogError(xClientID, strLog, __FUNCTION__, __LINE__);

                NFMsg::AckEventResult xMsg;
                xMsg.set_event_code(NFMsg::EGEC_ACCOUNTPWD_INVALID);

                m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_LOGIN, xMsg, xClientID, nPlayerID);
                return;
            }

            pSession->mnLogicState = 1;
            pSession->mstrAccout = xMsg.account();

            NFMsg::AckEventResult xData;
            xData.set_event_code(NFMsg::EGEC_ACCOUNT_SUCCESS);

            m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_LOGIN, xData, xClientID, nPlayerID);

            m_pLogModule->LogInfo(xClientID, "Login successed :", xMsg.account().c_str());
        }
    }
}

void AFCLoginNet_ServerModule::OnSelectWorldProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ReqConnectWorld xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    ARK_SHARE_PTR<SessionData> pSession = mmClientSessionData.GetElement(xClientID);
    if(!pSession)
    {
        return;
    }

    //没登录过
    if(pSession->mnLogicState <= 0)
    {
        return;
    }

    NFMsg::ReqConnectWorld xData;
    xData.set_world_id(xMsg.world_id());
    xData.set_login_id(pPluginManager->AppID());
    xData.mutable_sender()->CopyFrom(AFINetServerModule::NFToPB(pSession->mnClientID));
    xData.set_account(pSession->mstrAccout);

    m_pLoginToMasterModule->GetClusterModule()->SendSuitByPB(pSession->mstrAccout, NFMsg::EGameMsgID::EGMI_REQ_CONNECT_WORLD, xData);//here has a problem to be solve
}

void AFCLoginNet_ServerModule::OnSocketClientEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
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

void AFCLoginNet_ServerModule::SynWorldToClient(const AFGUID& xClientID)
{
    NFMsg::AckServerList xData;
    xData.set_type(NFMsg::RSLT_WORLD_SERVER);

    AFMapEx<int, NFMsg::ServerInfoReport>& xWorldMap = m_pLoginToMasterModule->GetWorldMap();
    NFMsg::ServerInfoReport* pWorldData = xWorldMap.FirstNude();
    while(pWorldData)
    {
        NFMsg::ServerInfo* pServerInfo = xData.add_info();

        pServerInfo->set_name(pWorldData->server_name());
        pServerInfo->set_status(pWorldData->server_state());
        pServerInfo->set_server_id(pWorldData->server_id());
        pServerInfo->set_wait_count(0);

        pWorldData = xWorldMap.NextNude();
    }


    m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_WORLD_LIST, xData, xClientID, AFGUID(0));
}

void AFCLoginNet_ServerModule::OnViewWorldProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ReqServerList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    if(xMsg.type() == NFMsg::RSLT_WORLD_SERVER)
    {
        SynWorldToClient(xClientID);
    }
}

void AFCLoginNet_ServerModule::OnHeartBeat(const int nMsgID, const char * msg, const uint32_t nLen, const AFGUID& xClientID)
{
}

void AFCLoginNet_ServerModule::OnLogOut(const int nMsgID, const char * msg, const uint32_t nLen, const AFGUID& xClientID)
{
}

void AFCLoginNet_ServerModule::InvalidMessage(const int nMsgID, const char * msg, const uint32_t nLen, const AFGUID& xClientID)
{
    printf("NFNet || 非法消息:unMsgID=%d\n", nMsgID);
}


