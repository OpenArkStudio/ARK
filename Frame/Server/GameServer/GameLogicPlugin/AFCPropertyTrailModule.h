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

#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFILogModule.h"
#include "Server/Interface/AFIPropertyModule.h"
#include "Server/Interface/AFIPropertyConfigModule.h"
#include "Server/Interface/AFIPropertyTrailModule.h"

class AFCPropertyTrailModule : public AFIPropertyTrailModule
{
public:
    explicit AFCPropertyTrailModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~AFCPropertyTrailModule() = default;

    virtual bool PostInit();

    virtual void StartTrail(const AFGUID self);
    virtual void EndTrail(const AFGUID self);

protected:

    int LogObjectData(const AFGUID& self);
    int TrailObjectData(const AFGUID& self);

    int OnObjectPropertyEvent(const AFGUID& self, const std::string& nodeName, const AFIData& oldVar, const AFIData& newVar);

    int OnEntityTableEvent(const AFGUID& self, const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar);

private:
    AFIKernelModule* m_pKernelModule;
    AFIElementModule* m_pElementModule;
    AFIClassModule* m_pClassModule;
    AFILogModule* m_pLogModule;
};