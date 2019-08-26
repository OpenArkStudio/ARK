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

#include "kernel/include/AFCStaticObjectInner.hpp"
#include "kernel/include/AFCDataNew.hpp"
#include "kernel/include/AFCRecordInner.hpp"
#include "kernel/include/AFClassMeta.hpp"

namespace ark {

AFCStaticObjectInner::AFCStaticObjectInner(ARK_SHARE_PTR<AFClassMeta> pClassMeta, uint64_t config_id)
    : config_id_(config_id)
{
    class_meta_ = pClassMeta;
}

AFCStaticObjectInner::~AFCStaticObjectInner()
{
    Reset();
}

// query data
const std::string& AFCStaticObjectInner::GetClassName() const
{
    return class_meta_->GetName();
}

uint64_t AFCStaticObjectInner::GetConfigID() const
{
    return config_id_;
}

bool AFCStaticObjectInner::GetBool(const std::string& name)
{
    auto pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_BOOLEAN);

    return pData->GetBool();
}

int32_t AFCStaticObjectInner::GetInt32(const std::string& name)
{
    auto pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->GetInt32();
}

uint32_t AFCStaticObjectInner::GetUInt32(const std::string& name)
{
    auto pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->GetUInt32();
}

int64_t AFCStaticObjectInner::GetInt64(const std::string& name)
{
    auto pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT64);

    return pData->GetInt64();
}

float AFCStaticObjectInner::GetFloat(const std::string& name)
{
    auto pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_FLOAT);

    return pData->GetFloat();
}

double AFCStaticObjectInner::GetDouble(const std::string& name)
{
    auto pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_DOUBLE);

    return pData->GetDouble();
}

const std::string& AFCStaticObjectInner::GetString(const std::string& name)
{
    auto pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_STR);

    return pData->GetString();
}

const std::wstring& AFCStaticObjectInner::GetWString(const std::string& name)
{
    auto pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_WIDESTR);

    return pData->GetWString();
}

// data operation
AFIDataNew* AFCStaticObjectInner::CreateData(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
{
    ARK_ASSERT_RET_VAL(pDataMeta != nullptr, nullptr);

    // has exist
    auto& name = pDataMeta->GetName();
    ARK_ASSERT_RET_VAL(data_list_.find_value(name) == nullptr, nullptr);

    auto pData = CreateDataByMeta(pDataMeta);
    ARK_ASSERT_RET_VAL(pData != nullptr, nullptr);

    if (!data_list_.insert(name, pData).second)
    {
        ARK_DELETE(pData);
        return nullptr;
    }

    return pData;
}

AFIDataNew* AFCStaticObjectInner::FindData(const std::string& name, bool bCreate /* = false*/)
{
    auto pData = data_list_.find_value(name);
    if (nullptr == pData && bCreate)
    {
        auto pDataMeta = class_meta_->FindDataMeta(name);
        ARK_ASSERT_RET_VAL(pDataMeta != nullptr, nullptr);

        pData = CreateDataByMeta(pDataMeta);
        ARK_ASSERT_RET_VAL(pData != nullptr, nullptr);

        if (!data_list_.insert(name, pData).second)
        {
            ARK_DELETE(pData);
            return nullptr;
        }
    }

    return pData;
}

// record meta operation
AFIRecord* AFCStaticObjectInner::AddRecord(const AFGUID& guid, const std::string& name)
{
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, nullptr);

    AFIRecord* pRecord = nullptr;

    auto iter = record_list_.find(name);
    if (iter != record_list_.end())
    {
        return iter->second;
    }

    auto pRecordMeta = class_meta_->FindRecordMeta(name);
    ARK_ASSERT_RET_VAL(pRecordMeta != nullptr, nullptr);

    pRecord = ARK_NEW AFCRecordInner(pRecordMeta, class_meta_->GetClassCallBackManager(), guid);
    ARK_ASSERT_RET_VAL(pRecord != nullptr, nullptr);

    if (!record_list_.insert(name, pRecord).second)
    {
        ARK_DELETE(pRecord);
        return nullptr;
    }

    return pRecord;
}

AFIRecord* AFCStaticObjectInner::FindRecord(const std::string& name)
{
    auto iter = record_list_.find(name);
    if (iter == record_list_.end())
    {
        return nullptr;
    }

    return iter->second;
}

void AFCStaticObjectInner::Reset()
{
    // clear data
    data_list_.removeall();
    record_list_.removeall();
}

bool AFCStaticObjectInner::IsEmpty() const
{
    if (data_list_.size() > 0 || record_list_.size() > 0)
    {
        return false;
    }

    return true;
}

const AFCStaticObjectInner::DataList& AFCStaticObjectInner::GetDataList()
{
    return data_list_;
}

ARK_SHARE_PTR<AFClassMeta> AFCStaticObjectInner::GetClassMeta()
{
    return class_meta_;
}

} // namespace ark
