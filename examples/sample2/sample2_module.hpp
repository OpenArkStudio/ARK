/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "ark/kernel/data.hpp"
#include "ark/module.hpp"
#include "ark/plugin.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

class sample2_module final : public module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit sample2_module() = default;

    bool init() override;
    bool post_init() override;
    bool pre_shut() override;
    bool shut() override;

protected:
    int OnDataNodeCB(const guid_t& self, const std::string& node_name, const data& old_var, const data& new_var);
};

} // namespace ark
