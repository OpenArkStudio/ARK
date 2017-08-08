// -------------------------------------------------------------------------
//    @FileName         ：    AFINetClientModule.hpp
//    @Author           ：    LvSheng.Huang
//    @Date             ：    2015-01-4
//    @Module           ：    AFINetClientModule
//
// -------------------------------------------------------------------------

#ifndef AFI_CLUSTER_CLIENT_MODULE_H
#define AFI_CLUSTER_CLIENT_MODULE_H

#include <iostream>
#include "AFIModule.h"
#include "SDK/Core/AFCConsistentHash.hpp"
#include "SDK/NetEvpp/AFCNetClient.h"
#include "AFINetModule.h"

enum ConnectDataState
{
    DISCONNECT,
    CONNECTING,
    NORMAL,
    RECONNECT,
};
struct ConnectData
{
    ConnectData()
    {
        nGameID = 0;
        nPort = 0;
        strName = "";
        strIP = "";
        eServerType = NF_ST_NONE;
        eState = ConnectDataState::DISCONNECT;
        mnLastActionTime = 0;
    }

    int nGameID;
    NF_SERVER_TYPES eServerType;
    std::string strIP;
    int nPort;
    std::string strIPAndPort;
    std::string strName;
    ConnectDataState eState;
    AFINT64 mnLastActionTime;

    NF_SHARE_PTR<AFCNetClient> mxNetModule;
};

class AFINetClientModule : public AFINetModule
{

protected:
    AFINetClientModule()
    {
    }
public:
    enum EConstDefine
    {
        EConstDefine_DefaultWeith = 500,
    };

    AFINetClientModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init()
    {
        return true;
    }

    virtual bool AfterInit()
    {
        return true;
    }

    virtual bool BeforeShut()
    {
        return true;
    }

    virtual bool Shut()
    {
        return true;
    }

    virtual bool Execute()
    {
        ProcessExecute();
        ProcessAddNetConnect();

        return true;
    }

    void AddServer(const ConnectData& xInfo)
    {
        mxTempNetList.push_back(xInfo);
    }

    ////////////////////////////////////////////////////////////////////////////////
    //裸数据,发时组包
    void SendByServerID(const int nServerID, const int nMsgID, const std::string& strData)
    {
        SendByServerID(nServerID, nMsgID, strData.c_str(), strData.length());
    }

    //裸数据,发时组包
    void SendByServerID(const int nServerID, const int nMsgID, const char* msg, const uint32_t nLen)
    {
        NF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
        if(pServer)
        {
            NF_SHARE_PTR<AFCNetClient> pNetModule = pServer->mxNetModule;
            if(pNetModule.get())
            {
                pNetModule->SendMsgWithOutHead(nMsgID, msg, nLen, 0);
            }
        }
    }
    //裸数据,发时组包
    void SendToAllServer(const int nMsgID, const std::string& strData)
    {
        NF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
        while(pServer)
        {
            NF_SHARE_PTR<AFINet> pNetModule = pServer->mxNetModule;
            if(pNetModule.get())
            {
                pNetModule->SendMsgWithOutHead(nMsgID, strData.data(), strData.size(), 0);
            }

            pServer = mxServerMap.Next();
        }
    }
    void SendToServerByPB(const int nServerID, const uint16_t nMsgID, google::protobuf::Message& xData)
    {
        std::string strData;
        PackMsgToBasePB(xData, AFGUID(0), strData);

        NF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
        if(pServer)
        {
            NF_SHARE_PTR<AFINet> pNetModule = pServer->mxNetModule;
            if(pNetModule.get())
            {
                pNetModule->SendMsgWithOutHead(nMsgID, strData.data(), strData.length(), AFGUID(0));
            }
        }
    }

    void SendToAllServerByPB(const uint16_t nMsgID, google::protobuf::Message& xData)
    {
        std::string strData;
        PackMsgToBasePB(xData, AFGUID(0), strData);

        NF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
        while(pServer)
        {
            NF_SHARE_PTR<AFINet> pNetModule = pServer->mxNetModule;
            if(pNetModule.get())
            {
                pNetModule->SendMsgWithOutHead(nMsgID, strData.data(), strData.length(), AFGUID(0));
            }

            pServer = mxServerMap.Next();
        }
    }

    void SendBySuit(const std::string& strHashKey, const int nMsgID, const std::string& strData)
    {
        uint32_t nCRC32 = NFrame::CRC32(strHashKey);
        SendBySuit(nCRC32, nMsgID, strData);
    }

    void SendBySuit(const std::string& strHashKey, const int nMsgID, const char* msg, const uint32_t nLen)
    {
        uint32_t nCRC32 = NFrame::CRC32(strHashKey);
        SendBySuit(nCRC32, nMsgID, msg, nLen);
    }

