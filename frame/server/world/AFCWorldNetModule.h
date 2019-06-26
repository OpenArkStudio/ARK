/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2019 ArkNX authors.
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

#include "base/AFProtoCPP.hpp"
#include "interface/AFIKernelModule.h"
#include "interface/AFILogModule.h"
#include "interface/AFITimerModule.h"
#include "interface/AFIMsgModule.h"
#include "interface/AFIBusModule.h"
#include "interface/AFIMsgModule.h"
#include "interface/AFINetServiceManagerModule.h"
#include "interface/AFIWorldNetModule.h"

namespace ark
{

    class AFCWorldNetModule : public AFIWorldNetModule
    {
    public:
        explicit AFCWorldNetModule() = default;

        bool Init() override;
        bool PostInit() override;
        bool PreUpdate() override;

        virtual bool SendMsgToGame(const int nGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer = AFGUID());
        virtual bool SendMsgToGame(const AFIDataList& argObjectVar, const AFIDataList& argGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData);
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
        int StartClient();

        //void OnGameServerRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        //void OnGameServerUnRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        //void OnRefreshGameServerInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        //void OnProxyServerRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        //void OnProxyServerUnRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        //void OnRefreshProxyServerInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        int OnLeaveGameProcess(const AFNetMsg* msg);
        //////////////////////////////////////////////////////////////////////////

        void SynGameToProxy();
        void SynGameToProxy(const AFGUID& xClientID);

        //////////////////////////////////////////////////////////////////////////
        void LogGameServer();

        //void OnOnlineProcess(const AFNetMsg* msg);
        //void OnOfflineProcess(const AFNetMsg* msg);

        //////////////////////////////////////////////////////////////////////////

        void RefreshWorldInfo();

        void OnSelectServerProcess(const AFNetMsg* msg);
        void OnKickClientProcess(const AFNetMsg* msg);

    private:
        AFMapEx<int, AFServerData> reg_servers_;


        int64_t mnLastCheckTime;

        //server id, data
        AFMapEx<int, AFServerData> mGameMap;
        AFMapEx<int, AFServerData> mProxyMap;

        AFIKernelModule* m_pKernelModule;
        AFILogModule* m_pLogModule;
        AFIBusModule* m_pBusModule;
        AFIMsgModule* m_pMsgModule;
        AFINetServiceManagerModule* m_pNetServiceManagerModule;
        AFITimerModule* m_pTimerModule;

        AFINetServerService* m_pNetServer;
    };

}