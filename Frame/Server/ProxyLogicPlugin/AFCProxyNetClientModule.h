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
#include "SDK/Core/AFArrayMap.hpp"
#include "SDK/Core/AFCHeartBeatManager.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIConfigModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFINetClientManagerModule.h"
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "Server/Interface/AFIProxyNetClientModule.h"
#include "Server/Interface/AFIProxyNetServerModule.h"

class AFCProxyNetClientModule : public AFIProxyNetClientModule
{
public:
    explicit AFCProxyNetClientModule() = default;

    bool Init() override;
    bool PostInit() override;

    bool VerifyConnectData(const std::string& strAccount, const std::string& strKey) override;

protected:
    int StartClient();
    void OnSocketWSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID);

    void Register(const int nServerID);

    void OnSelectServerResultProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnServerInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    void LogServerInfo(const std::string& strServerInfo);

    void OnOtherMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnBrocastmsg(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnAckEnterGame(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    class ClientConnectData
    {
    public:
        ClientConnectData()
        {
        }

        std::string strAccount{};
        std::string strConnectKey{};
    };

    AFMapEx<std::string, ClientConnectData> mxWantToConnectMap;

private:
    AFILogModule* m_pLogModule;
    AFIMsgModule* m_pMsgModule;
    AFIBusModule* m_pBusModule;
    AFIProxyNetServerModule* m_pProxyServerNet_ServerModule;
    AFINetClientManagerModule* m_pNetClientManagerModule;
};