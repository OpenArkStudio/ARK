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

#include "kernel/interface/AFIClassMetaModule.hpp"
#include "base/AFPluginManager.hpp"
#include "log/interface/AFILogModule.hpp"
#include "base/AFMap.hpp"
#include "AFClassMeta.hpp"
#include "AFClassMetaManager.hpp"

namespace ark {

class AFCClassMetaModule final : public AFIClassMetaModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    AFCClassMetaModule() = default;
    virtual ~AFCClassMetaModule() = default;

    bool Init() override;
    bool Shut() override;

    bool Load() override;

    bool AddClassCallBack(const std::string& class_name, CLASS_EVENT_FUNCTOR&& cb, const int32_t prio) override;

    bool DoClassEvent(const AFGUID& id, const std::string& class_name, const ArkEntityEvent class_event,
        const AFIDataList& args) override;

    ARK_SHARE_PTR<AFClassMeta> FindMeta(const std::string& class_name) override;

    const AFClassMetaManager::ClassMetaList& GetMetaList() const override;

protected:
    bool LoadConfig();

    bool LoadEntity();

    // load config meta
    bool LoadConfigMeta(const std::string& schema_path, ARK_SHARE_PTR<AFClassMeta> pClassMeta);

    bool LoadEntityMeta(const std::string& schema_path);

    ArkDataType ConvertDataType(const std::string& type_name);

private:
    AFILogModule* m_pLogModule{nullptr};

    AFClassMetaManager* m_pClassMetaManager{nullptr};
};

} // namespace ark
