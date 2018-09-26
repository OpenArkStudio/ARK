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

#pragma once

#include "Common/AFProtoCPP.hpp"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIConfigModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFINetServerManagerModule.h"
#include "Server/Interface/AFIWorldNetServerModule.h"

class AFCWorldNetServerModule : public AFIWorldNetServerModule
{
public:
    explicit AFCWorldNetServerModule()
    {
        mnLastCheckTime = pPluginManager->GetNowTime();
    }

    bool Init() override;
    bool Update() override;

    virtual bool SendMsgToGame(const int nGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer = AFGUID());
    virtual bool SendMsgToGame(const AFIDataList& argObjectVar, const AFIDataList& argGameID,  const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData);
    virtual bool SendMsgToPlayer(const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer);

    virtual int OnObjectListEnter(const AFIDataList& self, const AFIDataList& argVar);
    virtual int OnObjectListLeave(const AFIDataList& self, const AFIDataList& argVar);

    virtual int OnViewDataNodeEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self);
    virtual int OnSelfDataNodeEnter(const AFGUID& self, const AFIDataList& argGameID);
    virtual int OnViewDataTableEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self);
    virtual int OnSelfDataTableEnter(const AFGUID& self, const AFIDataList& argGameID);

    virtual ARK_SHARE_PTR<AFServerData> GetSuitProxyForEnter();
    virtual AFINetServerService* GetNetServer();

    virtual int GetPlayerGameID(const AFGUID self);

protected:
    int StartServer();
    void OnSocketEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID);

    //连接丢失,删2层(连接对象，帐号对象)
    void OnClientDisconnect(const AFGUID& xClientID);
    //有连接
    void OnClientConnected(const AFGUID& xClientID);

    void OnGameServerRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnGameServerUnRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnRefreshGameServerInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    void OnProxyServerRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnProxyServerUnRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnRefreshProxyServerInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    int OnLeaveGameProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    //////////////////////////////////////////////////////////////////////////

    void SynGameToProxy();
    void SynGameToProxy(const AFGUID& xClientID);

    //////////////////////////////////////////////////////////////////////////
    void LogGameServer();

    void OnOnlineProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnOfflineProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);


private:
    int64_t mnLastCheckTime;

    //serverid,data
    AFMapEx<int, AFServerData> mGameMap;
    AFMapEx<int, AFServerData> mProxyMap;

    AFIKernelModule* m_pKernelModule;
    AFILogModule* m_pLogModule;
    AFIBusModule* m_pBusModule;
    AFINetServerManagerModule* m_pNetServerManagerModule;

    AFINetServerService* m_pNetServer;
};