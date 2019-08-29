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

#include "interface/AFIModule.hpp"
#include "AFIStaticEntity.hpp"
#include "kernel/include/AFStaticEntityManager.hpp"

namespace ark {

class AFIConfigModule : public AFIModule
{
public:
    virtual bool Load() = 0;
    virtual bool Save() = 0;
    virtual bool Clear() = 0;

    // find config static object manager
    virtual ARK_SHARE_PTR<AFStaticEntityManager> FindStaticEntityMgr(const std::string& class_name) = 0;

    // find config
    virtual ARK_SHARE_PTR<AFIStaticEntity> FindStaticEntity(const std::string& class_name, const size_t config_id) = 0;
};

} // namespace ark
