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

#include "MysqlSampleModule.h"
#include "mysql/interface/AFIMysqlModule.hpp"

namespace ark {

bool MysqlSampleModule::Init()
{
    mysql_module_ptr_ = FindModule<AFIMysqlModule>();
    return true;
}

bool MysqlSampleModule::PostInit()
{
    std::cout << "do something to test mysql plugin \n";
    std::string doc("runoob_tbl");

    // select
    std::cout << "------------------------------------\n";
    struct OrderBy order;
    Result result;
    std::vector<std::string> col{"*"};
    std::vector<where> w{where("runoob_id", "1")};
    if (!mysql_module_ptr_->SelectOne(doc, col, w, order, result))
    {
        std::cout << "select one fail \n";
        return true;
    }

    for (auto iter : result)
    {
        std::cout << iter.first << " - " << iter.second << std::endl;
    }

    // insert
    std::cout << "------------------------------------\n";
    std::vector<KVPair> data{KVPair("runoob_id", "12"), KVPair("runoob_title", "ark"), KVPair("runoob_author", "ark")};
    if (!mysql_module_ptr_->Insert(doc, data))
    {
        std::cout << "insert fail \n";
        return true;
    }

    std::cout << "------------------------------------\n";
    std::vector<KVPair> data1{KVPair("runoob_title", "ark-xxx")};
    std::vector<where> w1{where("runoob_id", "12")};
    if (!mysql_module_ptr_->UpdateRecord(doc, data1, w1))
    {
        std::cout << "update fail \n";
        return true;
    }

    return true;
}

} // namespace ark
