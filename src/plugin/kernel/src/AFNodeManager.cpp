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

#include "kernel/include/AFCNode.hpp"
#include "kernel/include/AFCData.hpp"
#include "kernel/include/AFNodeManager.hpp"

namespace ark {

AFNodeManager::AFNodeManager(ARK_SHARE_PTR<AFClassMeta> pClassMeta)
{
    class_meta_ = pClassMeta;
}

AFNodeManager::AFNodeManager(ARK_SHARE_PTR<AFClassMeta> pClassMeta, NODE_MANAGER_FUNCTOR&& func)
{
    class_meta_ = pClassMeta;
    func_ = std::forward<NODE_MANAGER_FUNCTOR>(func);
}

bool AFNodeManager::IsEmpty() const
{
    return data_list_.size() == 0;
}

// data operation
AFINode* AFNodeManager::CreateData(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
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

// this is only called when create new entity so do no need check whether node exist
bool AFNodeManager::CreateData(AFINode* pData)
{
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    auto pNewData = CreateDataByMeta(pData->GetMeta());
    ARK_ASSERT_RET_VAL(pNewData != nullptr, false);

    pNewData->CopyFrom(pData);

    if (!data_list_.insert(pNewData->GetIndex(), pNewData).second)
    {
        ARK_DELETE(pNewData);
        return false;
    }

    return true;
}

// get node
AFINode* AFNodeManager::GetNode(const std::string& name) const
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, nullptr);

    return GetNode(index);
}

AFINode* AFNodeManager::GetNode(const uint32_t index) const
{
    return data_list_.find_value(index);
}

// query data
bool AFNodeManager::GetBool(const std::string& name) const
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_BOOLEAN);

    return GetBool(index);
}

int32_t AFNodeManager::GetInt32(const std::string& name) const
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_INT);

    return GetInt32(index);
}

uint32_t AFNodeManager::GetUInt32(const std::string& name) const
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_INT);

    return GetUInt32(index);
}

int64_t AFNodeManager::GetInt64(const std::string& name) const
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_INT64);

    return GetInt64(index);
}

uint64_t AFNodeManager::GetUInt64(const std::string& name) const
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_INT64);

    return GetInt64(index);
}

float AFNodeManager::GetFloat(const std::string& name) const
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_FLOAT);

    return GetFloat(index);
}

double AFNodeManager::GetDouble(const std::string& name) const
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_DOUBLE);

    return GetDouble(index);
}

const std::string& AFNodeManager::GetString(const std::string& name) const
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_STR);

    return GetString(index);
}

const std::wstring& AFNodeManager::GetWString(const std::string& name) const
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_WIDESTR);

    return GetWString(index);
}

const AFGUID& AFNodeManager::GetGUID(const std::string& name) const
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_GUID);

    return GetGUID(index);
}

bool AFNodeManager::GetBool(const uint32_t index) const
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_BOOLEAN);

    return pData->GetBool();
}

int32_t AFNodeManager::GetInt32(const uint32_t index) const
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->GetInt32();
}

uint32_t AFNodeManager::GetUInt32(const uint32_t index) const
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->GetUInt32();
}

int64_t AFNodeManager::GetInt64(const uint32_t index) const
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT64);

    return pData->GetInt64();
}

uint64_t AFNodeManager::GetUInt64(const uint32_t index) const
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT64);

    return pData->GetUInt64();
}

float AFNodeManager::GetFloat(const uint32_t index) const
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_FLOAT);

    return pData->GetFloat();
}

double AFNodeManager::GetDouble(const uint32_t index) const
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_DOUBLE);

    return pData->GetDouble();
}

const std::string& AFNodeManager::GetString(const uint32_t index) const
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_STR);

    return pData->GetString();
}

const std::wstring& AFNodeManager::GetWString(const uint32_t index) const
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_WIDESTR);

    return pData->GetWString();
}

const AFGUID& AFNodeManager::GetGUID(const uint32_t index) const
{
    auto pData = FindData(index);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_GUID);

    return pData->GetObject();
}

// set data
bool AFNodeManager::SetBool(const std::string& name, bool value)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetBool(index, value);
}

bool AFNodeManager::SetInt32(const std::string& name, const int32_t value)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetInt32(index, value);
}

bool AFNodeManager::SetUInt32(const std::string& name, const uint32_t value)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetUInt32(index, value);
}

