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

#include "kernel/include/AFCEntity.hpp"
#include "kernel/include/AFCData.hpp"
#include "kernel/include/AFCContainer.hpp"
#include "kernel/include/AFCStaticEntityInner.hpp"
#include "kernel/include/AFCEventManager.hpp"
#include "kernel/include/AFCContainerManager.hpp"

namespace ark {

AFCEntity::AFCEntity(ARK_SHARE_PTR<AFClassMeta> pClassMeta, const AFGUID& guid, const ID_TYPE config_id,
    const int32_t map, const int32_t map_entity_id)
    : guid_(guid)
    , map_id_(map)
    , map_entity_id_(map_entity_id)
{
    m_pStaticObject = std::make_shared<AFCStaticEntityInner>(pClassMeta, config_id);
    m_pContainerManager = std::make_shared<AFCContainerManager>();
    m_pEventManager = std::make_shared<AFCEventManager>(guid_);
    m_pCallBackManager = pClassMeta->GetClassCallBackManager();
}

void AFCEntity::Update()
{
    //
}

// get unique id
const AFGUID& AFCEntity::GetID() const
{
    return guid_;
}

ARK_SHARE_PTR<AFIContainer> AFCEntity::GetParentContainer() const
{
    return parent_container_;
}

bool AFCEntity::SetParentContainer(ARK_SHARE_PTR<AFIContainer> pContainer)
{
    parent_container_ = pContainer;
    return true;
}

bool AFCEntity::IsPublic(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return IsPublic(index);
}

bool AFCEntity::IsPublic(const uint32_t index)
{
    return HaveFeature(index, AFNodeFeature::PF_PUBLIC);
}

bool AFCEntity::IsPrivate(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return IsPrivate(index);
}

bool AFCEntity::IsPrivate(const uint32_t index)
{
    return HaveFeature(index, AFNodeFeature::PF_PRIVATE);
}

bool AFCEntity::IsSave(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return IsSave(index);
}

bool AFCEntity::IsSave(const uint32_t index)
{
    return HaveFeature(index, AFNodeFeature::PF_SAVE);
}

bool AFCEntity::IsRealTime(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return IsRealTime(index);
}

bool AFCEntity::IsRealTime(const uint32_t index)
{
    return HaveFeature(index, AFNodeFeature::PF_REAL_TIME);
}

bool AFCEntity::HaveFeature(const std::string& name, AFNodeFeature feature)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return HaveFeature(index, feature);
}

bool AFCEntity::HaveFeature(const uint32_t index, AFNodeFeature feature)
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    return p->HaveFeature(feature);
}

AFFeatureType AFCEntity::GetFeature(const uint32_t index) const
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    return p->GetFeature();
}

const std::string& AFCEntity::GetClassName() const
{
    ARK_ASSERT_RET_VAL(m_pStaticObject != nullptr, NULL_STR);

    return m_pStaticObject->GetClassName();
}

ID_TYPE AFCEntity::GetConfigID() const
{
    ARK_ASSERT_RET_VAL(m_pStaticObject != nullptr, NULL_INT);

    return m_pStaticObject->GetConfigID();
}

int32_t AFCEntity::GetMapID() const
{
    return map_id_;
}

int32_t AFCEntity::GetMapEntityID() const
{
    return map_entity_id_;
}

bool AFCEntity::SetMapID(const int32_t value)
{
    map_id_ = value;
    return true;
}

bool AFCEntity::SetMapEntityID(const int32_t value)
{
    map_entity_id_ = value;
    return true;
}

bool AFCEntity::InitData(ARK_SHARE_PTR<AFIStaticEntity> pStaticObject)
{
    ARK_ASSERT_RET_VAL(m_pStaticObject != nullptr && pStaticObject != nullptr, false);

    auto pStaticObjectEx = std::dynamic_pointer_cast<AFCStaticEntityInner>(pStaticObject);
    ARK_ASSERT_RET_VAL(pStaticObjectEx != nullptr, false);

    // static object should be empty
    ARK_ASSERT_RET_VAL(m_pStaticObject->IsEmpty(), false);

    // should be a same class
    ARK_ASSERT_RET_VAL(m_pStaticObject->GetClassMeta() == pStaticObjectEx->GetClassMeta(), false);

    auto& data_list = pStaticObjectEx->GetDataList();
    for (auto iter : data_list)
    {
        auto pData = iter.second;
        CopyData(pData);
    }

    return true;
}

