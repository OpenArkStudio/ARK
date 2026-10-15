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

mysql_module_impl::mysql_module_impl() {}

mysql_module_impl::~mysql_module_impl() {}

bool mysql_module_impl::init() {
    mysql_driver_manager = std::make_shared<mysql_driver_manager_module_impl>();

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

        std::cout << "----------\n";
        std::cout << server_id << "\n"
                  << dns << "\n"
                  << ip << "\n"
                  << port << "\n"
                  << db_name << "\n"
                  << db_user << "\n"
                  << db_pwd << "\n";
        std::cout << "----------\n";

        mysql_driver_manager->AddMySQLServer(server_id, dns, ip, port, db_name, db_user, db_pwd);
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

    if (mysql_driver_manager != nullptr)
    {
        mysql_driver_manager->CheckMySQL();
    }
    return true;
}

bool mysql_module_impl::AddMysqlServer(const int nServerID, const std::string& strDns, const std::string& strIP,
    const int nPort, const std::string strDBName, const std::string strDBUser, const std::string strDBPwd,
    const int nRconnectTime /* = 10*/, const int nRconneCount /* = -1*/) {
    if (!mysql_driver_manager)
    {
        return false;
    }

    return mysql_driver_manager->AddMySQLServer(
        nServerID, strDns, strIP, nPort, strDBName, strDBUser, strDBPwd, nRconnectTime, nRconneCount);
}

bool mysql_module_impl::UpdateOrInsert(const std::string& strRecordName, const std::string& strKey,
    const std::vector<std::string>& fieldVec, const std::vector<std::string>& valueVec) {
    mysql_driver_module* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->UpdateOrInsert(strRecordName, strKey, fieldVec, valueVec);
    }

    return false;
}

bool mysql_module_impl::Query(const std::string& strRecordName, const std::string& strKey,
    const std::vector<std::string>& fieldVec, std::vector<std::string>& valueVec) {
    mysql_driver_module* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Query(strRecordName, strKey, fieldVec, valueVec);
    }

    return false;
}

bool mysql_module_impl::Delete(const std::string& strRecordName, const std::string& strKey) {
    mysql_driver_module* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Delete(strRecordName, strKey);
    }

    return false;
}

bool mysql_module_impl::Exists(const std::string& strRecordName, const std::string& strKey, bool& bExit) {
    mysql_driver_module* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Exists(strRecordName, strKey, bExit);
    }

    return false;
}

bool mysql_module_impl::Select(const std::string& strRecordName, const std::string& strKey,
    const std::vector<std::string>& fieldVec, std::vector<std::string>& valueVec) {
    mysql_driver_module* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Select(strRecordName, strKey, fieldVec, valueVec);
    }

    return false;
}

bool mysql_module_impl::Keys(
    const std::string& strRecordName, const std::string& strKeyName, std::vector<std::string>& valueVec) {
    mysql_driver_module* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Keys(strRecordName, strKeyName, valueVec);
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool mysql_module_impl::Insert(const std::string& document, std::vector<kv_pair>& data) {
    mysql_driver_module* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Insert(document, data);
    }

    return false;
}

bool mysql_module_impl::UpdateRecord(
    const std::string& document, std::vector<kv_pair>& data, std::vector<where>& where) {
    mysql_driver_module* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->UpdateRecord(document, data, where);
    }

    return false;
}

bool mysql_module_impl::Select(const std::string& document, std::vector<std::string>& columns,
    std::vector<where>& where, order_by& order, int limit, result_arr& results) {
    mysql_driver_module* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Select(document, columns, where, order, limit, results);
    }

    return false;
}

bool mysql_module_impl::SelectOne(const std::string& document, std::vector<std::string>& columns,
    std::vector<where>& where, order_by& order, result& result) {
    mysql_driver_module* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->SelectOne(document, columns, where, order, result);
    }

    return false;
}
} // namespace ark
