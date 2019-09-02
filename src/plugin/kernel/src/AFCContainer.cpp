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

#include "kernel/include/AFCContainer.hpp"

namespace ark {

AFCContainer::AFCContainer(ARK_SHARE_PTR<AFContainerMeta> container_meta, const uint32_t index, const AFGUID& parent_id)
    : index_(index)
    , parent_(parent_id)
{
    container_meta_ = container_meta;
}

AFCContainer::~AFCContainer()
{
    entity_data_list_.clear();
}

// get parent unique id
const AFGUID& AFCContainer::GetParentID() const
{
    return parent_;
}

uint32_t AFCContainer::First()
{
    iter_ = entity_data_list_.begin();
    if (entity_data_list_.end() == iter_)
    {
        return NULL_INT;
    }

    return iter_->first;
}

uint32_t AFCContainer::Next()
{
    if (entity_data_list_.end() == iter_)
    {
        return NULL_INT;
    }

    ++iter_;
    if (entity_data_list_.end() == iter_)
    {
        return NULL_INT;
    }

    return iter_->first;
}

ARK_SHARE_PTR<AFIEntity> AFCContainer::Find(uint32_t index)
{
    auto iter = entity_data_list_.find(index);
    ARK_ASSERT_RET_VAL(iter != entity_data_list_.end(), nullptr);

    return iter->second;
}

uint32_t AFCContainer::Find(const AFGUID& id)
{
    for (auto iter : entity_data_list_)
    {
        if (id == iter.second->GetID())
        {
            return iter.first;
        }
    }

    return NULL_INT;
}

bool AFCContainer::Place(ARK_SHARE_PTR<AFIEntity> pEntity)
{
    // should not be in container
    ARK_ASSERT_RET_VAL(pEntity != nullptr, false);

    ARK_ASSERT_RET_VAL(Find(pEntity->GetID()) == 0, false);

    return AddEntity(current_index_, pEntity);
}

bool AFCContainer::Place(uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity, ARK_SHARE_PTR<AFIEntity> pEntityReplaced)
{
    auto iter = entity_data_list_.find(index);
    if (iter == entity_data_list_.end())
    {
        return AddEntity(index, pEntity);
    }
    else
    {
        pEntityReplaced = iter->second;
        iter->second = pEntity;
    }

    return true;
}

bool AFCContainer::Swap(const uint32_t src_index, const uint32_t dest_index)
{
    if (src_index == 0 || dest_index == 0)
    {
        return false;
    }

    auto iter_src = entity_data_list_.find(src_index);
    if (iter_src == entity_data_list_.end())
    {
        // fail if source do not exist
        return false;
    }

    auto iter_dest = entity_data_list_.find(dest_index);
    if (iter_dest == entity_data_list_.end())
    {
        if (AddEntity(dest_index, iter_src->second))
        {
            entity_data_list_.erase(src_index);
        }
    }
    else
    {
        // exchange
        auto pEntity = iter_src->second;
        iter_src->second = iter_dest->second;
        iter_dest->second = pEntity;

        return true;
    }

    return false;
}

bool AFCContainer::Swap(const AFGUID& src_entity, const AFGUID& dest_entity)
{
    auto src_index = Find(src_entity);
    auto dest_index = Find(dest_entity);

    return Swap(src_index, dest_index);
}

bool AFCContainer::Swap(ARK_SHARE_PTR<AFIContainer> pSrcContainer, const uint32_t src_index, const uint32_t dest_index)
{
    if (pSrcContainer == nullptr || src_index == 0 || dest_index == 0)
    {
        return false;
    }

    auto pSrcEntity = pSrcContainer->Find(src_index);
    if (pSrcEntity == nullptr)
    {
        return false;
    }

    auto iter_dest = entity_data_list_.find(dest_index);
    if (iter_dest == entity_data_list_.end())
    {
        if (!AddEntity(dest_index, pSrcEntity))
        {
            return false;
        }
    }
    else
    {
        iter_dest->second = pSrcEntity;
    }

    return pSrcContainer->Remove(src_index);
}

bool AFCContainer::Swap(ARK_SHARE_PTR<AFIContainer> pSrcContainer, const AFGUID& src_entity, const AFGUID& dest_entity)
{
    auto src_index = pSrcContainer->Find(src_entity);
    auto dest_index = Find(dest_entity);

    return Swap(pSrcContainer, src_index, dest_index);
}

bool AFCContainer::Remove(const uint32_t index)
{
    auto pEntity = entity_data_list_.find_value(index);
    if (nullptr == pEntity)
    {
        return false;
    }

    if (!entity_data_list_.erase(index))
    {
        return false;
    }

    pEntity->SetParentContainer(nullptr);

    return true;
}

bool AFCContainer::AddEntity(const uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity)
{
    if (!entity_data_list_.insert(index, pEntity).second)
    {
        return false;
    }

    pEntity->SetParentContainer(shared_from_this());

    if (current_index_ < index)
    {
        current_index_ = index;
    }

    return true;
}

const std::string& AFCContainer::GetName() const
{
    return container_meta_->GetName();
}

} // namespace ark