// set data
bool AFCEntity::SetBool(const std::string& name, bool value)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetBool(index, value);
}

bool AFCEntity::SetInt32(const std::string& name, const int32_t value)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetInt32(index, value);
}

bool AFCEntity::SetUInt32(const std::string& name, const uint32_t value)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetUInt32(index, value);
}

bool AFCEntity::SetInt64(const std::string& name, const int64_t value)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetInt64(index, value);
}

bool AFCEntity::SetUInt64(const std::string& name, const uint64_t value)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetUInt64(index, value);
}

bool AFCEntity::SetFloat(const std::string& name, const float value)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetFloat(index, value);
}

bool AFCEntity::SetDouble(const std::string& name, const double value)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetDouble(index, value);
}

bool AFCEntity::SetString(const std::string& name, const std::string& value)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetString(index, value);
}

bool AFCEntity::SetWString(const std::string& name, const std::wstring& value)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetWString(index, value);
}

bool AFCEntity::SetGUID(const std::string& name, const AFGUID& value)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return SetGUID(index, value);
}

bool AFCEntity::SetBool(const uint32_t index, bool value)
{
    AFINode* p = m_pStaticObject->FindData(index, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_BOOLEAN, false);

    auto old_value = p->GetBool();
    p->SetBool(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(p->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFCEntity::SetInt32(const uint32_t index, const int32_t value)
{
    AFINode* p = m_pStaticObject->FindData(index, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_INT32, false);

    auto old_value = p->GetInt32();
    p->SetInt32(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(p->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFCEntity::SetUInt32(const uint32_t index, const uint32_t value)
{
    AFINode* p = m_pStaticObject->FindData(index, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_UINT32, false);

    auto old_value = p->GetUInt32();
    p->SetUInt32(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(p->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFCEntity::SetInt64(const uint32_t index, const int64_t value)
{
    AFINode* p = m_pStaticObject->FindData(index, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_INT64, false);

    auto old_value = p->GetInt64();
    p->SetInt64(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(p->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFCEntity::SetUInt64(const uint32_t index, const uint64_t value)
{
    AFINode* p = m_pStaticObject->FindData(index, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_INT64, false);

    auto old_value = p->GetUInt64();
    p->SetUInt64(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(p->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFCEntity::SetFloat(const uint32_t index, const float value)
{
    AFINode* p = m_pStaticObject->FindData(index, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_FLOAT, false);

    auto old_value = p->GetFloat();
    p->SetFloat(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(p->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFCEntity::SetDouble(const uint32_t index, const double value)
{
    AFINode* p = m_pStaticObject->FindData(index, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_DOUBLE, false);

    auto old_value = p->GetDouble();
    p->SetDouble(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(p->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

bool AFCEntity::SetString(const uint32_t index, const std::string& value)
{
    AFINode* p = m_pStaticObject->FindData(index, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_STRING, false);

    auto old_value = p->GetString();
    p->SetString(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(p->GetName(), index, AFCData(data_type, old_value.c_str()), AFCData(data_type, value.c_str()));
    }

    return true;
}

bool AFCEntity::SetWString(const uint32_t index, const std::wstring& value)
{
    AFINode* p = m_pStaticObject->FindData(index, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_WSTRING, false);

    auto old_value = p->GetWString();
    p->SetWString(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(p->GetName(), index, AFCData(data_type, old_value.c_str()), AFCData(data_type, value.c_str()));
    }

    return true;
}

bool AFCEntity::SetGUID(const uint32_t index, const AFGUID& value)
{
    AFINode* p = m_pStaticObject->FindData(index, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_GUID, false);

    auto old_value = p->GetObject();
    p->SetObject(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(p->GetName(), index, AFCData(data_type, old_value), AFCData(data_type, value));
    }

    return true;
}

AFINode* AFCEntity::GetNode(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, nullptr);

    return GetNode(index);
}

// get data
bool AFCEntity::GetBool(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_BOOLEAN);

    return GetBool(index);
}

int32_t AFCEntity::GetInt32(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_INT);

    return GetInt32(index);
}

uint32_t AFCEntity::GetUInt32(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_INT);

    return GetUInt32(index);
}

int64_t AFCEntity::GetInt64(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_INT64);

    return GetInt64(index);
}

uint64_t AFCEntity::GetUInt64(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_INT64);

    return GetUInt64(index);
}

float AFCEntity::GetFloat(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_FLOAT);

    return GetFloat(index);
}

double AFCEntity::GetDouble(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_DOUBLE);

    return GetDouble(index);
}

const std::string& AFCEntity::GetString(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_STR);

    return GetString(index);
}

const std::wstring& AFCEntity::GetWString(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_WIDESTR);

    return GetWString(index);
}

const AFGUID& AFCEntity::GetGUID(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, NULL_GUID);

    return GetGUID(index);
}

AFINode* AFCEntity::GetNode(const uint32_t index)
{
    return m_pStaticObject->FindData(index);
}

bool AFCEntity::GetBool(const uint32_t index)
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_BOOLEAN);

    return p->GetBool();
}

int32_t AFCEntity::GetInt32(const uint32_t index)
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_INT);

    return p->GetInt32();
}

uint32_t AFCEntity::GetUInt32(const uint32_t index)
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_INT);

    return p->GetUInt32();
}

int64_t AFCEntity::GetInt64(const uint32_t index)
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_INT64);

    return p->GetInt64();
}

uint64_t AFCEntity::GetUInt64(const uint32_t index)
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_INT64);

    return p->GetUInt64();
}

