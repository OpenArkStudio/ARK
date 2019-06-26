/*
* This source file is part of ARK
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

namespace ark
{
    //TODO: delete
    enum AF_DATA_TYPE
    {
        DT_UNKNOWN,     //unknown type
        DT_BOOLEAN,     //bool
        DT_INT,         //int32_t
        DT_INT64,       //int64_t
        DT_FLOAT,       //float
        DT_DOUBLE,      //double
        DT_STRING,      //string(char*)
        DT_POINTER,     //pointer(void*)
        DT_USERDATA,    //user data(char*)
        DT_TABLE,       //table
        DT_MAX,         //max
    };

    enum class ArkDataType : int32_t
    {
        DT_EMPTY,           //unknown
        DT_BOOLEAN,         //bool
        DT_INT32,           //int32
        DT_UINT32,          //uint32
        DT_INT64,           //int64
        DT_UINT64,          //uint64
        DT_FLOAT,           //float
        DT_DOUBLE,          //double
        DT_STRING,          //string
        DT_VECTOR3D,        //vector3d
        DT_ARRAY,           //array
        DT_TABLE,           //DataTable
        DT_OBJECT,          //object(complex data container)
    };

    enum class ArkDataMaskType : int
    {
        DMT_UNKNOWN,        //unknown
        DMT_SAVE_DB,        //save to database
        DMT_SYNC_SELF,      //sync to self
        DMT_SYNC_VIEW,      //sync to view range
        DMT_SYNC_TEAM,      //sync to team member
        DMT_SYNC_GUILD,     //sync to guild member
        DMT_SYNC_MAP,       //sync to all player in same map
        DMT_LOG,            //log when changed
    };

    enum class ArkBusRelationType : int
    {
        BRT_UNKNOWN,       //unknown
        BRT_DIRECT,        //directly
        BRT_WAIT_NOTIFY,   //wait notify
        BRT_RECORD,        //do not connect, but need record info
    };

    enum class ArkEntityEvent : int
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
        ENTITY_EVT_ALL_FINISHED,        //Call it by yourself when create entity finished
    };

    enum class ArkDataOpType : uint16_t
    {
        DOT_UNKNOWN = 0,
        DOT_INC,     // +
        DOT_DEC,     // -
        DOT_MUL,     // *
        DOT_DIV,     // /
        DOT_SET,     // =
        DOT_AND,     // &
        DOT_OR,      // |
    };
}