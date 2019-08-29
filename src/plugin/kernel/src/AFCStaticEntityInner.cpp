/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
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

#include "kernel/include/AFCStaticEntityInner.hpp"
#include "kernel/include/AFCNode.hpp"
#include "kernel/include/AFCTableInner.hpp"
#include "kernel/include/AFClassMeta.hpp"

namespace ark {

AFCStaticEntityInner::AFCStaticEntityInner(ARK_SHARE_PTR<AFClassMeta> pClassMeta, const ID_TYPE_ARG config_id)
    : config_id_(config_id)
{
    class_meta_ = pClassMeta;
}

AFCStaticEntityInner::~AFCStaticEntityInner()
{
    Reset();
}

uint32_t AFCStaticEntityInner::GetIndex(const std::string& name)
{
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, NULL_INT);

    return class_meta_->GetIndex(name);
}

// query data
const std::string& AFCStaticEntityInner::GetClassName() const
{
    return class_meta_->GetName();
}

const ID_TYPE_ARG AFCStaticEntityInner::GetConfigID() const
{
    return config_id_;
}

bool AFCStaticEntityInner::GetBool(const std::string& name)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_BOOLEAN);

    return GetBool(index);
}

int32_t AFCStaticEntityInner::GetInt32(const std::string& name)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_INT);

    return GetInt32(index);
}

uint32_t AFCStaticEntityInner::GetUInt32(const std::string& name)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_INT);

    return GetUInt32(index);
}

int64_t AFCStaticEntityInner::GetInt64(const std::string& name)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_INT64);

    return GetInt64(index);
}

float AFCStaticEntityInner::GetFloat(const std::string& name)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_FLOAT);

    return GetFloat(index);
}

double AFCStaticEntityInner::GetDouble(const std::string& name)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_DOUBLE);

    return GetDouble(index);
}

const std::string& AFCStaticEntityInner::GetString(const std::string& name)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_STR);

    return GetString(index);
}

const std::wstring& AFCStaticEntityInner::GetWString(const std::string& name)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_WIDESTR);

    return GetWString(index);
}

bool AFCStaticEntityInner::GetBool(const uint32_t index)
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_BOOLEAN);

    return pData->GetBool();
}

int32_t AFCStaticEntityInner::GetInt32(const uint32_t index)
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->GetInt32();
}

uint32_t AFCStaticEntityInner::GetUInt32(const uint32_t index)
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->GetUInt32();
}

int64_t AFCStaticEntityInner::GetInt64(const uint32_t index)
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT64);

    return pData->GetInt64();
}

float AFCStaticEntityInner::GetFloat(const uint32_t index)
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_FLOAT);

    return pData->GetFloat();
}

double AFCStaticEntityInner::GetDouble(const uint32_t index)
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_DOUBLE);

    return pData->GetDouble();
}

const std::string& AFCStaticEntityInner::GetString(const uint32_t index)
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_STR);

    return pData->GetString();
}

const std::wstring& AFCStaticEntityInner::GetWString(const uint32_t index)
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_WIDESTR);

    return pData->GetWString();
}

// data operation
AFINode* AFCStaticEntityInner::CreateData(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
{
    ARK_ASSERT_RET_VAL(pDataMeta != nullptr, nullptr);

    // has exist
    auto index = pDataMeta->GetIndex();
    ARK_ASSERT_RET_VAL(index > 0, nullptr);
    ARK_ASSERT_RET_VAL(data_list_.find_value(index) == nullptr, nullptr);

    auto pData = CreateDataByMeta(pDataMeta);
    ARK_ASSERT_RET_VAL(pData != nullptr, nullptr);

    if (!data_list_.insert(index, pData).second)
    {
        ARK_DELETE(pData);
        return nullptr;
    }

    return pData;
}

AFINode* AFCStaticEntityInner::FindData(const uint32_t index, bool bCreate /* = false*/)
{
    auto pData = data_list_.find_value(index);
    if (nullptr == pData && bCreate)
    {
        auto pDataMeta = class_meta_->FindDataMeta(index);
        ARK_ASSERT_RET_VAL(pDataMeta != nullptr, nullptr);

        pData = CreateDataByMeta(pDataMeta);
        ARK_ASSERT_RET_VAL(pData != nullptr, nullptr);

        if (!data_list_.insert(index, pData).second)
        {
            ARK_DELETE(pData);
            return nullptr;
        }
    }

    return pData;
}

// table meta operation
AFITable* AFCStaticEntityInner::AddTable(const AFGUID& guid, const uint32_t index)
{
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, nullptr);

    AFITable* pTable = nullptr;

    auto iter = table_list_.find(index);
    if (iter != table_list_.end())
    {
        return iter->second;
    }

    auto pTableMeta = class_meta_->FindTableMeta(index);
    ARK_ASSERT_RET_VAL(pTableMeta != nullptr, nullptr);

    pTable = ARK_NEW AFCTableInner(pTableMeta, class_meta_->GetClassCallBackManager(), guid);
    ARK_ASSERT_RET_VAL(pTable != nullptr, nullptr);

    if (!table_list_.insert(index, pTable).second)
    {
        ARK_DELETE(pTable);
        return nullptr;
    }

    return pTable;
}

AFITable* AFCStaticEntityInner::FindTable(const uint32_t index)
{
    auto iter = table_list_.find(index);
    if (iter == table_list_.end())
    {
        return nullptr;
    }

    return iter->second;
}

void AFCStaticEntityInner::Reset()
{
    // clear data
    data_list_.removeall();
    table_list_.removeall();
}

bool AFCStaticEntityInner::IsEmpty() const
{
    if (data_list_.size() > 0 || table_list_.size() > 0)
    {
        return false;
    }

    return true;
}

const AFCStaticEntityInner::DataList& AFCStaticEntityInner::GetDataList()
{
    return data_list_;
}

ARK_SHARE_PTR<AFClassMeta> AFCStaticEntityInner::GetClassMeta()
{
    return class_meta_;
}

} // namespace ark
