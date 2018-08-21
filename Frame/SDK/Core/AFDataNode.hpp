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

#include "SDK/Core/AFDefine.hpp"
#include "SDK/Core/AFCDataList.hpp"
#include "SDK/Core/AFString.hpp"
#include "SDK/Core/AFArrayPod.hpp"
#include "SDK/Core/AFStringPod.hpp"
#include "SDK/Core/AFCData.hpp"

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
    AFFeatureType feature; //DataNode feature

    bool IsPublic() const
    {
        return feature.test(PF_PUBLIC);
    }

    bool IsPrivate() const
    {
        return feature.test(PF_PRIVATE);
    }

    bool IsRealTime() const
    {
        return feature.test(PF_REAL_TIME);
    }

    bool IsSave() const
    {
        return feature.test(PF_SAVE);
    }

    bool Changed() const
    {
        return !(value.IsNullValue());
    }

    const char* GetName()
    {
        return name.c_str();
    }

    const AFIData& GetValue()
    {
        return value;
    }

    int GetType()
    {
        return value.GetType();
    }

    const AFFeatureType& GetFeature()
    {
        return feature;
    }

    std::string ToString()
    {
        return value.ToString();
    }
};
