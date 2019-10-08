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

#include "plugin/kernel/interface/AFIContainer.hpp"
#include "base/AFMap.hpp"
#include "AFClassMeta.hpp"

namespace ark {

class AFCContainer final : public AFIContainer, public std::enable_shared_from_this<AFCContainer>
{
public:
    using EntityDataList = AFSmartPtrMap<uint32_t, AFIEntity>;

private:
    // contain entity class meta
    ARK_SHARE_PTR<AFContainerMeta> container_meta_{nullptr};

    // index start
    uint32_t current_index_{0u};

    // parent id
    AFGUID parent_{NULL_GUID};

    // entity data
    EntityDataList::iterator iter_;
    EntityDataList entity_data_list_;

    // call back
    ARK_SHARE_PTR<AFClassCallBackManager> call_back_mgr_{nullptr};

public:
    AFCContainer() = delete;

    explicit AFCContainer(ARK_SHARE_PTR<AFContainerMeta> container_meta, const AFGUID& parent_id,
        ARK_SHARE_PTR<AFClassCallBackManager> class_meta);

    const std::string& GetName() const override;

    // get parent unique id
    const AFGUID& GetParentID() const override;

    uint32_t First() override;
    uint32_t Next() override;

    ARK_SHARE_PTR<AFIEntity> Find(uint32_t index) const override;
    uint32_t Find(const AFGUID& id) const override;

    bool Exist(uint32_t index) const override;
    bool Exist(const AFGUID& id) const override;

    bool Place(ARK_SHARE_PTR<AFIEntity> pEntity) override;
    bool Place(uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity) override;

    bool Swap(const uint32_t src_index, const uint32_t dest_index) override;
    bool Swap(const AFGUID& src_entity, const AFGUID& dest_entity) override;

    bool Swap(ARK_SHARE_PTR<AFIContainer> pSrcContainer, const uint32_t src_index, const uint32_t dest_index) override;
    bool Swap(ARK_SHARE_PTR<AFIContainer> pSrcContainer, const AFGUID& src_entity, const AFGUID& dest_entity) override;

    bool Remove(const uint32_t index) override;
    bool Remove(const AFGUID& id) override;

    bool Destroy(const uint32_t index) override;
    bool Destroy(const AFGUID& id) override;

private:
    uint32_t SelectIndex() const;

    bool PlaceEntity(const uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity);

    void OnContainerPlace(const uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity);
    void OnContainerSwap(const uint32_t index, const uint32_t swap_index);
    void OnContainerRemove(const uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity);
    void OnContainerDestroy(const uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity);
};

} // namespace ark
