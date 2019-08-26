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

#include "base/AFMap.hpp"
#include "base/AFArrayMap.hpp"
#include "base/AFPluginManager.hpp"
#include "utility/interface/AFIGUIDModule.hpp"
#include "log/interface/AFILogModule.hpp"
#include "kernel/include/AFCDataList.hpp"
#include "kernel/interface/AFIKernelNewModule.hpp"
#include "kernel/interface/AFIMapModule.hpp"
#include "kernel/interface/AFIClassNewMetaModule.hpp"
#include "kernel/interface/AFIConfigNewModule.hpp"
#include "kernel/interface/AFIObject.hpp"

namespace ark {

class AFCKernelNewModule : public AFIKernelNewModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit AFCKernelNewModule();
    ~AFCKernelNewModule() override;

    bool Init() override;
    bool Update() override;
    bool PreShut() override;

    bool AddEventCallBack(const AFGUID& self, const int event_id, EVENT_PROCESS_FUNCTOR&& cb) override;

    bool AddDataCallBack(const std::string& class_name, const std::string& name, DATA_EVENT_FUNCTOR&& cb) override;

    bool AddRecordCallBack(const std::string& class_name, const std::string& name, RECORD_EVENT_FUNCTOR&& cb) override;

    ///////////////////////////////////////////////////////////////////////
    ARK_SHARE_PTR<AFIObject> FindObject(const AFGUID& self) override;
    ARK_SHARE_PTR<AFIObject> CreateObject(const AFGUID& self, const int nSceneID, const int nGroupID,
        const std::string& strClassName, const uint64_t config_id, const AFIDataList& arg) override;

    ARK_SHARE_PTR<AFIObject> CreateContainerObject(const AFGUID& self, const std::string& container_name,
        const std::string& class_name, const uint64_t config_id) override;

    bool DestroyAll() override;
    bool DestroyEntity(const AFGUID& self) override;

    //////////////////////////////////////////////////////////////////////////
    bool LogInfo(const AFGUID& id) override;
    bool LogSelfInfo(const AFGUID& id);
    //////////////////////////////////////////////////////////////////////////

    bool DoEvent(const AFGUID& self, const std::string& class_name, ArkEntityEvent class_event,
        const AFIDataList& args) override;
    bool DoEvent(const AFGUID& self, const int event_id, const AFIDataList& args) override;

protected:
    bool DestroySelf(const AFGUID& self);

    bool AddClassCallBack(const std::string& class_name, CLASS_EVENT_FUNCTOR&& cb) override;

private:
    std::list<AFGUID> delete_list_;

    AFGUID cur_exec_object_;

    AFIMapModule* m_pMapModule{nullptr};
    AFILogModule* m_pLogModule{nullptr};
    AFIGUIDModule* m_pGUIDModule{nullptr};
    AFIConfigNewModule* m_pConfigModule{nullptr};
    AFIClassNewMetaModule* m_pClassModule{nullptr};

    AFArrayMap<std::string, int32_t> inner_nodes_;
    AFNewSmartPtrMap<AFGUID, AFIObject> objects_;
};

} // namespace ark