    void SendBySuit(const int& nHashKey, const int nMsgID, const std::string& strData)
    {
        SendBySuit(nHashKey, nMsgID, strData.c_str(), strData.length());
    }

    void SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen)
    {
        if(mxConsistentHash.Size() <= 0)
        {
            return;
        }

        AFCMachineNode xNode;
        if(!GetServerMachineData(AF_LEXICAL_CAST<std::string>(nHashKey), xNode))
        {
            return;
        }

        SendByServerID(xNode.nMachineID, nMsgID, msg, nLen);
    }

    void SendSuitByPB(const std::string& strHashKey, const uint16_t nMsgID, google::protobuf::Message& xData)
    {
        uint32_t nCRC32 = NFrame::CRC32(strHashKey);
        SendSuitByPB(nCRC32, nMsgID, xData);
    }

    void SendSuitByPB(const int& nHashKey, const uint16_t nMsgID, google::protobuf::Message& xData)
    {
        if(mxConsistentHash.Size() <= 0)
        {
            return;
        }

        AFCMachineNode xNode;
        if(!GetServerMachineData(AF_LEXICAL_CAST<std::string> (nHashKey), xNode))
        {
            return;
        }

        SendToServerByPB(xNode.nMachineID, nMsgID, xData);
    }

    NF_SHARE_PTR<ConnectData> GetServerNetInfo(const int nServerID)
    {
        return mxServerMap.GetElement(nServerID);
    }

    AFMapEx<int, ConnectData>& GetServerList()
    {
        return mxServerMap;
    }

protected:
    bool PackMsgToBasePB(const google::protobuf::Message& xData, const AFGUID nPlayer, std::string& outData)
    {
        std::string xMsgData;
        if(!xData.SerializeToString(&xMsgData))
        {
            return false;
        }

        PackMsgToBasePB(xMsgData, nPlayer, outData);
    }

    bool PackMsgToBasePB(const std::string& strData, const AFGUID nPlayer, std::string& outData)
    {
        NFMsg::MsgBase xMsg;
        xMsg.set_msg_data(strData.data(), strData.length());

        NFMsg::Ident* pPlayerID = xMsg.mutable_player_id();
        *pPlayerID = NFToPB(nPlayer);

        std::string strMsg;
        if(!xMsg.SerializeToString(&strMsg))
        {
            return false;
        }

        return true;
    }
protected:
    void ProcessExecute()
    {
        ConnectData* pServerData = mxServerMap.FirstNude();
        while(pServerData)
        {
            switch(pServerData->eState)
            {
            case ConnectDataState::DISCONNECT:
                {
                    if(NULL != pServerData->mxNetModule)
                    {
                        pServerData->mxNetModule = nullptr;
                        pServerData->eState = ConnectDataState::RECONNECT;
                    }
                }
                break;
            case ConnectDataState::CONNECTING:
                {
                    if(pServerData->mxNetModule)
                    {
                        pServerData->mxNetModule->Execute();
                    }
                }
                break;
            case ConnectDataState::NORMAL:
                {
                    if(pServerData->mxNetModule)
                    {
                        pServerData->mxNetModule->Execute();

                        KeepState(pServerData);
                    }
                }
                break;
            case ConnectDataState::RECONNECT:
                {
                    //计算时间
                    if((pServerData->mnLastActionTime + 30) >= GetPluginManager()->GetNowTime())
                    {
                        break;
                    }

                    if(nullptr != pServerData->mxNetModule)
                    {
                        pServerData->mxNetModule = nullptr;
                    }

                    pServerData->eState = ConnectDataState::CONNECTING;
                    pServerData->mxNetModule = NF_SHARE_PTR<AFCNetClient>(new AFCNetClient(this, &AFINetClientModule::OnReceiveNetPack, &AFINetClientModule::OnSocketNetEvent));
                    pServerData->mxNetModule->Initialization(pServerData->strIPAndPort, pServerData->nGameID);
                }
                break;
            default:
                break;
            }

            pServerData = mxServerMap.NextNude();
        }
    }

    void KeepReport(ConnectData* pServerData) {};
    void LogServerInfo(const std::string& strServerInfo) {};


