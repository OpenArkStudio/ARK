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

#include "SDK/Interface/AFIGUIDModule.h"

namespace GUIDModule
{
class IdWorkerThreadUnsafe;
class IdWorkerThreadSafe;
}


class AFCGUIDModule
    : public AFIGUIDModule
{
public:
    explicit AFCGUIDModule(AFIPluginManager* p);
    virtual ~AFCGUIDModule() = default;

    virtual bool Init();
    virtual bool PreShut();

    virtual void SetGUIDMask(uint64_t mask);
    virtual AFGUID CreateGUID();

private:
    uint64_t mnMask; //area_id * 100000 + server_id
#ifdef AF_THREAD_SAFE
    GUIDModule::IdWorkerThreadSafe* m_pIDWoker;
#else
    GUIDModule::IdWorkerThreadUnsafe* m_pIDWoker;
#endif // AF_THREAD_SAFE
};