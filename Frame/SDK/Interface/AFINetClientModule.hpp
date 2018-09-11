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

#include "Common/AFApp.hpp"
#include "SDK/Core/AFMap.hpp"
#include "SDK/Core/AFDateTime.hpp"
#include "SDK/Core/AFGUID.hpp"
#include "SDK/Core/AFCConsistentHash.hpp"
#include "SDK/Interface/AFIModule.h"
#include "SDK/Interface/AFINetModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/NetPlugin/AFCTCPClient.h" //maybe another way to include this header file

class ConnectData
{
public:
    enum ConnectState
    {
        DISCONNECT,
        CONNECTING,
        CONNECTED,
        RECONNECT,
    };

    ConnectData() = default;

    int nGameID{ 0 };
    uint8_t eServerType{ ARK_APP_TYPE::ARK_APP_DEFAULT };
    std::string strIP{ "" };
    int nPort{ 0 };
    std::string strIPAndPort{ "" }; //TODO:delete
    std::string strName{ "" };
    ConnectState eState{ DISCONNECT };
    int64_t mnLastActionTime{ 0 };

    ARK_SHARE_PTR<AFINet> mxNetClientPtr{ nullptr };
};

class AFINetClientModule : public AFINetModule
{
public:
    enum EConstDefine
    {
        EConstDefine_DefaultWeight = 500,
    };

    AFINetClientModule() = delete;
    explicit AFINetClientModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Shut()
    {
        int id = 0;

        for (auto connect_data = mxServerMap.First(id); connect_data != nullptr; connect_data = mxServerMap.Next(id))
        {
            if (connect_data->mxNetClientPtr != nullptr)
            {
                connect_data->mxNetClientPtr->Shutdown();
            }
        }

        return true;
    }

