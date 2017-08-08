// -------------------------------------------------------------------------
//    @FileName         :    AFCProxyServerNet_ClientModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    AFCProxyServerNet_ClientModule
//
// -------------------------------------------------------------------------

#include "AFCProxyServerToWorldModule.h"
#include "AFProxyServerNet_ClientPlugin.h"
#include "SDK/Core/AFIHeartBeatManager.h"
#include "SDK/Core/AFCHeartBeatManager.h"
#include "SDK/Interface/AFIClassModule.h"

bool AFCProxyServerToWorldModule::Init()
{
    m_pNetClientModule = NF_NEW AFINetClientModule(pPluginManager);

    m_pNetClientModule->Init();

    return true;
}

bool AFCProxyServerToWorldModule::Shut()
{
    //Final();
    //Clear();
    return true;
}

bool AFCProxyServerToWorldModule::Execute()
{
    m_pNetClientModule->Execute();

    return true;
}

void AFCProxyServerToWorldModule::OnServerInfoProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    NFMsg::ServerInfoReportList xMsg;
    if(!AFINetServerModule::ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const NFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        //type
        ConnectData xServerData;

        xServerData.nGameID = xData.server_id();
        xServerData.strIP = xData.server_ip();
        xServerData.nPort = xData.server_port();
        xServerData.strName = xData.server_name();
        //xServerData.eState = pData->server_state();
        xServerData.eServerType = (NF_SERVER_TYPES)xData.server_type();

        switch(xServerData.eServerType)
        {
        case NF_SERVER_TYPES::NF_ST_GAME:
            {
                m_pProxyServerToGameModule->GetClusterModule()->AddServer(xServerData);
            }
            break;
        case NF_SERVER_TYPES::NF_ST_WORLD:
            {
                m_pNetClientModule->AddServer(xServerData);
            }
            break;
        default:
            break;
        }

    }
}

void AFCProxyServerToWorldModule::OnSocketWSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if(eEvent == DISCONNECTED)
    {
        m_pLogModule->LogInfo(xClientID, "NF_NET_EVENT_EOF", "Connection closed", __FUNCTION__, __LINE__);
    }
    else  if(eEvent == CONNECTED)
    {
        m_pLogModule->LogInfo(xClientID, "NF_NET_EVENT_CONNECTED", "connectioned success", __FUNCTION__, __LINE__);
        Register(nServerID);
    }
}

void AFCProxyServerToWorldModule::Register(const int nServerID)
{
    NF_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetPropertyInt(strConfigName, "Type");
            const int nSelfServerID = m_pElementModule->GetPropertyInt(strConfigName, "ServerID");
            if(nServerType == NF_SERVER_TYPES::NF_ST_PROXY && pPluginManager->AppID() == nSelfServerID)
            {
                const int nPort = m_pElementModule->GetPropertyInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetPropertyInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetPropertyInt(strConfigName, "CpuCount");
                const std::string& strName = m_pElementModule->GetPropertyString(strConfigName, "Name");
                const std::string& strIP = m_pElementModule->GetPropertyString(strConfigName, "IP");

                NFMsg::ServerInfoReportList xMsg;
                NFMsg::ServerInfoReport* pData = xMsg.add_server_list();

                pData->set_server_id(nSelfServerID);
                pData->set_server_name(strName);
                pData->set_server_cur_count(0);
                pData->set_server_ip(strIP);
                pData->set_server_port(nPort);
                pData->set_server_max_online(nMaxConnect);
                pData->set_server_state(NFMsg::EST_NARMAL);
                pData->set_server_type(nServerType);

                NF_SHARE_PTR<ConnectData> pServerData = GetClusterModule()->GetServerNetInfo(nServerID);
                if(pServerData)
                {
                    int nTargetID = pServerData->nGameID;
                    GetClusterModule()->SendToServerByPB(nTargetID, NFMsg::EGameMsgID::EGMI_PTWG_PROXY_REGISTERED, xMsg);

                    m_pLogModule->LogInfo(AFGUID(0, pData->server_id()), pData->server_name(), "Register");
                }
            }
        }
    }
}

