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

#include "AFCScheduleModule.h"
#include "SDK/Interface/AFIPluginManager.h"

namespace ark
{

    bool AFCScheduleModule::Init()
    {
        mxCronSheduler = std::make_shared<AFCronSheduler>();
        return true;
    }

    bool AFCScheduleModule::PreShut()
    {
        mxCronSheduler->Clear();
        return true;
    }

    bool AFCScheduleModule::Update()
    {
        mxCronSheduler->Update(pPluginManager->GetNowTime());
        return true;
    }

    bool AFCScheduleModule::AddSchedule(const int id, const int user_arg, const char* cron_expression, SCHEDULER_FUNCTOR_PTR cb)
    {
        return mxCronSheduler->AddCron(id, user_arg, cron_expression, pPluginManager->GetNowTime(), cb);
    }

    bool AFCScheduleModule::RemoveSchedule(const int cron_id)
    {
        return (mxCronSheduler->RemoveCron(cron_id) > 0);
    }

}