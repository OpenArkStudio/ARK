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

#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIGameLogicModule.h"
#include "SDK/Interface/AFIElementModule.h"

class AFCCreateRoleModule
    : public AFIModule
{
public:
    AFCCreateRoleModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~AFCCreateRoleModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute(const float fLasFrametime, const float fStartedTime);
    virtual bool AfterInit();

protected:
    int OnLoadRoleBeginEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnLoadRoleFinalEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnCreateRoleEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnDeleteRoleEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnAcountDisConnectEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

private:

    //新建立的连接对象，等待他们自己发验证KEY，KEY验证后删掉
    //-1
    int mnConnectContainer;

    //选人大厅容器
    //-3
    int mnRoleHallContainer;

    AFIGameLogicModule* m_pGameLogicModule;
    //AFIEventProcessModule* m_pEventProcessModule;
    //static AFIDataBaseModule* m_pDataBaseModule;
    //AFIDataNoSqlModule* m_pNoSqlModule;
    AFIKernelModule* m_pKernelModule;
    AFIElementModule* m_pElementInfoModule;
    AFCCreateRoleModule* m_pThis;
};