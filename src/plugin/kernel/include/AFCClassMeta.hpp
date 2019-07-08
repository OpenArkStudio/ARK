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

#include "interface/AFIClassMeta.h"
#include "AFMap.hpp"
#include "AFCDataMeta.hpp"

namespace ark {

class AFCEntityClassMeta : public AFIClassMeta
{
public:
    AFIDataMeta *FindDataMeta(const std::string &name) const
    {
        return entity_data_metas_.find_value(name);
    }

private:
    AFNewHashmap<std::string, AFCEntityDataMeta> entity_data_metas_;
};

class AFCConfigClassMeta : public AFIClassMeta
{
public:
    AFIDataMeta *FindDataMeta(const std::string &name) const
    {
        return config_data_metas_.find_value(name);
    }

private:
    AFNewHashmap<std::string, AFCConfigDataMeta> config_data_metas_;
};
} // namespace ark