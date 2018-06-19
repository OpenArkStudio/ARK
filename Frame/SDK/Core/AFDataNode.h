/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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

#include "SDK/Core/Base/AFDefine.h"
#include "SDK/Core/Base/AFCDataList.h"
#include "SDK/Core/Base/AFString.hpp"
#include "SDK/Core/Base/AFArrayPod.hpp"
#include "SDK/Core/Base/AFStringPod.hpp"
#include "SDK/Core/Base/AFCData.h"
#include "SDK/Core/Base/AFBitValue.hpp"

class AFDataNode
{
public:
    enum DATA_NODE_FEATURE
    {
        PF_PUBLIC = 0, //send to others
        PF_PRIVATE = 1, //send to self
        PF_REAL_TIME = 2, //send real-time when changed
        PF_SAVE = 3, //if need save to database
    };

    DataNodeName name = ""; //DataNode name, max = 16 bytes
    AFCData value;          //DataNode value
    int8_t feature = 0;     //DataNode feature

    bool IsPublic() const
    {
        return AFBitValue<int8_t>::HaveBitValue(feature, PF_PUBLIC);
    }

    bool IsPrivate() const
    {
        return AFBitValue<int8_t>::HaveBitValue(feature, PF_PRIVATE);
    }

    bool IsRealTime() const
    {
        return AFBitValue<int8_t>::HaveBitValue(feature, PF_REAL_TIME);
    }

    bool IsSave() const
    {
        return AFBitValue<int8_t>::HaveBitValue(feature, PF_SAVE);
    }

    bool Changed() const
    {
        return !(value.IsNullValue());
    }

    const DataNodeName& GetName()
    {
        return name;
    }

    const AFIData& GetValue()
    {
        return value;
    }

    int GetType()
    {
        return value.GetType();
    }

    int8_t GetFeature()
    {
        return feature;
    }
};
