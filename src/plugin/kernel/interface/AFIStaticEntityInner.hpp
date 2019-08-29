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

#include "AFIStaticEntity.hpp"
#include "AFINode.hpp"
#include "AFITable.hpp"
#include "kernel/include/AFClassMeta.hpp"
#include "kernel/include/AFNodeMeta.hpp"

namespace ark {

class AFIStaticEntityInner : public AFIStaticEntity
{
public:
    using DataList = AFNewMap<uint32_t, AFINode>;

    virtual ~AFIStaticEntityInner() = default;

    virtual uint32_t GetIndex(const std::string& name) = 0;

    // table op
    virtual AFITable* AddTable(const AFGUID& guid, const uint32_t index) = 0;

    virtual AFITable* FindTable(const uint32_t index) = 0;

    // data op
    virtual void Reset() = 0;
    virtual AFINode* CreateData(ARK_SHARE_PTR<AFNodeMeta> pDataMeta) = 0;

    virtual AFINode* FindData(const uint32_t index, bool bCreate = false) = 0;

    virtual bool IsEmpty() const = 0;

    virtual const DataList& GetDataList() = 0;
    virtual ARK_SHARE_PTR<AFClassMeta> GetClassMeta() = 0;
};

} // namespace ark
