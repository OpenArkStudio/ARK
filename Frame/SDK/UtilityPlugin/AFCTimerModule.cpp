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

#include "AFCTimerModule.h"
#include "SDK/Interface/AFIPluginManager.h"

bool AFCTimerModule::Init()
{
    mxTimerManager = std::make_shared<AFTimerManager>();
    mxTimerManager->Init(pPluginManager->GetNowTime());

    return true;
}

bool AFCTimerModule::PreShut()
{
    mxTimerManager->Shut();
    return true;
}

bool AFCTimerModule::Update()
{
    mxTimerManager->Update(pPluginManager->GetNowTime());
    return true;
}

bool AFCTimerModule::RemoveTimer(const std::string& name)
{
    return mxTimerManager->RemoveTimer(name);
}

bool AFCTimerModule::RemoveTimer(const std::string& name, const AFGUID& entity_id)
{
    return mxTimerManager->RemoveTimer(name, entity_id);
}

bool AFCTimerModule::AddSingleTimer(const std::string& name, const AFGUID& entity_id, const uint32_t interval_time, const uint32_t count, TIMER_FUNCTOR_PTR cb)
{
    return mxTimerManager->AddSingleTimer(name, entity_id, interval_time, count, cb);
}

bool AFCTimerModule::AddForeverTimer(const std::string& name, const AFGUID& entity_id, const uint32_t interval_time, TIMER_FUNCTOR_PTR cb)
{
    return mxTimerManager->AddForverTimer(name, entity_id, interval_time, cb);
}