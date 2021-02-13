/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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
#include "base/container/AFMap.hpp"
#include "base/container/AFList.hpp"
#include "kernel/interface/AFIData.hpp"
#include "AFClassCallBackManager.hpp"
#include "kernel/interface/AFIContainerManager.hpp"
#include "AFNodeManager.hpp"
#include "AFTableManager.hpp"
#include "kernel/interface/AFIEventManager.hpp"
#include "kernel/interface/AFINode.hpp"
#include "base/AFDefine.hpp"
#include "kernel/interface/AFIDataList.hpp"
#include "AFCContainerManager.hpp"

namespace ark {

class AFEntityOptCharactor
{
public:
    AFEntityOptCharactor(const int32_t map_id, const int32_t map_entity_id)
        : map_id_(map_id)
        , map_entity_id_(map_entity_id)
    {
        m_pContainerManager = std::make_shared<AFCContainerManager>();
    }

    // map id
    int32_t map_id_{NULL_INT};

    // map obj id
    int32_t map_entity_id_{NULL_INT};

    // pos
    AFVector3D pos_{NULL_VECTOR3D};

    // orient
    float orient_{NULL_FLOAT};

    // container manager
    std::shared_ptr<AFIContainerManager> m_pContainerManager{nullptr};
};

class AFCEntity final : public AFIEntity
{
private:
    friend class AFCKernelModule;

    // object unique id
    guid_t guid_{NULL_GUID};

    // config id
    ID_TYPE config_id_{0};

    // parent container
    std::shared_ptr<AFIContainer> parent_container_{nullptr};

    // is sent to client(for container use)
    bool sent_{false};

    // optional data only for player and npc
    std::shared_ptr<AFEntityOptCharactor> opt_charactor_{nullptr};

    // custom data
    using CustomDataList = AFHashmap<std::string, AFIData>;
    CustomDataList custom_data_list_;

    // class meta
    std::shared_ptr<AFClassMeta> class_meta_{nullptr};

    // node data
    std::shared_ptr<AFNodeManager> m_pNodeManager{nullptr};

    // table data
    std::shared_ptr<AFTableManager> m_pTableManager{nullptr};

    // event manager
    std::shared_ptr<AFIEventManager> m_pEventManager{nullptr};

    // call back manager
    std::shared_ptr<AFClassCallBackManager> m_pCallBackManager{nullptr};

public:
    AFCEntity() = delete;

    explicit AFCEntity(std::shared_ptr<AFClassMeta> pClassMeta, const guid_t& guid, const ID_TYPE config_id,
        const int32_t map_id, const int32_t map_entity_id, const AFIDataList& data_list);

    void Update() override;

    // get unique id
    const guid_t& GetID() const override;

    // get parent unique id
    std::shared_ptr<AFIContainer> GetParentContainer() const override;
    bool SetParentContainer(std::shared_ptr<AFIContainer> pContainer) override;

    bool IsPublic(const std::string& name) const override;
    bool IsPublic(const uint32_t index) const override;
    bool IsPrivate(const std::string& name) const override;
    bool IsPrivate(const uint32_t index) const override;
    bool IsSave(const std::string& name) const override;
    bool IsSave(const uint32_t index) const override;
    bool IsRealTime(const std::string& name) const override;
    bool IsRealTime(const uint32_t index) const override;
    bool HaveMask(const std::string& name, ArkDataMask feature) const override;
    bool HaveMask(const uint32_t index, ArkDataMask feature) const override;

    ArkMaskType GetMask(const uint32_t index) const override;

    const std::string& GetClassName() const override;

    ArkDataType GetNodeType(const std::string& name) const override;
    ArkDataType GetNodeType(const uint32_t value) const override;

    uint32_t GetIndex(const std::string& name) const override;

    ID_TYPE GetConfigID() const override;
    int32_t GetMapID() const override;
    int32_t GetMapEntityID() const override;
    bool SwitchScene(const int32_t map_id, const int32_t map_inst_id, const AFVector3D& pos) override;

    const AFVector3D& GetPosition() const override;
    float GetPositionX() const override;
    float GetPositionY() const override;
    float GetPositionZ() const override;
    float GetOrient() const override;

    bool SetPosition(const AFVector3D& postion, const float orient) override;
    bool SetPosition(const float x, const float y, const float z, const float orient) override;

    // set data
    bool SetBool(const std::string& name, bool value) override;
    bool SetInt32(const std::string& name, const int32_t value) override;
    bool SetUInt32(const std::string& name, const uint32_t value) override;
    bool SetInt64(const std::string& name, const int64_t value) override;
    bool SetUInt64(const std::string& name, const uint64_t value) override;
    bool SetFloat(const std::string& name, const float value) override;
    bool SetDouble(const std::string& name, const double value) override;
    bool SetString(const std::string& name, const std::string& value) override;
    bool SetWString(const std::string& name, const std::wstring& value) override;
    bool SetGUID(const std::string& name, const guid_t& value) override;

