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

#include "kernel/include/AFCObject.hpp"
#include "kernel/include/AFCDataNewType.hpp"
#include "kernel/include/AFCContainer.hpp"
#include "kernel/include/AFCStaticObjectInner.hpp"
#include "kernel/include/AFCEventManager.hpp"
#include "kernel/include/AFCContainerManager.hpp"

namespace ark {

AFCObject::AFCObject(ARK_SHARE_PTR<AFClassMeta> pClassMeta, const AFGUID& guid, const uint64_t config_id,
    const int32_t map, const int32_t map_obj_id)
    : guid_(guid)
    , map_id_(map)
    , map_obj_id_(map_obj_id)
{
    m_pStaticObject = std::make_shared<AFCStaticObjectInner>(pClassMeta, config_id);
    m_pContainerManager = std::make_shared<AFCContainerManager>();
    m_pEventManager = std::make_shared<AFCEventManager>(guid_);
    m_pCallBackManager = pClassMeta->GetClassCallBackManager();
}

AFCObject::~AFCObject()
{
    custom_data_list_.removeall();
}

void AFCObject::Update()
{
    //
}

// get unique id
const AFGUID& AFCObject::GetID() const
{
    return guid_;
}

// get parent unique id
const AFGUID& AFCObject::GetParentID() const
{
    return parent_;
}

bool AFCObject::HaveMask(const std::string& name, ArkDataMaskType mask)
{
    AFIDataNew* p = m_pStaticObject->FindData(name);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    return p->HaveMask(mask);
}

const std::string& AFCObject::GetClassName() const
{
    ARK_ASSERT_RET_VAL(m_pStaticObject != nullptr, NULL_STR);

    return m_pStaticObject->GetClassName();
}

uint64_t AFCObject::GetConfigID() const
{
    ARK_ASSERT_RET_VAL(m_pStaticObject != nullptr, NULL_INT);

    return m_pStaticObject->GetConfigID();
}

int32_t AFCObject::GetMapID() const
{
    return map_id_;
}

int32_t AFCObject::GetMapObjectID() const
{
    return map_obj_id_;
}

bool AFCObject::InitData(ARK_SHARE_PTR<AFIStaticObject> pStaticObject)
{
    ARK_ASSERT_RET_VAL(m_pStaticObject != nullptr && pStaticObject != nullptr, false);

    auto pStaticObjectEx = std::dynamic_pointer_cast<AFCStaticObjectInner>(pStaticObject);
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
bool AFCObject::SetBool(const std::string& name, bool value)
{
    AFIDataNew* p = m_pStaticObject->FindData(name, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_BOOLEAN, false);

    auto old_value = p->GetBool();
    p->SetBool(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

bool AFCObject::SetInt32(const std::string& name, const int32_t value)
{
    AFIDataNew* p = m_pStaticObject->FindData(name, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_INT32, false);

    auto old_value = p->GetInt32();
    p->SetInt32(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

bool AFCObject::SetUInt32(const std::string& name, const uint32_t value)
{
    AFIDataNew* p = m_pStaticObject->FindData(name, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_UINT32, false);

    auto old_value = p->GetUInt32();
    p->SetUInt32(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(name, AFCDataNewType(data_type, (int)old_value), AFCDataNewType(data_type, (int)value));
    }

    return true;
}

bool AFCObject::SetInt64(const std::string& name, const int64_t value)
{
    AFIDataNew* p = m_pStaticObject->FindData(name, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_INT64, false);

    auto old_value = p->GetInt64();
    p->SetInt64(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

bool AFCObject::SetFloat(const std::string& name, const float value)
{
    AFIDataNew* p = m_pStaticObject->FindData(name, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_FLOAT, false);

    auto old_value = p->GetFloat();
    p->SetFloat(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

bool AFCObject::SetDouble(const std::string& name, const double value)
{
    AFIDataNew* p = m_pStaticObject->FindData(name, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_DOUBLE, false);

    auto old_value = p->GetDouble();
    p->SetDouble(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

bool AFCObject::SetString(const std::string& name, const std::string& value)
{
    AFIDataNew* p = m_pStaticObject->FindData(name, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_STRING, false);

    auto old_value = p->GetString();
    p->SetString(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(name, AFCDataNewType(data_type, old_value.c_str()), AFCDataNewType(data_type, value.c_str()));
    }

    return true;
}

bool AFCObject::SetWString(const std::string& name, const std::wstring& value)
{
    AFIDataNew* p = m_pStaticObject->FindData(name, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_WSTRING, false);

    auto old_value = p->GetWString();
    p->SetWString(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(name, AFCDataNewType(data_type, old_value.c_str()), AFCDataNewType(data_type, value.c_str()));
    }

    return true;
}

bool AFCObject::SetObject(const std::string& name, const AFGUID& value)
{
    AFIDataNew* p = m_pStaticObject->FindData(name, true);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    ARK_ASSERT_RET_VAL(p->GetType() == ArkDataType::DT_OBJECT, false);

    auto old_value = p->GetObject();
    p->SetObject(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = p->GetType();
        OnDataCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

// get data
bool AFCObject::GetBool(const std::string& name)
{
    AFIDataNew* p = m_pStaticObject->FindData(name);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_BOOLEAN);

    return p->GetBool();
}

int32_t AFCObject::GetInt32(const std::string& name)
{
    AFIDataNew* p = m_pStaticObject->FindData(name);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_INT);

    return p->GetInt32();
}

uint32_t AFCObject::GetUInt32(const std::string& name)
{
    AFIDataNew* p = m_pStaticObject->FindData(name);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_INT);

    return p->GetUInt32();
}

int64_t AFCObject::GetInt64(const std::string& name)
{
    AFIDataNew* p = m_pStaticObject->FindData(name);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_INT64);

    return p->GetInt64();
}

float AFCObject::GetFloat(const std::string& name)
{
    AFIDataNew* p = m_pStaticObject->FindData(name);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_FLOAT);

    return p->GetFloat();
}

double AFCObject::GetDouble(const std::string& name)
{
    AFIDataNew* p = m_pStaticObject->FindData(name);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_DOUBLE);

    return p->GetDouble();
}

const std::string& AFCObject::GetString(const std::string& name)
{
    AFIDataNew* p = m_pStaticObject->FindData(name);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_STR);

    return p->GetString();
}

const std::wstring& AFCObject::GetWString(const std::string& name)
{
    AFIDataNew* p = m_pStaticObject->FindData(name);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_WIDESTR);

    return p->GetWString();
}

const AFGUID& AFCObject::GetObject(const std::string& name)
{
    AFIDataNew* p = m_pStaticObject->FindData(name);

    ARK_ASSERT_RET_VAL(p != nullptr, NULL_GUID);

    return p->GetObject();
}

// container operation
ARK_SHARE_PTR<AFIContainer> AFCObject::FindContainer(const std::string& name)
{
    ARK_ASSERT_RET_VAL(m_pContainerManager != nullptr, nullptr);

    auto pContainer = m_pContainerManager->FindContainer(name);
    if (pContainer == nullptr)
    {
        // check if need create a new container
        auto pClassMeta = m_pStaticObject->GetClassMeta();
        pContainer = m_pContainerManager->CreateContainer(name, pClassMeta);
    }

    return pContainer;
}

// record data
AFIRecord* AFCObject::FindRecord(const std::string& name)
{
    auto pRecord = m_pStaticObject->FindRecord(name);
    if (nullptr == pRecord)
    {
        pRecord = m_pStaticObject->AddRecord(guid_, name);
    }

    return pRecord;
}

ARK_SHARE_PTR<AFIEventManager>& AFCObject::GetEventManager()
{
    return m_pEventManager;
}

// create data new and copy data arg
bool AFCObject::CopyData(AFIDataNew* pData)
{
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    // do not need check
    auto pNewData = m_pStaticObject->CreateData(pData->GetMeta());
    ARK_ASSERT_RET_VAL(pNewData != nullptr, false);

    pNewData->CopyFrom(pData);

    return true;
}

void AFCObject::OnDataCallBack(const std::string& name, const AFIDataNewType& old_data, const AFIDataNewType& new_data)
{
    ARK_ASSERT_RET_NONE(m_pCallBackManager != nullptr);

    m_pCallBackManager->OnDataCallBack(guid_, name, old_data, new_data);
}

bool AFCObject::AddCustomBool(const std::string& name, bool value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIDataNewType* pData = ARK_NEW AFCDataNewType;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetBool(value);
    return custom_data_list_.insert(name, pData).second;
}

bool AFCObject::AddCustomInt32(const std::string& name, const int32_t value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIDataNewType* pData = ARK_NEW AFCDataNewType;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetInt(value);
    return custom_data_list_.insert(name, pData).second;
}

bool AFCObject::AddCustomUInt32(const std::string& name, const uint32_t value)
{
    return false;
}

bool AFCObject::AddCustomInt64(const std::string& name, const int64_t value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIDataNewType* pData = ARK_NEW AFCDataNewType;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetInt64(value);
    return custom_data_list_.insert(name, pData).second;
}

bool AFCObject::AddCustomFloat(const std::string& name, const float value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIDataNewType* pData = ARK_NEW AFCDataNewType;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetFloat(value);
    return custom_data_list_.insert(name, pData).second;
}

bool AFCObject::AddCustomDouble(const std::string& name, const double value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIDataNewType* pData = ARK_NEW AFCDataNewType;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetDouble(value);
    return custom_data_list_.insert(name, pData).second;
}

bool AFCObject::AddCustomString(const std::string& name, const std::string& value)
{
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    AFIDataNewType* pData = ARK_NEW AFCDataNewType;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->SetString(value.c_str());
    return custom_data_list_.insert(name, pData).second;
}

bool AFCObject::AddCustomWString(const std::string& name, const std::wstring& value)
{
    return false;
}

bool AFCObject::AddCustomObject(const std::string& name, const AFGUID& value)
{
    return false;
}

bool AFCObject::SetCustomBool(const std::string& name, bool value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_BOOLEAN, false);

    pData->SetBool(value);

    return true;
}

bool AFCObject::SetCustomInt32(const std::string& name, const int32_t value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_INT32, false);

    pData->SetInt(value);

    return true;
}

bool AFCObject::SetCustomUInt32(const std::string& name, const uint32_t value)
{
    return false;
}

bool AFCObject::SetCustomInt64(const std::string& name, const int64_t value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_INT64, false);

    pData->SetInt64(value);

    return true;
}

bool AFCObject::SetCustomFloat(const std::string& name, const float value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_FLOAT, false);

    pData->SetFloat(value);

    return true;
}

bool AFCObject::SetCustomDouble(const std::string& name, const double value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_DOUBLE, false);

    pData->SetDouble(value);

    return true;
}

bool AFCObject::SetCustomString(const std::string& name, const std::string& value)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_STRING, false);

    pData->SetString(value.c_str());

    return true;
}

