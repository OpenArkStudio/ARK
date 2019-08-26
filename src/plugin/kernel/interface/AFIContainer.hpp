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

#include "base/AFPlatform.hpp"

namespace ark {

class AFIContainer
{
public:
    virtual ~AFIContainer() = default;

    // get object parent unique id
    virtual const AFGUID& GetParentID() const = 0;
    virtual const AFGUID& First() = 0;
    virtual const AFGUID& Next() = 0;
    virtual const AFGUID& Find(size_t index) = 0;
    virtual size_t Find(const AFGUID& id) = 0;
    virtual bool Place(const AFGUID& guid) = 0;
    virtual bool Place(size_t index, const AFGUID& guid, AFGUID& replaced_guid) = 0;
    virtual bool Swap(const AFGUID& src_container, const int src_index, const int dest_index) = 0;
    virtual bool Swap(const AFGUID& src_container, const AFGUID& src_object, const AFGUID& dest_object) = 0;
};

} // namespace ark
