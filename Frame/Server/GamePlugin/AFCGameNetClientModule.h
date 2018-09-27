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
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFINetClientManagerModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "Server/Interface/AFIGameNetServerModule.h"
#include "Server/Interface/AFIGameNetClientModule.h"
#include "Server/Interface/AFIGameNetServerModule.h"

class AFCGameNetClientModule : public AFIGameNetClientModule
{
public:
    explicit AFCGameNetClientModule() = default;

    bool Init() override;
    bool PreUpdate() override;

protected:
    int StartClient();

    void OnSocketWSEvent(const NetEventType e, const AFGUID& xClientID, const int nServerID);

    void Register(const int nSeverID);
    void RefreshWorldInfo();
    void TransPBToProxy(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var);

    void SendOnline(const AFGUID& self);
    void SendOffline(const AFGUID& self);

private:
    AFILogModule* m_pLogModule;
    AFIKernelModule* m_pKernelModule;
    AFIClassModule* m_pClassModule;
    AFIConfigModule* m_pConfigModule;
    AFIBusModule* m_pBusModule;
    AFINetClientManagerModule* m_pNetClientManagerModule;
    AFIGameNetServerModule* m_pGameNetServerModule;
    AFIMsgModule* m_pMsgModule;
};