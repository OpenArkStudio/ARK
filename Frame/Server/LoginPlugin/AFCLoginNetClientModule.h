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
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFINetClientManagerModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "Server/Interface/AFILoginNetServerModule.h"
#include "Server/Interface/AFILoginNetClientModule.h"

class AFCLoginNetClientModule : public AFILoginNetClientModule
{
public:
    explicit AFCLoginNetClientModule() = default;

    bool Init() override;
    bool PostInit() override;

    AFMapEx<int, AFMsg::ServerInfoReport>& GetWorldMap() override;

protected:
    int StartClient();

    void OnSocketMSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID);

    //////////////////////////////////////////////////////////////////////////
    void OnSelectServerResultProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnWorldInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //////////////////////////////////////////////////////////////////////////
    void Register(const int nServerID);

private:
    AFMapEx<int, AFMsg::ServerInfoReport> mWorldMap;

    AFILogModule* m_pLogModule;
    AFIBusModule* m_pBusModule;
    AFIMsgModule* m_pMsgModule;
    AFINetClientManagerModule* m_pNetClientManagerModule;
    AFILoginNetServerModule* m_pLoginNetServerModule;
};