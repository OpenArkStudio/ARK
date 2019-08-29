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
#include "AFCEntity.hpp"

namespace ark {

class AFCContainer final : public AFIContainer
{
public:
    using ObjectDataList = std::map<size_t, AFGUID>;

    explicit AFCContainer(const uint32_t index, ARK_SHARE_PTR<AFClassMeta> pClassMeta);

    ~AFCContainer() override;

    // get parent unique id
    const AFGUID& GetParentID() const override;

    const AFGUID& First() override;
    const AFGUID& Next() override;

    const AFGUID& Find(size_t index) override;
    size_t Find(const AFGUID& id) override;

    bool Place(const AFGUID& guid) override;
    bool Place(size_t index, const AFGUID& guid, AFGUID& replaced_guid) override;

    bool Swap(const AFGUID& src_container, const int src_index, const int dest_index) override;
    bool Swap(const AFGUID& src_container, const AFGUID& src_object, const AFGUID& dest_object) override;

private:
    // name
    uint32_t index_;

    // contain object class meta
    ARK_SHARE_PTR<AFClassMeta> class_meta_;

    // index start
    size_t current_index{0u};

    // parent id
    AFGUID parent_{NULL_GUID};

    // object data
    ObjectDataList::iterator iter_;
    ObjectDataList object_data_list_;
};

} // namespace ark
