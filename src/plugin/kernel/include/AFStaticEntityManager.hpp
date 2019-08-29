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

#include "base/AFMap.hpp"
#include "AFCStaticEntityInner.hpp"

namespace ark {

class AFStaticEntityManager final
{
public:
    using StaticObjectList = AFNewSmartPtrMap<ID_TYPE, AFIStaticEntity>;

    AFStaticEntityManager() = default;
    virtual ~AFStaticEntityManager()
    {
        static_object_list_.clear();
    }

    ARK_SHARE_PTR<AFIStaticEntity> CreateObject(const ID_TYPE_ARG config_id, ARK_SHARE_PTR<AFClassMeta> pClassMeta)
    {
        ARK_ASSERT_RET_VAL(pClassMeta != nullptr, nullptr);

        // return nullptr if existed
        auto iter = static_object_list_.find(config_id);
        ARK_ASSERT_RET_VAL(iter == static_object_list_.end(), nullptr);

        // create new class
        auto pObject = std::make_shared<AFCStaticEntityInner>(pClassMeta, config_id);
        static_object_list_.insert(config_id, pObject);

        return pObject;
    }

    ARK_SHARE_PTR<AFIStaticEntity> FindObject(const ID_TYPE_ARG config_id)
    {
        return static_object_list_.find_value(config_id);
    }

private:
    StaticObjectList static_object_list_;
};

} // namespace ark
