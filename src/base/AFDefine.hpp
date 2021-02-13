/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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
#include "base/math/AFVector3D.hpp"

namespace ark {

class AFIData;
class AFIDataList;

// default value of build-in type
const static bool NULL_BOOLEAN = false;
const static int NULL_INT = 0U;
const static int64_t NULL_INT64 = 0L;
const static float NULL_FLOAT = 0.0F;
const static double NULL_DOUBLE = 0.0;
const static std::string NULL_STR = "";
const static std::wstring NULL_WIDESTR = L"";
const static guid_t NULL_GUID = 0;
const static std::nullptr_t NULL_POINT = nullptr;
const static AFVector3D NULL_VECTOR3D = AFVector3D(0.0F, 0.0F, 0.0F);

// kernel resource filename
const static std::string CONFIG_CLASS_FILE_PATH = "meta/config_class.config";
const static std::string ENTITY_CLASS_FILE_PATH = "meta/entity_class.config";
//const static std::string PROC_CONFIG_FILE_PATH = "conf/proc.xml";
//const static std::string BUS_RELATION_CONFIG_FILE_PATH = "conf/bus_relation.xml";
//const static std::string REG_CENTER_CONFIG_FILE_PATH = "conf/reg_center.xml";

/// data define(only support based integer type)
using ID_TYPE = uint32_t;

using ArkMaskType = std::bitset<16>;

/// Table callback data
class TABLE_EVENT_DATA
{
public:
    TABLE_EVENT_DATA() = default;

    uint8_t op_type_{0};
    uint32_t row_{0};
    uint32_t data_index_{0};
    uint32_t table_index_{0};
    std::string table_name_{NULL_STR};
};

using DATA_TABLE_EVENT_FUNCTOR =
    std::function<int(const guid_t&, const TABLE_EVENT_DATA&, const AFIData&, const AFIData&)>;

using DATA_NODE_EVENT_FUNCTOR =
    std::function<int(const guid_t&, const std::string&, const uint32_t, const AFIData&, const AFIData&)>;

using CONTAINER_EVENT_FUNCTOR =
    std::function<int(const guid_t&, const uint32_t, const ArkContainerOpType, const uint32_t, const uint32_t)>;

using CLASS_EVENT_FUNCTOR =
    std::function<bool(const guid_t&, const std::string&, const ArkEntityEvent, const AFIDataList&)>;
using SCENE_EVENT_FUNCTOR = std::function<bool(const guid_t&, const int, const int)>;
using MOVE_EVENT_FUNCTOR = std::function<bool(const guid_t&, const AFVector3D&, const AFVector3D&)>;
using EVENT_PROCESS_FUNCTOR = std::function<int(const guid_t&, const int, const AFIDataList&)>;
using TIMER_FUNCTOR = std::function<void(uint64_t, const guid_t&)>;
using SCHEDULER_FUNCTOR = std::function<void()>;

} // namespace ark