bool AFCObject::SetCustomWString(const std::string& name, const std::wstring& value)
{
    return false;
}

bool AFCObject::SetCustomObject(const std::string& name, const AFGUID& value)
{
    return false;
}

bool AFCObject::GetCustomBool(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    return pData->GetBool();
}

int32_t AFCObject::GetCustomInt32(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->GetInt();
}

uint32_t AFCObject::GetCustomUInt32(const std::string& name)
{
    return NULL_INT;
}

int64_t AFCObject::GetCustomInt64(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->GetInt64();
}

float AFCObject::GetCustomFloat(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_FLOAT);

    return pData->GetFloat();
}

double AFCObject::GetCustomDouble(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_DOUBLE);

    return pData->GetDouble();
}

const std::string& AFCObject::GetCustomString(const std::string& name)
{
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_STR);

    return pData->GetString();
}

const std::wstring& AFCObject::GetCustomWString(const std::string& name)
{
    return NULL_WIDESTR;
}

const AFGUID& AFCObject::GetCustomObject(const std::string& name)
{
    return NULL_GUID;
}

bool AFCObject::FindCustomData(const std::string& name)
{
    return custom_data_list_.find(name) != custom_data_list_.end();
}

bool AFCObject::RemoveCustomData(const std::string& name)
{
    return custom_data_list_.remove(name);
}

} // namespace ark
