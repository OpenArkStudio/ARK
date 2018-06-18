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

#include "SDK/Core/Base/AFArrayMap.hpp"
#include "SDK/Proto/AFProtoCPP.hpp"
#include "SDK/Core/AFCHeartBeatManager.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "Server/Interface/AFINetClientModule.hpp"
#include "Server/Interface/AFIProxyServerToGameModule.h"
#include "Server/Interface/AFIProxyServerToWorldModule.h"
#include "Server/Interface/AFIProxyNetServerModule.h"
#include "Server/Interface/AFIProxyLogicModule.h"
#include "Server/Interface/AFINetServerModule.h"

class AFCProxyServerToWorldModule : public AFIProxyServerToWorldModule
{
public:
    explicit AFCProxyServerToWorldModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Update();

    virtual bool PostInit();

    virtual void LogReceive(const char* str) {/*log*/}
    virtual void LogSend(const char* str) {/*log*/}

    virtual AFINetClientModule* GetClusterModule();
    virtual bool VerifyConnectData(const std::string& strAccount, const std::string& strKey);

protected:

    void OnSocketWSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID);

    void Register(const int nServerID);

    void OnSelectServerResultProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnServerInfoProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    void LogServerInfo(const std::string& strServerInfo);

    void OnOtherMessage(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnBrocastmsg(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

private:
    struct ClientConnectData
    {
        ClientConnectData()
        {
            strAccount = "";
            strConnectKey = "";
        }

        std::string strAccount;
        std::string strConnectKey;
    };

    AFMapEx<std::string, ClientConnectData> mxWantToConnectMap;

private:
    AFILogModule* m_pLogModule;
    AFIProxyLogicModule* m_pProxyLogicModule;
    AFIKernelModule* m_pKernelModule;
    AFIProxyNetServerModule* m_pProxyServerNet_ServerModule;
    AFIElementModule* m_pElementModule;
    AFIClassModule* m_pClassModule;
    AFIProxyServerToGameModule* m_pProxyServerToGameModule;
    AFINetClientModule* m_pNetClientModule;
};