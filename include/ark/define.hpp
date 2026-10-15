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

#include "ark/enum.hpp"
#include "ark/macros.hpp"
#include "ark/math/vector3d.hpp"
#include "ark/meta_define.hpp"
#include "ark/string.hpp"

namespace ark {

class data;
class data_list;

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
const static vector3d NULL_VECTOR3D = vector3d(0.0F, 0.0F, 0.0F);

// kernel resource filename
const static std::string CONFIG_CLASS_FILE_PATH = "meta/config_class.config";
const static std::string ENTITY_CLASS_FILE_PATH = "meta/entity_class.config";
//const static std::string PROC_CONFIG_FILE_PATH = "conf/proc.xml";
//const static std::string BUS_RELATION_CONFIG_FILE_PATH = "conf/bus_relation.xml";
//const static std::string REG_CENTER_CONFIG_FILE_PATH = "conf/reg_center.xml";

/// data define(only support based integer type)
using id_type = uint32_t;

using mask_type = std::bitset<16>;

/// Table callback data
class table_event_data
{
public:
    table_event_data() = default;

    uint8_t op_type_{0};
    uint32_t row_{0};
    uint32_t data_index_{0};
    uint32_t table_index_{0};
    std::string table_name_{NULL_STR};
};

using data_table_event_functor = std::function<int(const guid_t&, const table_event_data&, const data&, const data&)>;

using data_node_event_functor =
    std::function<int(const guid_t&, const std::string&, const uint32_t, const data&, const data&)>;

using container_event_functor =
    std::function<int(const guid_t&, const uint32_t, const container_op_type, const uint32_t, const uint32_t)>;

using class_event_functor =
    std::function<bool(const guid_t&, const std::string&, const entity_event, const data_list&)>;
using scene_event_functor = std::function<bool(const guid_t&, const int, const int)>;
using move_event_functor = std::function<bool(const guid_t&, const vector3d&, const vector3d&)>;
using event_process_functor = std::function<int(const guid_t&, const int, const data_list&)>;
using timer_functor = std::function<void(uint64_t, const guid_t&)>;
using scheduler_functor = std::function<void()>;

} // namespace ark
