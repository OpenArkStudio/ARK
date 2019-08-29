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

#include "AFITable.hpp"
#include "AFIData.hpp"
#include "kernel/include/AFTableMeta.hpp"

namespace ark {

class AFITableInner : public AFITable
{
public:
    virtual bool OnRowDataChanged(
        uint32_t row, const uint32_t index, const AFIData& old_data, const AFIData& new_data) const = 0;

    virtual ARK_SHARE_PTR<AFTableMeta> GetMeta() const = 0;
};

} // namespace ark
