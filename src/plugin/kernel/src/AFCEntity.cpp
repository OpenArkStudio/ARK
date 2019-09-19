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
#include "kernel/include/AFCStaticEntity.hpp"
#include "kernel/include/AFCEventManager.hpp"
#include "kernel/include/AFCContainerManager.hpp"

namespace ark {

AFCEntity::AFCEntity(ARK_SHARE_PTR<AFClassMeta> pClassMeta, const AFGUID& guid, const ID_TYPE config_id,
    const int32_t map, const int32_t map_entity_id)
    : guid_(guid)
    , config_id_(config_id)
    , map_id_(map)
    , map_entity_id_(map_entity_id)
{
    // data node
    auto func = std::bind(&AFCEntity::OnDataCallBack, this, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4);
    m_pNodeManager = std::make_shared<AFNodeManager>(pClassMeta, std::move(func));

    // data table
    m_pTableManager = std::make_shared<AFTableManager>(pClassMeta);

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
    auto index = m_pNodeManager->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return IsPublic(index);
}

bool AFCEntity::IsPublic(const uint32_t index)
{
    return HaveMask(index, ArkNodeMask::PF_PUBLIC);
}

bool AFCEntity::IsPrivate(const std::string& name)
{
    auto index = m_pNodeManager->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return IsPrivate(index);
}

bool AFCEntity::IsPrivate(const uint32_t index)
{
    return HaveMask(index, ArkNodeMask::PF_PRIVATE);
}

bool AFCEntity::IsSave(const std::string& name)
{
    auto index = m_pNodeManager->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return IsSave(index);
}

bool AFCEntity::IsSave(const uint32_t index)
{
    return HaveMask(index, ArkNodeMask::PF_SAVE);
}

bool AFCEntity::IsRealTime(const std::string& name)
{
    auto index = m_pNodeManager->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return IsRealTime(index);
}

bool AFCEntity::IsRealTime(const uint32_t index)
{
    return HaveMask(index, ArkNodeMask::PF_REAL_TIME);
}

bool AFCEntity::HaveMask(const std::string& name, ArkNodeMask feature)
{
    auto index = m_pNodeManager->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return HaveMask(index, feature);
}

bool AFCEntity::HaveMask(const uint32_t index, ArkNodeMask feature)
{
    AFINode* p = m_pNodeManager->GetNode(index);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    return p->HaveMask(feature);
}

ArkMaskType AFCEntity::GetMask(const uint32_t index) const
{
    AFINode* p = m_pNodeManager->GetNode(index);

    ARK_ASSERT_RET_VAL(p != nullptr, 0);

    return p->GetMask();
}

const std::string& AFCEntity::GetClassName() const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_STR);

    return m_pNodeManager->GetClassName();
}

ID_TYPE AFCEntity::GetConfigID() const
{
    return config_id_;
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
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr && pStaticObject != nullptr, false);

    // static object should be empty
    ARK_ASSERT_RET_VAL(m_pNodeManager->IsEmpty(), false);

    // should be a same class
    ARK_ASSERT_RET_VAL(m_pNodeManager->GetClassName() == pStaticObject->GetClassName(), false);

    auto pStaticEntity = std::dynamic_pointer_cast<AFCStaticEntity>(pStaticObject);
    ARK_ASSERT_RET_VAL(pStaticEntity != nullptr, false);

    auto pStaticComponent = pStaticEntity->m_pNodeManager;
    ARK_ASSERT_RET_VAL(pStaticComponent != nullptr, false);

    auto& data_list = pStaticComponent->GetDataList();
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
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetBool(name, value);
}

bool AFCEntity::SetInt32(const std::string& name, const int32_t value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetInt32(name, value);
}

bool AFCEntity::SetUInt32(const std::string& name, const uint32_t value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetUInt32(name, value);
}

bool AFCEntity::SetInt64(const std::string& name, const int64_t value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetInt64(name, value);
}

bool AFCEntity::SetUInt64(const std::string& name, const uint64_t value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetUInt64(name, value);
}

bool AFCEntity::SetFloat(const std::string& name, const float value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetFloat(name, value);
}

bool AFCEntity::SetDouble(const std::string& name, const double value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetDouble(name, value);
}

bool AFCEntity::SetString(const std::string& name, const std::string& value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetString(name, value);
}

bool AFCEntity::SetWString(const std::string& name, const std::wstring& value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetWString(name, value);
}

bool AFCEntity::SetGUID(const std::string& name, const AFGUID& value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetGUID(name, value);
}

bool AFCEntity::SetBool(const uint32_t index, bool value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetBool(index, value);
}

bool AFCEntity::SetInt32(const uint32_t index, const int32_t value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetInt32(index, value);
}

bool AFCEntity::SetUInt32(const uint32_t index, const uint32_t value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetUInt32(index, value);
}

bool AFCEntity::SetInt64(const uint32_t index, const int64_t value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetInt64(index, value);
}

bool AFCEntity::SetUInt64(const uint32_t index, const uint64_t value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetUInt64(index, value);
}

bool AFCEntity::SetFloat(const uint32_t index, const float value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetFloat(index, value);
}

