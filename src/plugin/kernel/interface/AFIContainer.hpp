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
#include "base/AFMacros.hpp"
#include "AFIEntity.hpp"

namespace ark {

class AFIEntity;
class AFIContainer
{
public:
    virtual ~AFIContainer() = default;

    // get entity parent unique id
    virtual const std::string& GetName() const = 0;
    virtual const AFGUID& GetParentID() const = 0;
    virtual uint32_t First() = 0;
    virtual uint32_t Next() = 0;
    virtual ARK_SHARE_PTR<AFIEntity> Find(uint32_t index) = 0;
    virtual uint32_t Find(const AFGUID& id) = 0;
    virtual bool Place(ARK_SHARE_PTR<AFIEntity> pEntity) = 0;
    virtual bool Place(uint32_t index, ARK_SHARE_PTR<AFIEntity> pEntity, ARK_SHARE_PTR<AFIEntity> pEntityReplaced) = 0;
    virtual bool Swap(const uint32_t src_index, const uint32_t dest_index) = 0;
    virtual bool Swap(const AFGUID& src_entity, const AFGUID& dest_entity) = 0;
    virtual bool Swap(
        ARK_SHARE_PTR<AFIContainer> pSrcContainer, const uint32_t src_index, const uint32_t dest_index) = 0;
    virtual bool Swap(
        ARK_SHARE_PTR<AFIContainer> pSrcContainer, const AFGUID& src_entity, const AFGUID& dest_entity) = 0;

    virtual bool Remove(const uint32_t index) = 0;
};

} // namespace ark
