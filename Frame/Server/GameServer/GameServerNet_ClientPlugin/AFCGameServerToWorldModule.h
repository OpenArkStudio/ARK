/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @file      AFCGameServerToWorldModule.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFCGameServerToWorldModule                                                  *
*****************************************************************************/
#pragma once

#include "SDK/Proto/AFMsgDefine.h"
#include "SDK/Interface/AFINetClientModule.hpp"
#include "SDK/Interface/AFIGameServerNet_ClientModule.h"
#include "SDK/Interface/AFIGameServerNet_ServerModule.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIGameLogicModule.h"

#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIGameServerToWorldModule.h"

class AFCGameServerToWorldModule : public AFIGameServerToWorldModule
{
public:
    AFCGameServerToWorldModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

    virtual void SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen);
    virtual AFINetClientModule* GetClusterClientModule();

protected:
    void OnSocketWSEvent(const NetEventType e, const AFGUID& xClientID, const int nServerID);

protected:
    void Register(const int nSeverID);
    void RefreshWorldInfo();
    void TransPBToProxy(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& var);

private:
    void SendOnline(const AFGUID& self);
    void SendOffline(const AFGUID& self);

private:
    AFILogModule* m_pLogModule;
    AFIKernelModule* m_pKernelModule;
    AFIClassModule* m_pClassModule;
    AFIElementModule* m_pElementModule;
    AFINetClientModule* m_pNetClientModule;
    AFIGameServerNet_ServerModule* m_pGameServerNet_ServerModule;
};

