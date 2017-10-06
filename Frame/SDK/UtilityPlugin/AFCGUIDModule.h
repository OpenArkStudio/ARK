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

#include "SDK/Interface/AFIGUIDModule.h"

namespace GUIDModule
{
extern uint64_t GetNowInMsec();
class IdWorkerUnThreadSafe;
class IdWorkerThreadSafe;
}


class AFCGUIDModule
    : public AFIGUIDModule
{
public:

    AFCGUIDModule(AFIPluginManager* p);
    virtual ~AFCGUIDModule() {}

    virtual bool Init();
    virtual bool AfterInit();
    virtual bool Execute();
    virtual bool BeforeShut();
    virtual bool Shut();

    virtual void SetWorkerAndDatacenter(uint16_t worker_id, uint16_t data_center_id);
    virtual uint64_t CreateGUID();

private:
#ifdef AF_THREAD_SAFE
    GUIDModule::IdWorkerThreadSafe* m_pIDWoker;
#else
    GUIDModule::IdWorkerUnThreadSafe* m_pIDWoker;
#endif // AF_THREAD_SAFE
};