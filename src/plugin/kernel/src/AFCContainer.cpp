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

AFCContainer::AFCContainer(const uint32_t index, ARK_SHARE_PTR<AFClassMeta> pClassMeta)
    : index_(index)
{
    class_meta_ = pClassMeta;
}

AFCContainer::~AFCContainer()
{
    object_data_list_.clear();
}

// get parent unique id
const AFGUID& AFCContainer::GetParentID() const
{
    return parent_;
}

const AFGUID& AFCContainer::First()
{
    iter_ = object_data_list_.begin();
    if (object_data_list_.end() == iter_)
    {
        return NULL_GUID;
    }

    return iter_->second;
}

const AFGUID& AFCContainer::Next()
{
    if (object_data_list_.end() == iter_)
    {
        return NULL_GUID;
    }

    ++iter_;
    if (object_data_list_.end() == iter_)
    {
        return NULL_GUID;
    }

    return iter_->second;
}

const AFGUID& AFCContainer::Find(size_t index)
{
    auto iter = object_data_list_.find(index);
    ARK_ASSERT_RET_VAL(iter != object_data_list_.end(), NULL_GUID);

    return iter->second;
}

size_t AFCContainer::Find(const AFGUID& id)
{
    for (auto iter : object_data_list_)
    {
        if (id == iter.second)
        {
            return iter.first;
        }
    }

    return NULL_INT;
}

bool AFCContainer::Place(const AFGUID& guid)
{
    // should not be in container
    ARK_ASSERT_RET_VAL(Find(guid) == 0, false);

    return object_data_list_.insert(std::make_pair(++current_index, guid)).second;
}

bool AFCContainer::Place(size_t index, const AFGUID& guid, AFGUID& replaced_guid)
{
    auto iter = object_data_list_.find(index);
    if (iter == object_data_list_.end())
    {
        return object_data_list_.insert(std::make_pair(index, guid)).second;
    }
    else
    {
        replaced_guid = iter->second;
        iter->second = guid;
    }

    return true;
}

bool AFCContainer::Swap(const AFGUID& src_container, const int src_index, const int dest_index)
{
    return false;
}

bool AFCContainer::Swap(const AFGUID& src_container, const AFGUID& src_object, const AFGUID& dest_object)
{
    return false;
}

} // namespace ark
