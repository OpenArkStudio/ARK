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

#pragma once

#include "kernel/interface/AFIEntity.hpp"
#include "base/AFMap.hpp"
#include "base/AFList.hpp"
#include "kernel/interface/AFIData.hpp"
#include "kernel/interface/AFIStaticEntityInner.hpp"
#include "AFClassCallBackManager.hpp"
#include "kernel/interface/AFIContainerManager.hpp"

namespace ark {

class AFCEntity final : public AFIEntity
{
public:
    AFCEntity() = delete;

    explicit AFCEntity(ARK_SHARE_PTR<AFClassMeta> pClassMeta, const AFGUID& guid, const ID_TYPE_ARG config_id,
        const int32_t map, const int32_t map_entity_id);

    ~AFCEntity() override;

    void Update() override;

    // get unique id
    const AFGUID& GetID() const override;

    // get parent unique id
    const AFGUID& GetParentID() const override;

    bool HaveMask(const std::string& name, ArkDataMaskType mask) override;
    bool HaveMask(const uint32_t index, ArkDataMaskType mask) override;

    const std::string& GetClassName() const override;
    const ID_TYPE_ARG GetConfigID() const override;
    int32_t GetMapID() const override;
    int32_t GetMapEntityID() const override;

    bool InitData(ARK_SHARE_PTR<AFIStaticEntity> pStaticObject) override;

    // set data
    bool SetBool(const std::string& name, bool value) override;
    bool SetInt32(const std::string& name, const int32_t value) override;
    bool SetUInt32(const std::string& name, const uint32_t value) override;
    bool SetInt64(const std::string& name, const int64_t value) override;
    bool SetFloat(const std::string& name, const float value) override;
    bool SetDouble(const std::string& name, const double value) override;
    bool SetString(const std::string& name, const std::string& value) override;
    bool SetWString(const std::string& name, const std::wstring& value) override;
    bool SetObject(const std::string& name, const AFGUID& value) override;

    bool SetBool(const uint32_t index, bool value) override;
    bool SetInt32(const uint32_t index, const int32_t value) override;
    bool SetUInt32(const uint32_t index, const uint32_t value) override;
    bool SetInt64(const uint32_t index, const int64_t value) override;
    bool SetFloat(const uint32_t index, const float value) override;
    bool SetDouble(const uint32_t index, const double value) override;
    bool SetString(const uint32_t index, const std::string& value) override;
    bool SetWString(const uint32_t index, const std::wstring& value) override;
    bool SetObject(const uint32_t index, const AFGUID& value) override;

    // get data
    AFINode* GetNode(const std::string& name) override;
    bool GetBool(const std::string& name) override;
    int32_t GetInt32(const std::string& name) override;
    uint32_t GetUInt32(const std::string& name) override;
    int64_t GetInt64(const std::string& name) override;
    float GetFloat(const std::string& name) override;
    double GetDouble(const std::string& name) override;
    const std::string& GetString(const std::string& name) override;
    const std::wstring& GetWString(const std::string& name) override;
    const AFGUID& GetObject(const std::string& name) override;

    AFINode* GetNode(const uint32_t index) override;
    bool GetBool(const uint32_t index) override;
    int32_t GetInt32(const uint32_t index) override;
    uint32_t GetUInt32(const uint32_t index) override;
    int64_t GetInt64(const uint32_t index) override;
    float GetFloat(const uint32_t index) override;
    double GetDouble(const uint32_t index) override;
    const std::string& GetString(const uint32_t index) override;
    const std::wstring& GetWString(const uint32_t index) override;
    const AFGUID& GetObject(const uint32_t index) override;

    // container operation
    ARK_SHARE_PTR<AFIContainer> FindContainer(const std::string& name) override;
    ARK_SHARE_PTR<AFIContainer> FindContainer(const uint32_t index) override;

    // table data
    AFITable* FindTable(const std::string& name) override;
    AFITable* FindTable(const uint32_t index) override;

    ARK_SHARE_PTR<AFIEventManager>& GetEventManager() override;

    // custom data
    bool AddCustomBool(const std::string& name, bool value) override;
    bool AddCustomInt32(const std::string& name, const int32_t value) override;
    bool AddCustomUInt32(const std::string& name, const uint32_t value) override;
    bool AddCustomInt64(const std::string& name, const int64_t value) override;
    bool AddCustomFloat(const std::string& name, const float value) override;
    bool AddCustomDouble(const std::string& name, const double value) override;
    bool AddCustomString(const std::string& name, const std::string& value) override;
    bool AddCustomWString(const std::string& name, const std::wstring& value) override;
    bool AddCustomObject(const std::string& name, const AFGUID& value) override;

    bool SetCustomBool(const std::string& name, bool value) override;
    bool SetCustomInt32(const std::string& name, const int32_t value) override;
    bool SetCustomUInt32(const std::string& name, const uint32_t value) override;
    bool SetCustomInt64(const std::string& name, const int64_t value) override;
    bool SetCustomFloat(const std::string& name, const float value) override;
    bool SetCustomDouble(const std::string& name, const double value) override;
    bool SetCustomString(const std::string& name, const std::string& value) override;
    bool SetCustomWString(const std::string& name, const std::wstring& value) override;
    bool SetCustomObject(const std::string& name, const AFGUID& value) override;

    bool GetCustomBool(const std::string& name) override;
    int32_t GetCustomInt32(const std::string& name) override;
    uint32_t GetCustomUInt32(const std::string& name) override;
    int64_t GetCustomInt64(const std::string& name) override;
    float GetCustomFloat(const std::string& name) override;
    double GetCustomDouble(const std::string& name) override;
    const std::string& GetCustomString(const std::string& name) override;
    const std::wstring& GetCustomWString(const std::string& name) override;
    const AFGUID& GetCustomObject(const std::string& name) override;

    bool FindCustomData(const std::string& name) override;
    bool RemoveCustomData(const std::string& name) override;

private:
    // create data new and copy data arg
    bool CopyData(AFINode* pData);

    void OnDataCallBack(
        const std::string& name, const uint32_t index, const AFIData& old_data, const AFIData& new_data);

private:
    // object unique id
    AFGUID guid_{NULL_GUID};

    // map id
    int32_t map_id_{NULL_INT};

    // map obj id
    int32_t map_entity_id_{NULL_INT};

    // parent guid
    AFGUID parent_{NULL_GUID};

    // custom data
    using CustomDataList = AFNewHashmap<std::string, AFIData>;
    CustomDataList custom_data_list_;

    // static object
    ARK_SHARE_PTR<AFIStaticEntityInner> m_pStaticObject{nullptr};

    // container manager
    ARK_SHARE_PTR<AFIContainerManager> m_pContainerManager{nullptr};

    // event manager
    ARK_SHARE_PTR<AFIEventManager> m_pEventManager{nullptr};

    // call back manager
    ARK_SHARE_PTR<AFClassCallBackManager> m_pCallBackManager{nullptr};
};

} // namespace ark
