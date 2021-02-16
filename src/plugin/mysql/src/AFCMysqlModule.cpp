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

#include "mysql/include/AFCMysqlModule.hpp"
#include "mysql/include/AFCMysqlDriverModule.hpp"
#include "mysql/include/AFCMysqlDriverManagerModule.hpp"
#include "mysql/include/AFMysqlPlugin.hpp"

namespace ark {

AFCMysqlModule::AFCMysqlModule() {}

AFCMysqlModule::~AFCMysqlModule() {}

bool AFCMysqlModule::Init()
{
    mysql_driver_manager = std::make_shared<AFCMysqlDriverManagerModule>();

    AFXml xml_doc(AFMysqlPlugin::GetPluginConf());
    auto root_node = xml_doc.GetRootNode();
    ARK_ASSERT_RET_VAL(root_node.IsValid(), false);

    auto mysql_node = root_node.FindNode("mysql");
    ARK_ASSERT_RET_VAL(mysql_node.IsValid(), false);

    for (auto instance_node = mysql_node.FindNode("instance"); instance_node.IsValid(); instance_node.NextNode())
    {
        auto server_id = instance_node.GetUint32("id");
        auto dns = instance_node.GetString("dns");
        auto ip = instance_node.GetString("ip");
        auto port = instance_node.GetUint32("port");
        auto db_name = instance_node.GetString("dbname");
        auto db_user = instance_node.GetString("dbuser");
        auto db_pwd = instance_node.GetString("dbpwd");

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

bool AFCMysqlModule::Shut()
{
    return true;
}

bool AFCMysqlModule::PostInit()
{
    return true;
}

bool AFCMysqlModule::Update()
{
    if (last_check_time_ + 10 > GetPluginManager()->GetNowTime())
    {
        return true;
    }

    last_check_time_ = GetPluginManager()->GetNowTime();

    if (mysql_driver_manager != nullptr)
    {
        mysql_driver_manager->CheckMySQL();
    }
    return true;
}

bool AFCMysqlModule::AddMysqlServer(const int nServerID, const std::string& strDns, const std::string& strIP,
    const int nPort, const std::string strDBName, const std::string strDBUser, const std::string strDBPwd,
    const int nRconnectTime /* = 10*/, const int nRconneCount /* = -1*/)
{
    if (!mysql_driver_manager)
    {
        return false;
    }

    return mysql_driver_manager->AddMySQLServer(
        nServerID, strDns, strIP, nPort, strDBName, strDBUser, strDBPwd, nRconnectTime, nRconneCount);
}

bool AFCMysqlModule::UpdateOrInsert(const std::string& strRecordName, const std::string& strKey,
    const std::vector<std::string>& fieldVec, const std::vector<std::string>& valueVec)
{
    AFIMysqlDriverModule* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->UpdateOrInsert(strRecordName, strKey, fieldVec, valueVec);
    }

    return false;
}

bool AFCMysqlModule::Query(const std::string& strRecordName, const std::string& strKey,
    const std::vector<std::string>& fieldVec, std::vector<std::string>& valueVec)
{
    AFIMysqlDriverModule* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Query(strRecordName, strKey, fieldVec, valueVec);
    }

    return false;
}

bool AFCMysqlModule::Delete(const std::string& strRecordName, const std::string& strKey)
{
    AFIMysqlDriverModule* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Delete(strRecordName, strKey);
    }

    return false;
}

bool AFCMysqlModule::Exists(const std::string& strRecordName, const std::string& strKey, bool& bExit)
{
    AFIMysqlDriverModule* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Exists(strRecordName, strKey, bExit);
    }

    return false;
}

bool AFCMysqlModule::Select(const std::string& strRecordName, const std::string& strKey,
    const std::vector<std::string>& fieldVec, std::vector<std::string>& valueVec)
{
    AFIMysqlDriverModule* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Select(strRecordName, strKey, fieldVec, valueVec);
    }

    return false;
}

bool AFCMysqlModule::Keys(
    const std::string& strRecordName, const std::string& strKeyName, std::vector<std::string>& valueVec)
{
    AFIMysqlDriverModule* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Keys(strRecordName, strKeyName, valueVec);
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AFCMysqlModule::Insert(const std::string& document, std::vector<KVPair>& data)
{
    AFIMysqlDriverModule* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Insert(document, data);
    }

    return false;
}
bool AFCMysqlModule::UpdateRecord(const std::string& document, std::vector<KVPair>& data, std::vector<where>& where)
{
    AFIMysqlDriverModule* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->UpdateRecord(document, data, where);
    }

    return false;
}
bool AFCMysqlModule::Select(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
    OrderBy& order, int limit, ResultArr& results)
{
    AFIMysqlDriverModule* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->Select(document, columns, where, order, limit, results);
    }

    return false;
}
bool AFCMysqlModule::SelectOne(const std::string& document, std::vector<std::string>& columns,
    std::vector<where>& where, OrderBy& order, Result& result)
{
    AFIMysqlDriverModule* pDriver = mysql_driver_manager->GetMySQLDriver();
    if (pDriver)
    {
        return pDriver->SelectOne(document, columns, where, order, result);
    }

    return false;
}
} // namespace ark
