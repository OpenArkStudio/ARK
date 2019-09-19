/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "base/AFMacros.hpp"
#include "base/AFEnum.hpp"
#include "base/AFString.hpp"
#include "base/AFMetaDefine.hpp"
#include "base/AFVector3D.hpp"

namespace ark {

static const std::string CONFIG_CLASS_FILE_PATH = "meta/config_class.config";
static const std::string ENTITY_CLASS_FILE_PATH = "meta/entity_class.config";

static const std::string PROC_CONFIG_FILE_PATH = "conf/proc.xml";
static const std::string BUS_RELATION_CONFIG_FILE_PATH = "conf/bus_relation.xml";
static const std::string REG_CENTER_CONFIG_FILE_PATH = "conf/reg_center.xml";

class AFIData;
class AFIDataList;

const static bool NULL_BOOLEAN = false;
const static int NULL_INT = 0U;
const static int64_t NULL_INT64 = 0L;
const static float NULL_FLOAT = 0.0F;
const static double NULL_DOUBLE = 0.0;
const static std::string NULL_STR = "";
const static std::wstring NULL_WIDESTR = L"";
const static AFGUID NULL_GUID = 0;
const static nullptr_t NULL_POINT = nullptr;
const static AFVector3D NULL_VECTOR3D = AFVector3D(0.0f, 0.0f, 0.0f);

//data define(only support based integer type)
using ID_TYPE = uint32_t;

using AFMaskType = std::bitset<16>;

// Table call back event data
class TABLE_EVENT_DATA
{
public:
    uint8_t op_type_{0u};
    size_t row_{0u};
    uint32_t data_index_{0u};
    uint32_t table_index_{0u};
    std::string table_name_{NULL_STR};
};

using DATA_NODE_EVENT_FUNCTOR = std::function<int(const AFGUID&, const std::string&, const uint32_t, const AFIData&, const AFIData&)>;
using DATA_TABLE_EVENT_FUNCTOR = std::function<int(const AFGUID&, const TABLE_EVENT_DATA&, const AFIData&, const AFIData&)>;
using CLASS_EVENT_FUNCTOR = std::function<bool(const AFGUID&, const std::string&, const ArkEntityEvent, const AFIDataList&)>;
using EVENT_PROCESS_FUNCTOR = std::function<int(const AFGUID&, const int, const AFIDataList&)>;
using TIMER_FUNCTOR = std::function<void(const std::string&, const AFGUID&)>;
using SCHEDULER_FUNCTOR = std::function<bool(const int, const int)>;

} // namespace ark
