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

#include <mysql++/mysql++.h>

#include "ark/module.hpp"
#include "ark/mysql/mysql_driver_module.hpp"

namespace ark {

class mysql_module : public module
{
public:
    virtual bool add_mysql_server(const int server_id, const std::string& dns, const std::string& ip, const int port,
        const std::string db_name, const std::string db_user, const std::string db_pwd, const int reconnect_time = 10,
        const int reconnect_count = -1) = 0;
    virtual bool update_or_insert(const std::string& record_name, const std::string& key,
        const std::vector<std::string>& field_vec, const std::vector<std::string>& value_vec) = 0;
    virtual bool query(const std::string& record_name, const std::string& key,
        const std::vector<std::string>& field_vec, std::vector<std::string>& value_vec) = 0;
    virtual bool select(const std::string& record_name, const std::string& key,
        const std::vector<std::string>& field_vec, std::vector<std::string>& value_vec) = 0;

    virtual bool erase(const std::string& record_name, const std::string& key) = 0;
    virtual bool exists(const std::string& record_name, const std::string& key, bool& exists) = 0;
    virtual bool keys(
        const std::string& record_name, const std::string& key_name, std::vector<std::string>& value_vec) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool insert(const std::string& document, std::vector<kv_pair>& data) = 0;
    virtual bool update_record(const std::string& document, std::vector<kv_pair>& data, std::vector<where>& where) = 0;
    virtual bool select(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        order_by& order, int limit, result_arr& results) = 0;
    virtual bool select_one(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        order_by& order, result& result) = 0;
};

} // namespace ark
