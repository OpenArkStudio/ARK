/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "interface/AFIPlugin.hpp"
#include "interface/AFIModule.hpp"
#include "utility/interface/AFITimerModule.hpp"
#include "log/interface/AFILogModule.hpp"
#include "utility/interface/AFIScheduleModule.hpp"
#include "utility/interface/AFIGUIDModule.hpp"

namespace ark {

class Sample1Module : public AFIModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit Sample1Module() = default;

    bool Init() override;
    bool PostInit() override;
    bool Update() override;
    bool PreShut() override;
    bool Shut() override;

protected:
    void TestTimer(const std::string& name, const AFGUID& entity_id);
    void TestOssLog();
    // bool TestSchduler(const int id, const int arg);

protected:
    AFITimerModule* m_pTimerModule;
    AFILogModule* m_pLogModule;
    AFIGUIDModule* m_pGUIDModule;
    AFIScheduleModule* m_pScheduleModule;
};

} // namespace ark