float AFCEntity::GetFloat(const uint32_t index)
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_FLOAT);

    return p->GetFloat();
}

double AFCEntity::GetDouble(const uint32_t index)
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_DOUBLE);

    return p->GetDouble();
}

const std::string& AFCEntity::GetString(const uint32_t index)
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_STR);

    return p->GetString();
}

const std::wstring& AFCEntity::GetWString(const uint32_t index)
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_WIDESTR);

    return p->GetWString();
}

const AFGUID& AFCEntity::GetGUID(const uint32_t index)
{
    AFINode* p = m_pStaticObject->FindData(index);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_GUID);

    return p->GetObject();
}

// container operation
ARK_SHARE_PTR<AFIContainer> AFCEntity::FindContainer(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, nullptr);

    return FindContainer(index);
}

ARK_SHARE_PTR<AFIContainer> AFCEntity::FindContainer(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pContainerManager != nullptr, nullptr);

    auto pContainer = m_pContainerManager->FindContainer(index);
    if (pContainer == nullptr)
    {
        // create a new container
        auto pClassMeta = m_pStaticObject->GetClassMeta();
        pContainer = m_pContainerManager->CreateContainer(pClassMeta, index, guid_);
    }

    return pContainer;
}

// table data
AFITable* AFCEntity::FindTable(const std::string& name)
{
    auto index = m_pStaticObject->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, nullptr);

    return FindTable(index);
}

AFITable* AFCEntity::FindTable(const uint32_t index)
{
    auto pTable = m_pStaticObject->FindTable(index);
    if (nullptr == pTable)
    {
        pTable = m_pStaticObject->AddTable(guid_, index);
    }

    return pTable;
}

ARK_SHARE_PTR<AFIEventManager>& AFCEntity::GetEventManager()
{
    return m_pEventManager;
}

// create data new and copy data arg
bool AFCEntity::CopyData(AFINode* pData)
{
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    // do not need check
    auto pNewData = m_pStaticObject->CreateData(pData->GetMeta());
    ARK_ASSERT_RET_VAL(pNewData != nullptr, false);

    pNewData->CopyFrom(pData);

    return true;
}

void AFCEntity::OnDataCallBack(
    const std::string& name, const uint32_t index, const AFIData& old_data, const AFIData& new_data)
{
    ARK_ASSERT_RET_NONE(m_pCallBackManager != nullptr);

    m_pCallBackManager->OnDataCallBack(guid_, name, index, old_data, new_data);
}

bool AFCEntity::AddCustomBool(const std::string& name, bool value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIData* pData = ARK_NEW AFCData;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetBool(value);
    return custom_data_list_.insert(name, pData).second;
}

bool AFCEntity::AddCustomInt32(const std::string& name, const int32_t value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIData* pData = ARK_NEW AFCData;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetInt(value);
    return custom_data_list_.insert(name, pData).second;
}

