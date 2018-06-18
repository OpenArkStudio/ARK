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

#include "SDK/Interface/AFITimerModule.h"
#include "SDK/Core/Base/AFTimer.hpp"

class AFCTimerModule : public AFITimerModule
{
public:
    explicit AFCTimerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual ~AFCTimerModule() = default;

    virtual bool Init();
    virtual bool PreShut();
    virtual bool Update();

    virtual bool RemoveTimer(const std::string& name);
    virtual bool RemoveTimer(const std::string& name, const AFGUID& entity_id);

protected:
    virtual bool AddSingleTimer(const std::string& name, const AFGUID& entity_id, const uint32_t interval_time, const uint32_t count, TIMER_FUNCTOR_PTR cb);
    virtual bool AddForeverTimer(const std::string& name, const AFGUID& entity_id, const int64_t interval_time, TIMER_FUNCTOR_PTR cb);

private:
    std::shared_ptr<AFTimerManager> mxTimerManager;
};