    virtual bool Update()
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
    void SendByServerID(const int nServerID, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    {
        SendByServerID(nServerID, nMsgID, strData.c_str(), strData.length(), nPlayerID);
    }

    //裸数据,发时组包
    void SendByServerID(const int nServerID, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID)
    {
        ARK_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);

        if (pServer)
        {
            ARK_SHARE_PTR<AFINet> pNetModule = pServer->mxNetClientPtr;

            if (pNetModule != nullptr)
            {
                pNetModule->SendMsgWithOutHead(nMsgID, msg, nLen, 0, nPlayerID);
            }
        }
    }
    //裸数据,发时组包
    void SendToAllServer(const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    {
        ARK_SHARE_PTR<ConnectData> pServer = mxServerMap.First();

        while (pServer)
        {
            ARK_SHARE_PTR<AFINet> pNetModule = pServer->mxNetClientPtr;

            if (pNetModule != nullptr)
            {
                pNetModule->SendMsgWithOutHead(nMsgID, strData.data(), strData.size(), 0, nPlayerID);
            }

            pServer = mxServerMap.Next();
        }
    }
    void SendToServerByPB(const int nServerID, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    {
        std::string strData;

        if (!PackMsgToBasePB(xData, nPlayerID, strData))
        {
            //add log
            return;
        }

        ARK_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);

        if (pServer)
        {
            ARK_SHARE_PTR<AFINet> pNetModule = pServer->mxNetClientPtr;

            if (pNetModule.get())
            {
                pNetModule->SendMsgWithOutHead(nMsgID, strData.data(), strData.length(), AFGUID(0), nPlayerID);
            }
        }
    }

    void SendToAllServerByPB(const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    {
        std::string strData;

        if (!PackMsgToBasePB(xData, nPlayerID, strData))
        {
            //add log
            return;
        }

        for (ARK_SHARE_PTR<ConnectData> pServer = mxServerMap.First(); pServer != nullptr; pServer = mxServerMap.Next())
        {
            ARK_SHARE_PTR<AFINet> pNetModule = pServer->mxNetClientPtr;

            if (pNetModule != nullptr)
            {
                pNetModule->SendMsgWithOutHead(nMsgID, strData.data(), strData.length(), AFGUID(0), nPlayerID);
            }
        }
    }

    void SendBySuit(const std::string& strHashKey, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    {
        uint32_t nCRC32 = CRC32(strHashKey);
        SendBySuit(nCRC32, nMsgID, strData, nPlayerID);
    }

    void SendBySuit(const std::string& strHashKey, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID)
    {
        uint32_t nCRC32 = CRC32(strHashKey);
        SendBySuit(nCRC32, nMsgID, msg, nLen, nPlayerID);
    }

    void SendBySuit(const int& nHashKey, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID)
    {
        SendBySuit(nHashKey, nMsgID, strData.c_str(), strData.length(), nPlayerID);
    }

    void SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID)
    {
        if (mxConsistentHash.Size() <= 0)
        {
            return;
        }

        AFCMachineNode xNode;

        if (!GetServerMachineData(ARK_LEXICAL_CAST<std::string>(nHashKey), xNode))
        {
            return;
        }

        SendByServerID(xNode.nMachineID, nMsgID, msg, nLen, nPlayerID);
    }

    void SendSuitByPB(const std::string& strHashKey, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    {
        uint32_t nCRC32 = CRC32(strHashKey);
        SendSuitByPB(nCRC32, nMsgID, xData, nPlayerID);
    }

    void SendSuitByPB(const int& nHashKey, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID)
    {
        if (mxConsistentHash.Size() <= 0)
        {
            return;
        }

        AFCMachineNode xNode;

        if (!GetServerMachineData(ARK_LEXICAL_CAST<std::string> (nHashKey), xNode))
        {
            return;
        }

        SendToServerByPB(xNode.nMachineID, nMsgID, xData, nPlayerID);
    }

    ARK_SHARE_PTR<ConnectData> GetServerNetInfo(const int nServerID)
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
        if (!xData.SerializeToString(&outData))
        {
            return false;
        }

        return true;
    }

    void ProcessExecute()
    {
        for (bool bRet = mxServerMap.Begin(); bRet; bRet = mxServerMap.Increase())
        {
            const auto& pServerData = mxServerMap.GetCurrentData();

            switch (pServerData->eState)
            {
            case ConnectData::DISCONNECT:
                {
                    if (pServerData->mxNetClientPtr != nullptr)
                    {
                        pServerData->mxNetClientPtr = nullptr;
                        pServerData->eState = ConnectData::RECONNECT;
                    }
                }
                break;

            case ConnectData::CONNECTING:
                {
                    if (pServerData->mxNetClientPtr)
                    {
                        pServerData->mxNetClientPtr->Update();
                    }
                }
                break;

            case ConnectData::CONNECTED:
                {
                    if (pServerData->mxNetClientPtr)
                    {
                        pServerData->mxNetClientPtr->Update();

                        KeepState(pServerData.get());
                    }
                }
                break;

            case ConnectData::RECONNECT:
                {
                    //计算时间
                    if ((pServerData->mnLastActionTime + 30 * AFTimespan::SECOND_MS) >= GetPluginManager()->GetNowTime())
                    {
                        break;
                    }

                    if (nullptr != pServerData->mxNetClientPtr)
                    {
                        pServerData->mxNetClientPtr = nullptr;
                    }

                    pServerData->eState = ConnectData::CONNECTING;
                    //TODO:修改为通过外部传入的内容来区分tcp http wss
                    pServerData->mxNetClientPtr = std::make_shared<AFCTCPClient>(this, &AFINetClientModule::OnReceiveNetPack, &AFINetClientModule::OnSocketNetEvent);
                    pServerData->mxNetClientPtr->Start(pServerData->nGameID, pServerData->strIP, pServerData->nPort, false);
                }
                break;

            default:
                break;
            }
        }
    }

    void KeepReport(ConnectData* pServerData) {/*Will add*/}
    void LogServerInfo(const std::string& strServerInfo) {/*Will add*/}

private:
    virtual void LogServerInfo()
    {
        LogServerInfo("This is a client, begin to print Server Info----------------------------------");

        bool bRet = mxServerMap.Begin();

        while (bRet)
        {
            const auto& pServerData = mxServerMap.GetCurrentData();
            std::ostringstream stream;
            stream << "Type: " << pServerData->eServerType << " ProxyServer ID: " << pServerData->nGameID << " State: " << pServerData->eState << " IP: " << pServerData->strIP << " Port: " << pServerData->nPort;

            LogServerInfo(stream.str());

            bRet = mxServerMap.Increase();
        }

        LogServerInfo("This is a client, end to print Server Info----------------------------------");
    };

    void KeepState(ConnectData* pServerData)
    {
        if (pServerData->mnLastActionTime + 10 > GetPluginManager()->GetNowTime())
        {
            return;
        }

        pServerData->mnLastActionTime = GetPluginManager()->GetNowTime();

        KeepReport(pServerData);
        LogServerInfo();
    }

    int OnConnected(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
    {
        ARK_SHARE_PTR<ConnectData> pServerInfo = GetServerNetInfo(nServerID);

        if (pServerInfo.get())
        {
            AddServerWeightData(pServerInfo);
            pServerInfo->eState = ConnectData::CONNECTED;
        }

        return 0;
    }

    int OnDisConnected(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
    {
        ARK_SHARE_PTR<ConnectData> pServerInfo = GetServerNetInfo(nServerID);

        if (nullptr != pServerInfo)
        {
            RemoveServerWeightData(pServerInfo);
            pServerInfo->eState = ConnectData::DISCONNECT;
            pServerInfo->mnLastActionTime = GetPluginManager()->GetNowTime();
        }

        return 0;
    }

    void ProcessAddNetConnect()
    {
        std::list<ConnectData>::iterator it = mxTempNetList.begin();

        for (; it != mxTempNetList.end(); ++it)
        {
            const ConnectData& xInfo = *it;
            ARK_SHARE_PTR<ConnectData> xServerData = mxServerMap.GetElement(xInfo.nGameID);

            if (nullptr == xServerData)
            {
                //正常，添加新服务器
                xServerData = std::make_shared<ConnectData>();

                xServerData->nGameID = xInfo.nGameID;
                xServerData->eServerType = xInfo.eServerType;
                xServerData->strIP = xInfo.strIP;

                if (xInfo.strIPAndPort.empty())
                {
                    std::string strPort;
                    AFMisc::ARK_TO_STR(strPort, xInfo.nPort);
                    xServerData->strIPAndPort = xInfo.strIP + ":" + strPort;
                }
                else
                {
                    xServerData->strIPAndPort = xInfo.strIPAndPort;
                }

                xServerData->strName = xInfo.strName;
                xServerData->eState = ConnectData::CONNECTING;
                xServerData->nPort = xInfo.nPort;
                xServerData->mnLastActionTime = GetPluginManager()->GetNowTime();

                xServerData->mxNetClientPtr = std::make_shared<AFCTCPClient>(this, &AFINetClientModule::OnReceiveNetPack, &AFINetClientModule::OnSocketNetEvent);

                xServerData->mxNetClientPtr->Start(xServerData->nGameID, xServerData->strIP, xServerData->nPort);

                if (!mxServerMap.AddElement(xInfo.nGameID, xServerData))
                {
                    //add log
                }
            }
        }

        mxTempNetList.clear();
    }

    bool GetServerMachineData(const std::string& strServerID, AFCMachineNode& xMachineData)
    {
        uint32_t nCRC32 = CRC32(strServerID);
        return mxConsistentHash.GetSuitNode(nCRC32, xMachineData);
    }

    void AddServerWeightData(ARK_SHARE_PTR<ConnectData>& xInfo)
    {
        //根据权重创建节点
        for (int i = 0; i < EConstDefine_DefaultWeight; ++i)
        {
            AFCMachineNode vNode(i);

            vNode.nMachineID = xInfo->nGameID;
            vNode.strIP = xInfo->strIP;
            vNode.nPort = xInfo->nPort;
            vNode.nWeight = EConstDefine_DefaultWeight;
            mxConsistentHash.Insert(vNode);
        }
    }

    void RemoveServerWeightData(ARK_SHARE_PTR<ConnectData>& xInfo)
    {
        for (int i = 0; i < EConstDefine_DefaultWeight; ++i)
        {
            AFCMachineNode vNode(i);

            vNode.nMachineID = xInfo->nGameID;
            vNode.strIP = xInfo->strIP;
            vNode.nPort = xInfo->nPort;
            vNode.nWeight = EConstDefine_DefaultWeight;
            mxConsistentHash.Erase(vNode);
        }
    }

protected:
    void OnReceiveNetPack(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID)
    {
        OnRecvBaseNetPack(xHead, nMsgID, msg, nLen, xClientID);
    }

    void OnSocketNetEvent(const NetEventType eEvent, const AFGUID& xClientID, int nServerID)
    {
        if (eEvent == CONNECTED)
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