bool AFCEntity::AddCustomUInt32(const std::string& name, const uint32_t value)
{
    return false;
}

bool AFCEntity::AddCustomInt64(const std::string& name, const int64_t value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIData* pData = ARK_NEW AFCData;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetInt64(value);
    return custom_data_list_.insert(name, pData).second;
}

bool AFCEntity::AddCustomFloat(const std::string& name, const float value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIData* pData = ARK_NEW AFCData;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetFloat(value);
    return custom_data_list_.insert(name, pData).second;
}

bool AFCEntity::AddCustomDouble(const std::string& name, const double value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIData* pData = ARK_NEW AFCData;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetDouble(value);
    return custom_data_list_.insert(name, pData).second;
}

bool AFCEntity::AddCustomString(const std::string& name, const std::string& value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIData* pData = ARK_NEW AFCData;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetString(value.c_str());
    return custom_data_list_.insert(name, pData).second;
}

bool AFCEntity::AddCustomWString(const std::string& name, const std::wstring& value)
{
    return false;
}

bool AFCEntity::AddCustomGUID(const std::string& name, const AFGUID& value)
{
    return false;
}

bool AFCEntity::SetCustomBool(const std::string& name, bool value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_BOOLEAN, false);

    pData->SetBool(value);

    return true;
}

bool AFCEntity::SetCustomInt32(const std::string& name, const int32_t value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_INT32, false);

    pData->SetInt(value);

    return true;
}

bool AFCEntity::SetCustomUInt32(const std::string& name, const uint32_t value)
{
    return false;
}

bool AFCEntity::SetCustomInt64(const std::string& name, const int64_t value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_INT64, false);

    pData->SetInt64(value);

    return true;
}

bool AFCEntity::SetCustomFloat(const std::string& name, const float value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_FLOAT, false);

    pData->SetFloat(value);

    return true;
}

bool AFCEntity::SetCustomDouble(const std::string& name, const double value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_DOUBLE, false);

    pData->SetDouble(value);

    return true;
}

bool AFCEntity::SetCustomString(const std::string& name, const std::string& value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_STRING, false);

    pData->SetString(value.c_str());

    return true;
}

bool AFCEntity::SetCustomWString(const std::string& name, const std::wstring& value)
{
    return false;
}

bool AFCEntity::SetCustomGUID(const std::string& name, const AFGUID& value)
{
    return false;
}

bool AFCEntity::GetCustomBool(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    return pData->GetBool();
}

int32_t AFCEntity::GetCustomInt32(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->GetInt();
}

uint32_t AFCEntity::GetCustomUInt32(const std::string& name)
{
    return NULL_INT;
}

int64_t AFCEntity::GetCustomInt64(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->GetInt64();
}

float AFCEntity::GetCustomFloat(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_FLOAT);

    return pData->GetFloat();
}

double AFCEntity::GetCustomDouble(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_DOUBLE);

    return pData->GetDouble();
}

const char* AFCEntity::GetCustomString(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_STR.c_str());

    return pData->GetString();
}

const std::wstring& AFCEntity::GetCustomWString(const std::string& name)
{
    return NULL_WIDESTR;
}

const AFGUID& AFCEntity::GetCustomGUID(const std::string& name)
{
    return NULL_GUID;
}

bool AFCEntity::FindCustomData(const std::string& name)
{
    return custom_data_list_.find(name) != custom_data_list_.end();
}

bool AFCEntity::RemoveCustomData(const std::string& name)
{
    return custom_data_list_.erase(name);
}

AFINode* AFCEntity::FirstNode()
{
    return m_pStaticObject->FirstNode();
}

AFINode* AFCEntity::NextNode()
{
    return m_pStaticObject->NextNode();
}

AFITable* AFCEntity::FirstTable()
{
    return m_pStaticObject->FirstTable();
}

AFITable* AFCEntity::NextTable()
{
    return m_pStaticObject->NextTable();
}

ARK_SHARE_PTR<AFIContainer> AFCEntity::FirstContainer()
{
    return m_pContainerManager->First();
}

ARK_SHARE_PTR<AFIContainer> AFCEntity::NextContainer()
{
    return m_pContainerManager->Next();
}

} // namespace ark
