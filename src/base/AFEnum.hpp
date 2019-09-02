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

#include "base/AFPlatform.hpp"

namespace ark {

enum class ArkDataType : uint32_t
{
    DT_EMPTY,    // unknown
    DT_BOOLEAN,  // bool
    DT_INT32,    // int32
    DT_UINT32,   // uint32
    DT_INT64,    // int64
    DT_UINT64,   // uint64
    DT_FLOAT,    // float
    DT_DOUBLE,   // double
    DT_STRING,   // string
    DT_WSTRING,  // wstring
    DT_VECTOR3D, // vector3d
    DT_ARRAY,    // array
    DT_TABLE,    // DataTable
    DT_GUID,     // guid
    DT_POINTER,  // pointer(void*)
    DT_USERDATA, // user data(char*)
};

enum class ArkBusRelationType : uint32_t
{
    BRT_UNKNOWN,     // unknown
    BRT_DIRECT,      // directly
    BRT_WAIT_NOTIFY, // wait notify
    BRT_RECORD,      // do not connect, but need record info
};

enum class ArkEntityEvent : uint32_t
{
    ENTITY_EVT_NONE = 0,
    ENTITY_EVT_DESTROY,
    ENTITY_EVT_PRE_DESTROY,
    ENTITY_EVT_PRE_LOAD_DATA,
    ENTITY_EVT_LOAD_DATA,
    ENTITY_EVT_PRE_EFFECT_DATA,
    ENTITY_EVT_EFFECT_DATA,
    ENTITY_EVT_POST_EFFECT_DATA,
    ENTITY_EVT_DATA_FINISHED,
    ENTITY_EVT_ALL_FINISHED, // Call it by yourself when create entity finished
};

enum class ArkDataOpType : uint16_t
{
    DOT_UNKNOWN = 0,
    DOT_INC, // +
    DOT_DEC, // -
    DOT_MUL, // *
    DOT_DIV, // /
    DOT_SET, // =
    DOT_AND, // &
    DOT_OR,  // |
};

enum class ArkTableOpType : uint16_t // table operate type
{
    TABLE_ADD = 0,  // add whole row data
    TABLE_DELETE,   // delete whole row data
    TABLE_UPDATE,   // update row & col cell data
    TABLE_COVERAGE, // coverage whole row data
    TABLE_SWAP,     // swap two whole row data
};

enum class ArkTableNodeFeature : uint16_t
{
    PF_PUBLIC = 0,    // send to others
    PF_PRIVATE = 1,   // send to self
    PF_REAL_TIME = 2, // send real-time when changed
    PF_SAVE = 3,      // if need save to database
};

enum class AFNodeFeature : uint16_t
{
    PF_PUBLIC = 0,     // send to others
    PF_PRIVATE = 1,    // send to self
    PF_REAL_TIME = 2,  // send real-time when changed
    PF_SAVE = 3,       // save to database
    PF_SYNC_SELF = 4,  // sync to self
    PF_SYNC_VIEW = 5,  // sync to view range
    PF_SYNC_TEAM = 6,  // sync to team member
    PF_SYNC_GUILD = 7, // sync to guild member
    PF_SYNC_MAP = 8,   // sync to all player in same map
    PF_LOG = 9,        // log when changed
};

} // namespace ark
