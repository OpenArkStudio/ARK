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

#include "kernel/interface/AFIContainerManager.hpp"
#include "AFCContainer.hpp"

namespace ark {

class AFCContainerManager final : public AFIContainerManager
{
public:
    explicit AFCContainerManager() = default;
    ~AFCContainerManager() override;

    // find container
    ARK_SHARE_PTR<AFIContainer> FindContainer(const uint32_t index) override;

    ARK_SHARE_PTR<AFIContainer> CreateContainer(
        ARK_SHARE_PTR<AFClassMeta> pClassMeta, const uint32_t index, const AFGUID& parent_id) override;

    bool DestroyContainer(const uint32_t index) override;

    ARK_SHARE_PTR<AFIContainer> First() override;
    ARK_SHARE_PTR<AFIContainer> Next() override;

private:
    // container
    using ContainerList = AFNewSmartPtrMap<uint32_t, AFIContainer>;
    ContainerList container_data_;
    ContainerList::const_iterator iter_;
};

} // namespace ark
