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

#include "kernel/include/AFCContainerManager.hpp"

namespace ark {

AFCContainerManager ::~AFCContainerManager()
{
    container_data_.clear();
}

// find container
ARK_SHARE_PTR<AFIContainer> AFCContainerManager::FindContainer(const std::string& name)
{
    return container_data_.find_value(name);
}

ARK_SHARE_PTR<AFIContainer> AFCContainerManager::CreateContainer(
    const std::string& name, ARK_SHARE_PTR<AFClassMeta> pClassMeta)
{
    // return if found
    auto pContainer = FindContainer(name);
    if (nullptr != pContainer)
    {
        return pContainer;
    }

    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, nullptr);

    auto pContainerClassMeta = pClassMeta->FindContainer(name);
    ARK_ASSERT_RET_VAL(pContainerClassMeta != nullptr, nullptr);

    pContainer = std::make_shared<AFCContainer>(name, pContainerClassMeta);
    ARK_ASSERT_RET_VAL(pContainer != nullptr, nullptr);

    container_data_.insert(name, pContainer);

    return pContainer;
}

bool AFCContainerManager::DeleteContainer(const std::string& name)
{
    return container_data_.erase(name);
}

} // namespace ark
