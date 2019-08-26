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

#include "kernel/interface/AFIConfigNewModule.hpp"
#include "log/interface/AFILogModule.hpp"
#include "kernel/interface/AFIClassNewMetaModule.hpp"

namespace ark {

class AFCConfigNewModule final : public AFIConfigNewModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool Shut() override;

    bool Load() override;
    bool Save() override;
    bool Clear() override;

    // find config static object manager
    ARK_SHARE_PTR<AFStaticObjectManager> FindStaticObjectMgr(const std::string& class_name) override;

    ARK_SHARE_PTR<AFIStaticObject> FindStaticObject(const std::string& class_name, const size_t config_id) override;

protected:
    bool LoadConfig(ARK_SHARE_PTR<AFClassMeta> pClassMeta);

    ARK_SHARE_PTR<AFStaticObjectManager> CreateStaticObjectManager(const std::string& name);

private:
    bool loaded_{false};

    AFILogModule* m_pLogModule{nullptr};
    AFIClassNewMetaModule* m_pClassModule{nullptr};

    AFNewSmartPtrHashmap<std::string, AFStaticObjectManager> static_obj_mgr_list_;
};

} // namespace ark