bool AFCEntity::SetDouble(const uint32_t index, const double value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetDouble(index, value);
}

bool AFCEntity::SetString(const uint32_t index, const std::string& value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetString(index, value);
}

bool AFCEntity::SetWString(const uint32_t index, const std::wstring& value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetWString(index, value);
}

bool AFCEntity::SetGUID(const uint32_t index, const AFGUID& value)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->SetGUID(index, value);
}

AFINode* AFCEntity::GetNode(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, nullptr);

    return m_pNodeManager->GetNode(name);
}

// get data
bool AFCEntity::GetBool(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_BOOLEAN);

    return m_pNodeManager->GetBool(name);
}

int32_t AFCEntity::GetInt32(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->GetInt32(name);
}

uint32_t AFCEntity::GetUInt32(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->GetUInt32(name);
}

int64_t AFCEntity::GetInt64(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->GetInt64(name);
}

uint64_t AFCEntity::GetUInt64(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->GetUInt64(name);
}

float AFCEntity::GetFloat(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_FLOAT);

    return m_pNodeManager->GetFloat(name);
}

double AFCEntity::GetDouble(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_DOUBLE);

    return m_pNodeManager->GetDouble(name);
}

const std::string& AFCEntity::GetString(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_STR);

    return m_pNodeManager->GetString(name);
}

const std::wstring& AFCEntity::GetWString(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_WIDESTR);

    return m_pNodeManager->GetWString(name);
}

const AFGUID& AFCEntity::GetGUID(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_GUID);

    return m_pNodeManager->GetGUID(name);
}

AFINode* AFCEntity::GetNode(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, nullptr);

    return m_pNodeManager->GetNode(index);
}

bool AFCEntity::GetBool(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_BOOLEAN);

    return m_pNodeManager->GetBool(index);
}

int32_t AFCEntity::GetInt32(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->GetInt32(index);
}

uint32_t AFCEntity::GetUInt32(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->GetUInt32(index);
}

int64_t AFCEntity::GetInt64(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->GetInt64(index);
}

uint64_t AFCEntity::GetUInt64(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->GetUInt64(index);
}

float AFCEntity::GetFloat(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_FLOAT);

    return m_pNodeManager->GetFloat(index);
}

double AFCEntity::GetDouble(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_DOUBLE);

    return m_pNodeManager->GetDouble(index);
}

const std::string& AFCEntity::GetString(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_STR);

    return m_pNodeManager->GetString(index);
}

const std::wstring& AFCEntity::GetWString(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_WIDESTR);

    return m_pNodeManager->GetWString(index);
}

const AFGUID& AFCEntity::GetGUID(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_GUID);

    return m_pNodeManager->GetGUID(index);
}

// container operation
ARK_SHARE_PTR<AFIContainer> AFCEntity::FindContainer(const std::string& name)
{
    auto index = m_pNodeManager->GetIndex(name);
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
        auto pClassMeta = m_pNodeManager->GetClassMeta();
        pContainer = m_pContainerManager->CreateContainer(pClassMeta, index, guid_);
    }

    return pContainer;
}

// table data
AFITable* AFCEntity::FindTable(const std::string& name)
{
    auto index = m_pNodeManager->GetIndex(name);
    ARK_ASSERT_RET_VAL(index > 0, nullptr);

    return FindTable(index);
}

AFITable* AFCEntity::FindTable(const uint32_t index)
{
    auto pTable = m_pTableManager->FindTable(index);
    if (nullptr == pTable)
    {
        pTable = m_pTableManager->AddTable(guid_, index);
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
    auto pNewData = m_pNodeManager->CreateData(pData->GetMeta());
    ARK_ASSERT_RET_VAL(pNewData != nullptr, false);

    pNewData->CopyFrom(pData);

    return true;
}

int AFCEntity::OnDataCallBack(
    const std::string& name, const uint32_t index, const AFIData& old_data, const AFIData& new_data)
{
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr, 0);

    m_pCallBackManager->OnDataCallBack(guid_, name, index, old_data, new_data);

    return 0;
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
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, nullptr);

    return m_pNodeManager->First();
}

AFINode* AFCEntity::NextNode()
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, nullptr);

    return m_pNodeManager->Next();
}

AFITable* AFCEntity::FirstTable()
{
    ARK_ASSERT_RET_VAL(m_pTableManager != nullptr, nullptr);

    return m_pTableManager->First();
}

AFITable* AFCEntity::NextTable()
{
    ARK_ASSERT_RET_VAL(m_pTableManager != nullptr, nullptr);

    return m_pTableManager->Next();
}

ARK_SHARE_PTR<AFIContainer> AFCEntity::FirstContainer()
{
    ARK_ASSERT_RET_VAL(m_pContainerManager != nullptr, nullptr);

    return m_pContainerManager->First();
}

ARK_SHARE_PTR<AFIContainer> AFCEntity::NextContainer()
{
    ARK_ASSERT_RET_VAL(m_pContainerManager != nullptr, nullptr);

    return m_pContainerManager->Next();
}

bool AFCEntity::IsSent() const
{
    return is_sent_;
}

void AFCEntity::UpdateSent()
{
    is_sent_ = true;
}

} // namespace ark
