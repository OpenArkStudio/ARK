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
#include "kernel/interface/AFIData.hpp"

namespace ark {

class Sample2Module : public AFIModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit Sample2Module() = default;

    bool Init() override;
    bool PostInit() override;
    bool PreShut() override;
    bool Shut() override;

protected:
    int OnDataNodeCB(const AFGUID& self, const std::string& node_name, const AFIData& old_var, const AFIData& new_var);
};

} // namespace ark
