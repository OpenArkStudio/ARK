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

#pragma once

#include "base/AFArrayMap.hpp"
#include "base/AFPluginManager.hpp"
#include "kernel/interface/AFIConfigModule.hpp"
#include "kernel/include/AFCDataList.hpp"
#include "kernel/include/AFDataTable.hpp"
#include "kernel/include/AFCDataNodeManager.hpp"
#include "kernel/include/AFCDataTableManager.hpp"
#include "kernel/include/AFCMetaClassModule.hpp"

namespace ark {

class AFCMetaClass;

class AFConfigInfo
{
public:
    AFConfigInfo()
    {
        m_pNodeManager = std::make_shared<AFCDataNodeManager>(NULL_GUID);
        m_pTableManager = std::make_shared<AFCDataTableManager>(NULL_GUID);
    }

    ~AFConfigInfo()
    {
        m_pNodeManager->Clear();
        m_pTableManager->Clear();
    }

    ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager()
    {
        return m_pNodeManager;
    }

    ARK_SHARE_PTR<AFIDataTableManager> GetTableManager()
    {
        return m_pTableManager;
    }

private:
    ARK_SHARE_PTR<AFIDataNodeManager> m_pNodeManager;
    ARK_SHARE_PTR<AFIDataTableManager> m_pTableManager;
};

class AFCConfigModule : public AFIConfigModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool Shut() override;

    bool Load() override;
    bool Save() override;
    bool Clear() override;

    bool ExistConfig(const std::string& config_id) override;
    bool ExistConfig(const std::string& class_name, const std::string& config_id) override;

    ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager(const std::string& config_id) override;
    ARK_SHARE_PTR<AFIDataTableManager> GetTableManager(const std::string& config_id) override;

    bool GetNodeBool(const std::string& config_id, const std::string& node_name) override;
    int32_t GetNodeInt(const std::string& config_id, const std::string& node_name) override;
    int64_t GetNodeInt64(const std::string& config_id, const std::string& node_name) override;
    float GetNodeFloat(const std::string& config_id, const std::string& node_name) override;
    double GetNodeDouble(const std::string& config_id, const std::string& node_name) override;
    const char* GetNodeString(const std::string& config_id, const std::string& node_name) override;

protected:
    AFDataNode* GetNode(const std::string& config_id, const std::string& node_name);
    bool Load(rapidxml::xml_node<>* attr_node, ARK_SHARE_PTR<AFIMetaClass> pMetaClass);

private:
    AFIMetaClassModule* m_pMetaClassModule{nullptr};
    bool loaded_{false};
    AFArrayMap<std::string, AFConfigInfo> all_configs_;
};

} // namespace ark
