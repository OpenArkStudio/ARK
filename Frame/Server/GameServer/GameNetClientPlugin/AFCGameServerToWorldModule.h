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
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "Server/Interface/AFINetClientModule.hpp"
#include "Server/Interface/AFIGameNetServerModule.h"
#include "Server/Interface/AFIGameServerToWorldModule.h"

class AFCGameServerToWorldModule : public AFIGameServerToWorldModule
{
public:
    explicit AFCGameServerToWorldModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual bool Init();
    virtual bool Update();
    virtual bool PostInit();

    virtual void SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen);
    virtual AFINetClientModule* GetClusterClientModule();

protected:
    void OnSocketWSEvent(const NetEventType e, const AFGUID& xClientID, const int nServerID);

    void Register(const int nSeverID);
    void RefreshWorldInfo();
    void TransPBToProxy(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var);

    void SendOnline(const AFGUID& self);
    void SendOffline(const AFGUID& self);

private:
    AFILogModule* m_pLogModule;
    AFIKernelModule* m_pKernelModule;
    AFIClassModule* m_pClassModule;
    AFIElementModule* m_pElementModule;
    AFINetClientModule* m_pNetClientModule;
    AFIGameNetServerModule* m_pGameServerNet_ServerModule;
};