    bool SetBool(const uint32_t index, bool value) override;
    bool SetInt32(const uint32_t index, const int32_t value) override;
    bool SetUInt32(const uint32_t index, const uint32_t value) override;
    bool SetInt64(const uint32_t index, const int64_t value) override;
    bool SetUInt64(const uint32_t index, const uint64_t value) override;
    bool SetFloat(const uint32_t index, const float value) override;
    bool SetDouble(const uint32_t index, const double value) override;
    bool SetString(const uint32_t index, const std::string& value) override;
    bool SetWString(const uint32_t index, const std::wstring& value) override;
    bool SetGUID(const uint32_t index, const guid_t& value) override;

    // get data
    bool GetNode(const std::string& name, AFIData& data) const override;
    bool GetBool(const std::string& name) const override;
    int32_t GetInt32(const std::string& name) const override;
    uint32_t GetUInt32(const std::string& name) const override;
    int64_t GetInt64(const std::string& name) const override;
    uint64_t GetUInt64(const std::string& name) const override;
    float GetFloat(const std::string& name) const override;
    double GetDouble(const std::string& name) const override;
    const std::string& GetString(const std::string& name) const override;
    const std::wstring& GetWString(const std::string& name) const override;
    const guid_t& GetGUID(const std::string& name) const override;

    bool GetNode(const uint32_t index, AFIData& data) const override;
    bool GetBool(const uint32_t index) const override;
    int32_t GetInt32(const uint32_t index) const override;
    uint32_t GetUInt32(const uint32_t index) const override;
    int64_t GetInt64(const uint32_t index) const override;
    uint64_t GetUInt64(const uint32_t index) const override;
    float GetFloat(const uint32_t index) const override;
    double GetDouble(const uint32_t index) const override;
    const std::string& GetString(const uint32_t index) const override;
    const std::wstring& GetWString(const uint32_t index) const override;
    const guid_t& GetGUID(const uint32_t index) const override;

    // container operation
    std::shared_ptr<AFIContainer> FindContainer(const std::string& name) override;
    std::shared_ptr<AFIContainer> FindContainer(const uint32_t index) override;

    // table data
    AFITable* FindTable(const std::string& name) override;
    AFITable* FindTable(const uint32_t index) override;

    // custom data
    bool AddCustomBool(const std::string& name, bool value) override;
    bool AddCustomInt32(const std::string& name, const int32_t value) override;
    bool AddCustomUInt32(const std::string& name, const uint32_t value) override;
    bool AddCustomInt64(const std::string& name, const int64_t value) override;
    bool AddCustomFloat(const std::string& name, const float value) override;
    bool AddCustomDouble(const std::string& name, const double value) override;
    bool AddCustomString(const std::string& name, const std::string& value) override;
    bool AddCustomWString(const std::string& name, const std::wstring& value) override;
    bool AddCustomGUID(const std::string& name, const guid_t& value) override;

    bool SetCustomBool(const std::string& name, bool value) override;
    bool SetCustomInt32(const std::string& name, const int32_t value) override;
    bool SetCustomUInt32(const std::string& name, const uint32_t value) override;
    bool SetCustomInt64(const std::string& name, const int64_t value) override;
    bool SetCustomFloat(const std::string& name, const float value) override;
    bool SetCustomDouble(const std::string& name, const double value) override;
    bool SetCustomString(const std::string& name, const std::string& value) override;
    bool SetCustomWString(const std::string& name, const std::wstring& value) override;
    bool SetCustomGUID(const std::string& name, const guid_t& value) override;

    bool GetCustomBool(const std::string& name) const override;
    int32_t GetCustomInt32(const std::string& name) const override;
    uint32_t GetCustomUInt32(const std::string& name) const override;
    int64_t GetCustomInt64(const std::string& name) const override;
    float GetCustomFloat(const std::string& name) const override;
    double GetCustomDouble(const std::string& name) const override;
    const char* GetCustomString(const std::string& name) const override;
    const std::wstring& GetCustomWString(const std::string& name) const override;
    const guid_t& GetCustomGUID(const std::string& name) const override;

    bool FindCustomData(const std::string& name) const override;
    bool RemoveCustomData(const std::string& name) override;

    bool IsSent() const override;
    void UpdateSent() override;

private:
    std::shared_ptr<AFNodeManager> GetNodeManager() const;

    std::shared_ptr<AFTableManager> GetTableManager() const;

    std::shared_ptr<AFIContainerManager> GetContainerManager() const;

    std::shared_ptr<AFIEventManager> GetEventManager() const;

    std::shared_ptr<AFClassMeta> GetClassMeta() const;

    int OnNodeCallBack(AFINode* pNode, const AFIData& old_data, const AFIData& new_data);

    int OnTableCallBack(const ArkMaskType mask, AFINode* pNode, const TABLE_EVENT_DATA& event_data,
        const AFIData& old_data, const AFIData& new_data);
};

} // namespace ark
