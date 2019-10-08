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

AFCContainer::AFCContainer(ARK_SHARE_PTR<AFContainerMeta> container_meta, const AFGUID& parent_id,
    ARK_SHARE_PTR<AFClassCallBackManager> call_back_mgr)
    : parent_(parent_id)
{
    container_meta_ = container_meta;
    call_back_mgr_ = call_back_mgr;
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

ARK_SHARE_PTR<AFIEntity> AFCContainer::Find(uint32_t index) const
{
    return entity_data_list_.find_value(index);
}

uint32_t AFCContainer::Find(const AFGUID& id) const
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

bool AFCContainer::Exist(uint32_t index) const
{
    return (entity_data_list_.find(index) != entity_data_list_.end());
}

bool AFCContainer::Exist(const AFGUID& id) const
{
    return (Find(id) > 0);
}

bool AFCContainer::Place(ARK_SHARE_PTR<AFIEntity> pEntity)
{
    // should not be in container
    ARK_ASSERT_RET_VAL(pEntity != nullptr, false);

    ARK_ASSERT_RET_VAL(Find(pEntity->GetID()) == 0, false);

    return Place(SelectIndex(), pEntity);
}

bool AFCContainer::Place(uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity)
{
    // class should be same
    if (!pEntity || !container_meta_ || pEntity->GetClassName() != container_meta_->GetClassName())
    {
        return false;
    }

    auto iter = entity_data_list_.find(index);
    if (iter != entity_data_list_.end())
    {
        // has exist other entity
        return false;
    }

    if (!PlaceEntity(index, pEntity))
    {
        return false;
    }

    OnContainerPlace(index, pEntity);
    return true;
}

bool AFCContainer::Swap(const uint32_t src_index, const uint32_t dest_index)
{
    ARK_ASSERT_RET_VAL(src_index > 0u && dest_index > 0u && src_index != dest_index, false);

    auto iter_src = entity_data_list_.find(src_index);
    if (iter_src == entity_data_list_.end())
    {
        // fail if source do not exist
        return false;
    }

    auto pSrcEntity = iter_src->second;
    ARK_ASSERT_RET_VAL(pSrcEntity != nullptr, false);

    auto iter_dest = entity_data_list_.find(dest_index);
    if (iter_dest == entity_data_list_.end())
    {
        entity_data_list_.erase(src_index);
        if (!PlaceEntity(dest_index, iter_src->second))
        {
            return false;
        }

        OnContainerSwap(src_index, dest_index);
    }
    else
    {
        // exchange
        auto pDestEntity = iter_dest->second;
        iter_src->second = pDestEntity;
        iter_dest->second = pSrcEntity;

        OnContainerSwap(src_index, dest_index);
    }

    return true;
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

    // class should be same
    if (!container_meta_ || pSrcEntity->GetClassName() != container_meta_->GetClassName())
    {
        return false;
    }

    if (!pSrcContainer->Remove(src_index))
    {
        return false;
    }

    auto iter_dest = entity_data_list_.find(dest_index);
    if (iter_dest == entity_data_list_.end())
    {
        if (!PlaceEntity(dest_index, pSrcEntity))
        {
            return false;
        }

        OnContainerPlace(dest_index, pSrcEntity);
    }
    else
    {
        if (!Remove(dest_index))
        {
            return false;
        }

        auto pDestSrcEntity = iter_dest->second;
        if (!pSrcContainer->Place(src_index, pDestSrcEntity) || !PlaceEntity(dest_index, pSrcEntity))
        {
            return false;
        }

        OnContainerPlace(dest_index, pSrcEntity);
    }

    return true;
}

bool AFCContainer::Swap(ARK_SHARE_PTR<AFIContainer> pSrcContainer, const AFGUID& src_entity, const AFGUID& dest_entity)
{
    if (pSrcContainer == nullptr)
    {
        return false;
    }

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

    entity_data_list_.erase(index);
    OnContainerRemove(index, pEntity);

    return true;
}

bool AFCContainer::Remove(const AFGUID& id)
{
    auto index = Find(id);
    return Remove(index);
}

bool AFCContainer::Destroy(const uint32_t index)
{
    auto pEntity = entity_data_list_.find_value(index);
    if (nullptr == pEntity)
    {
        return false;
    }

    OnContainerDestroy(index, pEntity);
    entity_data_list_.erase(index);

    return true;
}

bool AFCContainer::Destroy(const AFGUID& id)
{
    auto index = Find(id);
    return Destroy(index);
}

uint32_t AFCContainer::SelectIndex() const
{
    if (entity_data_list_.size() == current_index_)
    {
        return current_index_ + 1;
    }
    else
    {
        // index starts from 1
        uint32_t index = 1u;
        for (auto iter : entity_data_list_)
        {
            if (iter.first != index)
            {
                break;
            }

            index += 1;
        }

        return index;
    }
}

bool AFCContainer::PlaceEntity(const uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity)
{
    if (!entity_data_list_.insert(index, pEntity).second)
    {
        return false;
    }

    if (current_index_ < index)
    {
        current_index_ = index;
    }

    return true;
}

void AFCContainer::OnContainerPlace(const uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity)
{
    ARK_ASSERT_RET_NONE(pEntity != nullptr);
    pEntity->SetParentContainer(shared_from_this());

    ARK_ASSERT_RET_NONE(call_back_mgr_ != nullptr && container_meta_ != nullptr);

    if (pEntity->IsSent())
    {
        call_back_mgr_->OnContainerCallBack(
            parent_, container_meta_->GetIndex(), ArkContainerOpType::OP_PLACE, index, 0u);
    }
    else
    {
        pEntity->UpdateSent();
        call_back_mgr_->OnContainerCallBack(
            parent_, container_meta_->GetIndex(), ArkContainerOpType::OP_CREATE, index, 0u);
    }
}

void AFCContainer::OnContainerSwap(const uint32_t index, const uint32_t swap_index)
{
    ARK_ASSERT_RET_NONE(call_back_mgr_ != nullptr && container_meta_ != nullptr);
    call_back_mgr_->OnContainerCallBack(
        parent_, container_meta_->GetIndex(), ArkContainerOpType::OP_SWAP, index, swap_index);
}

void AFCContainer::OnContainerRemove(const uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity)
{
    pEntity->SetParentContainer(nullptr);

    ARK_ASSERT_RET_NONE(call_back_mgr_ != nullptr && container_meta_ != nullptr);
    call_back_mgr_->OnContainerCallBack(parent_, container_meta_->GetIndex(), ArkContainerOpType::OP_REMOVE, index, 0u);
}

void AFCContainer::OnContainerDestroy(const uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity)
{
    pEntity->SetParentContainer(nullptr);

    ARK_ASSERT_RET_NONE(call_back_mgr_ != nullptr && container_meta_ != nullptr);
    call_back_mgr_->OnContainerCallBack(
        parent_, container_meta_->GetIndex(), ArkContainerOpType::OP_DESTROY, index, 0u);
}

const std::string& AFCContainer::GetName() const
{
    ARK_ASSERT_RET_VAL(container_meta_ != nullptr, NULL_STR);

    return container_meta_->GetName();
}

} // namespace ark