bool AFCProxyServerToWorldModule::AfterInit()
{
    m_pProxyLogicModule = pPluginManager->FindModule<AFIProxyLogicModule>();
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pProxyServerNet_ServerModule = pPluginManager->FindModule<AFIProxyServerNet_ServerModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pProxyServerToGameModule = pPluginManager->FindModule<AFIProxyServerToGameModule>();

    m_pNetClientModule->AddReceiveCallBack(NFMsg::EGMI_ACK_CONNECT_WORLD, this, &AFCProxyServerToWorldModule::OnSelectServerResultProcess);
    m_pNetClientModule->AddReceiveCallBack(NFMsg::EGMI_STS_NET_INFO, this, &AFCProxyServerToWorldModule::OnServerInfoProcess);
    m_pNetClientModule->AddReceiveCallBack(this, &AFCProxyServerToWorldModule::OnOtherMessage);

    m_pNetClientModule->AddEventCallBack(this, &AFCProxyServerToWorldModule::OnSocketWSEvent);

    NF_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetPropertyInt(strConfigName, "Type");
            const int nServerID = m_pElementModule->GetPropertyInt(strConfigName, "ServerID");
            if(nServerType == NF_SERVER_TYPES::NF_ST_WORLD)
            {
                const int nPort = m_pElementModule->GetPropertyInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetPropertyInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetPropertyInt(strConfigName, "CpuCount");
                const std::string& strName = m_pElementModule->GetPropertyString(strConfigName, "Name");
                const std::string& strIP = m_pElementModule->GetPropertyString(strConfigName, "IP");

                ConnectData xServerData;

                xServerData.nGameID = nServerID;
                xServerData.eServerType = (NF_SERVER_TYPES)nServerType;
                xServerData.strIP = strIP;
                xServerData.nPort = nPort;
                xServerData.strName = strName;

                m_pNetClientModule->AddServer(xServerData);
            }
        }
    }

    return true;
}


void AFCProxyServerToWorldModule::OnSelectServerResultProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //保持记录,直到下线,或者1分钟不上线即可删除
    AFGUID nPlayerID;
    NFMsg::AckConnectWorldResult xMsg;
    if(!AFINetServerModule::ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    NF_SHARE_PTR<ClientConnectData> pConnectData = mWantToConnectMap.GetElement(xMsg.account());
    if(nullptr != pConnectData)
    {
        pConnectData->strConnectKey = xMsg.world_key();
        return;
    }

    pConnectData = NF_SHARE_PTR<ClientConnectData>(NF_NEW ClientConnectData());
    pConnectData->strAccount = xMsg.account();
    pConnectData->strConnectKey = xMsg.world_key();
    mWantToConnectMap.AddElement(pConnectData->strAccount, pConnectData);
}

AFINetClientModule* AFCProxyServerToWorldModule::GetClusterModule()
{
    return m_pNetClientModule;
}

bool AFCProxyServerToWorldModule::VerifyConnectData(const std::string& strAccount, const std::string& strKey)
{
    NF_SHARE_PTR<ClientConnectData> pConnectData = mWantToConnectMap.GetElement(strAccount);
    if(pConnectData && strKey == pConnectData->strConnectKey)
    {
        mWantToConnectMap.RemoveElement(strAccount);

        return true;
    }

    return false;
}

void AFCProxyServerToWorldModule::LogServerInfo(const std::string& strServerInfo)
{
    m_pLogModule->LogInfo(AFGUID(), strServerInfo, "");
}

void AFCProxyServerToWorldModule::OnOtherMessage(const int nMsgID, const char * msg, const uint32_t nLen, const AFGUID& xClientID)
{
    m_pProxyServerNet_ServerModule->Transpond(nMsgID, msg, nLen, xClientID);
}
