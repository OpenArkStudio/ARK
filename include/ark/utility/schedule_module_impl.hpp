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

#include "ark/plugin_manager.hpp"
#include "ark/utility/schedule_module.hpp"
#include "ark/utility/scheduler.hpp"

namespace ark {

class schedule_module_impl final : public schedule_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;

protected:
    void add_interval_schedule(const std::chrono::system_clock::duration interval, scheduler_functor&& cb) override;

    void add_at_schedule(const std::string& target_time, scheduler_functor&& cb) override;
    void add_cron_schedule(const std::string& cron_expression, scheduler_functor&& cb) override;

private:
    std::shared_ptr<scheduler> scheduler_{nullptr};
};

} // namespace ark
