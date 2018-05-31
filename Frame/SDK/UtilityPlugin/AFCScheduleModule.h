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

#include "SDK/Interface/AFIScheduleModule.h"
#include "SDK/Core/Base/AFCronScheduler.hpp"

class AFCScheduleModule : public AFIScheduleModule
{
public:
    explicit AFCScheduleModule(AFIPluginManager* p);

    virtual ~AFCScheduleModule() {}

    virtual bool Init();
    virtual bool PostInit();

    virtual bool PreShut();
    virtual bool Shut();

    virtual bool Update();

    virtual bool RemoveSchedule(const int cron_id);

protected:
    virtual bool AddSchedule(const int id, const int user_arg, const char* cron_expression, SCHEDULER_FUNCTOR_PTR cb);

private:
    std::shared_ptr<AFCronSheduler> mxCronSheduler;
};