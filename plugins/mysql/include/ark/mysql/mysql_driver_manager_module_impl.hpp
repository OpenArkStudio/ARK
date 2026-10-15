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

#include "ark/mysql/mysql_driver_manager_module.hpp"
#include "ark/mysql/mysql_driver_module.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

class mysql_driver_manager_module_impl final : public mysql_driver_manager_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
private:
    object_map<int, mysql_driver_module> mysql_modules_;
    object_map<int, mysql_driver_module> invalid_mysql_modules_;
    //    int64_t last_check_time{0};

public:
    bool add_mysql_server(const int server_id, const std::string& dns, const std::string& ip, const int port,
        const std::string db_name, const std::string db_user, const std::string db_pwd, const int reconnect_time = 10,
        const int reconnect_count = -1) override;
    mysql_driver_module* get_mysql_driver() override;
    void check_mysql() override;

protected:
    std::string get_ip_by_host_name(const std::string& host_name);
};

} // namespace ark
