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

#include "SDK/Proto/AFProtoCPP.hpp"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIGUIDModule.h"
#include "Server/Interface/AFINetServerModule.h"
#include "Server/Interface/AFILoginToMasterModule.h"
#include "Server/Interface/AFILoginNetServerModule.h"
#include "Server/Interface/AFILoginLogicModule.h"

class AFCLoginNetServerModule
    : public AFILoginNetServerModule
{
public:
    AFCLoginNetServerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Update();


    virtual bool PreShut();
    virtual bool PostInit();

    virtual void LogReceive(const char* str) {/*log*/}
    virtual void LogSend(const char* str) {/*log*/}

    virtual int OnSelectWorldResultsProcess(const int nWorldID, const AFGUID xSenderID, const int nLoginID, const std::string& strAccount, const std::string& strWorldIP, const int nWorldPort, const std::string& strKey);

protected:
    void OnSocketClientEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID);

    void OnClientDisconnect(const AFGUID& xClientID);
    void OnClientConnected(const AFGUID& xClientID);

    //登入
    void OnLoginProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //选择大世界
    void OnSelectWorldProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //申请查看世界列表
    void OnViewWorldProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    void OnHeartBeat(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnLogOut(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void InvalidMessage(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    void SynWorldToClient(const AFGUID& xClientID);

private:
    AFMapEx<AFGUID, SessionData> mmClientSessionData;

    AFILoginToMasterModule* m_pLoginToMasterModule;
    AFIClassModule* m_pClassModule;
    AFIElementModule* m_pElementModule;
    AFIKernelModule* m_pKernelModule;
    AFILogModule* m_pLogModule;
    AFILoginLogicModule* m_pLoginLogicModule;
    AFIGUIDModule* m_pUUIDModule;
    AFINetServerModule* m_pNetModule;
};