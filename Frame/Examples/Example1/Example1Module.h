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

#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFITimerModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIScheduleModule.h"

class Example1Module : public AFIModule
{
public:
    explicit Example1Module() = default;

    bool Init() override;
    bool PostInit() override;
    bool Update() override;
    bool PreShut() override;
    bool Shut() override;

protected:
    void TestTimer(const std::string& name, const AFGUID& entity_id);
    bool TestSchduler(const int id, const int arg);
protected:
    AFITimerModule* m_pTimerModule;
    AFILogModule* m_pLogModule;
    AFIDynamicLogModule* m_pDynamicLogModule;
    AFIScheduleModule* m_pScheduleModule;
};