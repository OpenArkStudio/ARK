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
#include "base/AFDefine.hpp"

namespace ark {

class AFIEntity;
class AFIContainer
{
public:
    virtual ~AFIContainer() = default;

    // get entity parent unique id
    virtual const std::string& GetName() const = 0;
    virtual const ArkMaskType GetMask() const = 0;
    virtual const AFGUID& GetParentID() const = 0;
    virtual uint32_t GetIndex() const = 0;
    virtual uint32_t First() = 0;
    virtual uint32_t Next() = 0;
    virtual std::shared_ptr<AFIEntity> Find(uint32_t index) const = 0;
    virtual uint32_t Find(const AFGUID& id) const = 0;
    virtual bool Exist(uint32_t index) const = 0;
    virtual bool Exist(const AFGUID& id) const = 0;
    virtual bool Place(std::shared_ptr<AFIEntity> pEntity) = 0;
    virtual bool Place(uint32_t index, std::shared_ptr<AFIEntity> pEntity) = 0;
    virtual bool Swap(const uint32_t src_index, const uint32_t dest_index) = 0;
    virtual bool Swap(const AFGUID& src_entity, const AFGUID& dest_entity) = 0;

    virtual bool Remove(const uint32_t index) = 0;
    virtual bool Remove(const AFGUID& id) = 0;

    virtual bool Destroy(const uint32_t index) = 0;
    virtual bool Destroy(const AFGUID& id) = 0;
};

} // namespace ark