private:
    virtual void LogServerInfo()
    {
        LogServerInfo("This is a client, begin to print Server Info----------------------------------");

        ConnectData* pServerData = mxServerMap.FirstNude();
        while(nullptr != pServerData)
        {
            std::ostringstream stream;
            stream << "Type: " << pServerData->eServerType << " ProxyServer ID: " << pServerData->nGameID << " State: " << pServerData->eState << " IP: " << pServerData->strIP << " Port: " << pServerData->nPort;

            LogServerInfo(stream.str());

            pServerData = mxServerMap.NextNude();
        }

        LogServerInfo("This is a client, end to print Server Info----------------------------------");
    };

    void KeepState(ConnectData* pServerData)
    {
        if(pServerData->mnLastActionTime + 10 > GetPluginManager()->GetNowTime())
        {
            return;
        }

        pServerData->mnLastActionTime = GetPluginManager()->GetNowTime();

        KeepReport(pServerData);
        LogServerInfo();
    }

    int OnConnected(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
    {
        NF_SHARE_PTR<ConnectData> pServerInfo = GetServerNetInfo(nServerID);
        if(pServerInfo.get())
        {
            AddServerWeightData(pServerInfo);
            pServerInfo->eState = ConnectDataState::NORMAL;
        }

        return 0;
    }

    int OnDisConnected(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
    {
        NF_SHARE_PTR<ConnectData> pServerInfo = GetServerNetInfo(nServerID);
        if(nullptr != pServerInfo)
        {
            RemoveServerWeightData(pServerInfo);
            pServerInfo->eState = ConnectDataState::DISCONNECT;
            pServerInfo->mnLastActionTime = GetPluginManager()->GetNowTime();
        }

        return 0;
    }

    void ProcessAddNetConnect()
    {
        std::list<ConnectData>::iterator it = mxTempNetList.begin();
        for(; it != mxTempNetList.end(); ++it)
        {
            const ConnectData& xInfo = *it;
            NF_SHARE_PTR<ConnectData> xServerData = mxServerMap.GetElement(xInfo.nGameID);
            if(nullptr == xServerData)
            {
                //正常，添加新服务器
                xServerData = NF_SHARE_PTR<ConnectData>(NF_NEW ConnectData());

                xServerData->nGameID = xInfo.nGameID;
                xServerData->eServerType = xInfo.eServerType;
                xServerData->strIP = xInfo.strIP;

                if(xInfo.strIPAndPort.empty())
                {
                    std::string strPort;
                    NF_ToStr(strPort, xInfo.nPort);
                    xServerData->strIPAndPort = xInfo.strIP + ":" + strPort;
                }
                else
                {
                    xServerData->strIPAndPort = xInfo.strIPAndPort;
                }

                xServerData->strName = xInfo.strName;
                xServerData->eState = ConnectDataState::CONNECTING;
                xServerData->nPort = xInfo.nPort;
                xServerData->mnLastActionTime = GetPluginManager()->GetNowTime();

                xServerData->mxNetModule = NF_SHARE_PTR<AFCNetClient>(NF_NEW AFCNetClient(this, &AFINetClientModule::OnReceiveNetPack, &AFINetClientModule::OnSocketNetEvent));

                xServerData->mxNetModule->Initialization(xServerData->strIPAndPort, xServerData->nGameID);

                mxServerMap.AddElement(xInfo.nGameID, xServerData);
            }
        }

        mxTempNetList.clear();
    }

    bool GetServerMachineData(const std::string& strServerID, AFCMachineNode& xMachineData)
    {
        uint32_t nCRC32 = NFrame::CRC32(strServerID);
        return mxConsistentHash.GetSuitNode(nCRC32, xMachineData);
    }

    void AddServerWeightData(NF_SHARE_PTR<ConnectData> xInfo)
    {
        //根据权重创建节点
        for(int j = 0; j < EConstDefine_DefaultWeith; ++j)
        {
            AFCMachineNode vNode(j);

            vNode.nMachineID = xInfo->nGameID;
            vNode.strIP = xInfo->strIP;
            vNode.nPort = xInfo->nPort;
            vNode.nWeight = EConstDefine_DefaultWeith;
            mxConsistentHash.Insert(vNode);
        }
    }

    void RemoveServerWeightData(NF_SHARE_PTR<ConnectData> xInfo)
    {
        for(int j = 0; j < EConstDefine_DefaultWeith; ++j)
        {
            AFCMachineNode vNode(j);

            vNode.nMachineID = xInfo->nGameID;
            vNode.strIP = xInfo->strIP;
            vNode.nPort = xInfo->nPort;
            vNode.nWeight = EConstDefine_DefaultWeith;
            mxConsistentHash.Erase(vNode);
        }
    }

protected:
    void OnReceiveNetPack(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        OnReceiveBaseNetPack(nMsgID, msg, nLen, xClientID);
    }

    void OnSocketNetEvent(const NetEventType eEvent, const AFGUID& xClientID, int nServerID)
    {
        if(eEvent == CONNECTED)
        {
            OnConnected(eEvent, xClientID, nServerID);
        }
        else
        {
            OnDisConnected(eEvent, xClientID, nServerID);
        }

        OnSocketBaseNetEvent(eEvent, xClientID, nServerID);
    }
private:
    AFMapEx<int, ConnectData> mxServerMap;
    AFCConsistentHash mxConsistentHash;

    std::list<ConnectData> mxTempNetList;

};
#endif