bool AFNodeManager::SetInt64(const std::string& name, const int64_t value)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetInt64(index, value);
}

bool AFNodeManager::SetUInt64(const std::string& name, const uint64_t value)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetUInt64(index, value);
}

bool AFNodeManager::SetFloat(const std::string& name, const float value)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetFloat(index, value);
}

bool AFNodeManager::SetDouble(const std::string& name, const double value)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetDouble(index, value);
}

bool AFNodeManager::SetString(const std::string& name, const std::string& value)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetString(index, value);
}

bool AFNodeManager::SetWString(const std::string& name, const std::wstring& value)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetWString(index, value);
}

bool AFNodeManager::SetGUID(const std::string& name, const AFGUID& value)
{
    auto index = GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetGUID(index, value);
}

bool AFNodeManager::SetBool(const uint32_t index, bool value)
{
    AFINode* pData = FindData(index);

    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_BOOLEAN, false);

    auto old_value = pData->GetBool();
    pData->SetBool(value);

    if (old_value != value && func_)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        func_(pData->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFNodeManager::SetInt32(const uint32_t index, const int32_t value)
{
    AFINode* pData = FindData(index);

    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_INT32, false);

    auto old_value = pData->GetInt32();
    pData->SetInt32(value);

    if (old_value != value && func_)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        func_(pData->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFNodeManager::SetUInt32(const uint32_t index, const uint32_t value)
{
    AFINode* pData = FindData(index);

    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_UINT32, false);

    auto old_value = pData->GetUInt32();
    pData->SetUInt32(value);

    if (old_value != value && func_)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        func_(pData->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFNodeManager::SetInt64(const uint32_t index, const int64_t value)
{
    AFINode* pData = FindData(index);

    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_INT64, false);

    auto old_value = pData->GetInt64();
    pData->SetInt64(value);

    if (old_value != value && func_)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        func_(pData->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFNodeManager::SetUInt64(const uint32_t index, const uint64_t value)
{
    AFINode* pData = FindData(index);

    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_INT64, false);

    auto old_value = pData->GetUInt64();
    pData->SetUInt64(value);

    if (old_value != value && func_)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        func_(pData->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFNodeManager::SetFloat(const uint32_t index, const float value)
{
    AFINode* pData = FindData(index);

    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_FLOAT, false);

    auto old_value = pData->GetFloat();
    pData->SetFloat(value);

    if (old_value != value && func_)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        func_(pData->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFNodeManager::SetDouble(const uint32_t index, const double value)
{
    AFINode* pData = FindData(index);

    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_DOUBLE, false);

    auto old_value = pData->GetDouble();
    pData->SetDouble(value);

    if (old_value != value && func_)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        func_(pData->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFNodeManager::SetString(const uint32_t index, const std::string& value)
{
    AFINode* pData = FindData(index);

    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_STRING, false);

    auto old_value = pData->GetString();
    pData->SetString(value);

    if (old_value != value && func_)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        func_(pData->GetName(), index, AFCData(data_type, old_value.c_str()), AFCData(data_type, value.c_str()));
    }

    return true;
}

bool AFNodeManager::SetWString(const uint32_t index, const std::wstring& value)
{
    AFINode* pData = FindData(index);

    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_WSTRING, false);

    auto old_value = pData->GetWString();
    pData->SetWString(value);

    if (old_value != value && func_)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        func_(pData->GetName(), index, AFCData(data_type, old_value.c_str()), AFCData(data_type, value.c_str()));
    }

    return true;
}

bool AFNodeManager::SetGUID(const uint32_t index, const AFGUID& value)
{
    AFINode* pData = FindData(index);

    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_GUID, false);

    auto old_value = pData->GetObject();
    pData->SetObject(value);

    if (old_value != value && func_)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        func_(pData->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

// other query
const AFNodeManager::DataList& AFNodeManager::GetDataList() const
{
    return data_list_;
}

uint32_t AFNodeManager::GetIndex(const std::string& name) const
{
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, NULL_INT);

    return class_meta_->GetIndex(name);
}

AFINode* AFNodeManager::FindData(const uint32_t index)
{
    auto pData = data_list_.find_value(index);
    if (nullptr == pData)
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

AFINode* AFNodeManager::FindData(const uint32_t index) const
{
    return data_list_.find_value(index);
}

} // namespace ark
