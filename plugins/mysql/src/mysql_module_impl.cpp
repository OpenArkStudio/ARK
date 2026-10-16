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

#include "ark/mysql/mysql_driver_manager_module_impl.hpp"
#include "ark/mysql/mysql_driver_module_impl.hpp"
#include "ark/mysql/mysql_module_impl.hpp"
#include "ark/mysql/mysql_plugin.hpp"

namespace ark {

mysql_module_impl::mysql_module_impl() {
}

mysql_module_impl::~mysql_module_impl() {
}

bool mysql_module_impl::init() {
    mysql_driver_manager_ = std::make_shared<mysql_driver_manager_module_impl>();

    xml xml_doc(mysql_plugin::get_plugin_conf());
    auto root_node = xml_doc.get_root_node();
    ARK_ASSERT_RET_VAL(root_node.is_valid(), false);

    auto mysql_node = root_node.find_node("mysql");
    ARK_ASSERT_RET_VAL(mysql_node.is_valid(), false);

    for (auto instance_node = mysql_node.find_node("instance"); instance_node.is_valid(); instance_node.next_node())
    {
        auto server_id = instance_node.get_uint32("id");
        auto dns = instance_node.get_string("dns");
        auto ip = instance_node.get_string("ip");
        auto port = instance_node.get_uint32("port");
        auto db_name = instance_node.get_string("dbname");
        auto db_user = instance_node.get_string("dbuser");
        auto db_pwd = instance_node.get_string("dbpwd");

        mysql_driver_manager_->add_mysql_server(server_id, dns, ip, port, db_name, db_user, db_pwd);
    }

    return true;
}

bool mysql_module_impl::shut() {
    return true;
}

bool mysql_module_impl::post_init() {
    return true;
}

bool mysql_module_impl::update() {
    if (last_check_time_ + 10 > get_plugin_manager()->get_now_time())
    {
        return true;
    }

    last_check_time_ = get_plugin_manager()->get_now_time();

    if (mysql_driver_manager_ != nullptr)
    {
        mysql_driver_manager_->check_mysql();
    }
    return true;
}

bool mysql_module_impl::add_mysql_server(const int server_id, const std::string& dns, const std::string& ip,
    const int port, const std::string db_name, const std::string db_user, const std::string db_pwd,
    const int reconnect_time /* = 10*/, const int reconnect_count /* = -1*/) {
    if (!mysql_driver_manager_)
    {
        return false;
    }

    return mysql_driver_manager_->add_mysql_server(
        server_id, dns, ip, port, db_name, db_user, db_pwd, reconnect_time, reconnect_count);
}

bool mysql_module_impl::update_or_insert(const std::string& record_name, const std::string& key,
    const std::vector<std::string>& field_vec, const std::vector<std::string>& value_vec) {
    mysql_driver_module* driver_ptr = mysql_driver_manager_->get_mysql_driver();
    if (driver_ptr)
    {
        return driver_ptr->update_or_insert(record_name, key, field_vec, value_vec);
    }

    return false;
}

bool mysql_module_impl::query(const std::string& record_name, const std::string& key,
    const std::vector<std::string>& field_vec, std::vector<std::string>& value_vec) {
    mysql_driver_module* driver_ptr = mysql_driver_manager_->get_mysql_driver();
    if (driver_ptr)
    {
        return driver_ptr->query(record_name, key, field_vec, value_vec);
    }

    return false;
}

bool mysql_module_impl::erase(const std::string& record_name, const std::string& key) {
    mysql_driver_module* driver_ptr = mysql_driver_manager_->get_mysql_driver();
    if (driver_ptr)
    {
        return driver_ptr->erase(record_name, key);
    }

    return false;
}

bool mysql_module_impl::exists(const std::string& record_name, const std::string& key, bool& exists) {
    mysql_driver_module* driver_ptr = mysql_driver_manager_->get_mysql_driver();
    if (driver_ptr)
    {
        return driver_ptr->exists(record_name, key, exists);
    }

    return false;
}

bool mysql_module_impl::select(const std::string& record_name, const std::string& key,
    const std::vector<std::string>& field_vec, std::vector<std::string>& value_vec) {
    mysql_driver_module* driver_ptr = mysql_driver_manager_->get_mysql_driver();
    if (driver_ptr)
    {
        return driver_ptr->select(record_name, key, field_vec, value_vec);
    }

    return false;
}

bool mysql_module_impl::keys(
    const std::string& record_name, const std::string& key_name, std::vector<std::string>& value_vec) {
    mysql_driver_module* driver_ptr = mysql_driver_manager_->get_mysql_driver();
    if (driver_ptr)
    {
        return driver_ptr->keys(record_name, key_name, value_vec);
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool mysql_module_impl::insert(const std::string& document, std::vector<kv_pair>& data) {
    mysql_driver_module* driver_ptr = mysql_driver_manager_->get_mysql_driver();
    if (driver_ptr)
    {
        return driver_ptr->insert(document, data);
    }

    return false;
}

bool mysql_module_impl::update_record(
    const std::string& document, std::vector<kv_pair>& data, std::vector<where>& where) {
    mysql_driver_module* driver_ptr = mysql_driver_manager_->get_mysql_driver();
    if (driver_ptr)
    {
        return driver_ptr->update_record(document, data, where);
    }

    return false;
}

bool mysql_module_impl::select(const std::string& document, std::vector<std::string>& columns,
    std::vector<where>& where, order_by& order, int limit, result_arr& results) {
    mysql_driver_module* driver_ptr = mysql_driver_manager_->get_mysql_driver();
    if (driver_ptr)
    {
        return driver_ptr->select(document, columns, where, order, limit, results);
    }

    return false;
}

bool mysql_module_impl::select_one(const std::string& document, std::vector<std::string>& columns,
    std::vector<where>& where, order_by& order, result& result) {
    mysql_driver_module* driver_ptr = mysql_driver_manager_->get_mysql_driver();
    if (driver_ptr)
    {
        return driver_ptr->select_one(document, columns, where, order, result);
    }

    return false;
}
} // namespace ark
