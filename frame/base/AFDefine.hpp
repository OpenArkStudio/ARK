/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
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

#include "AFMacros.hpp"
#include "AFEnum.hpp"
#include "AFString.hpp"
#include "AFMetaDefine.hpp"
#include "AFVector3D.hpp"

namespace ark
{
    class AFIData;
    class AFIDataList;

    const static bool NULL_BOOLEAN = false;
    const static int NULL_INT = 0U;
    const static int64_t NULL_INT64 = 0L;
    const static float NULL_FLOAT = 0.0F;
    const static double NULL_DOUBLE = 0.0;
    const static std::string NULL_STR = "";
    const static AFGUID NULL_GUID = 0;
    const static nullptr_t NULL_POINT = nullptr;
    const static AFVector3D NULL_VECTOR3D = AFVector3D(0.0f, 0.0f, 0.0f);

    static const std::string config_class_file = "meta/config_class.config";

    using AFFeatureType = std::bitset<8>;

    class DATA_TABLE_EVENT_DATA
    {
    public:
        DATA_TABLE_EVENT_DATA() = default;

        uint8_t nOpType{0};
        int16_t nRow{-1};
        int16_t nCol{-1};
        DataTableName strName{""};
    };

    using DATA_NODE_EVENT_FUNCTOR = std::function<int(const AFGUID&, const std::string&, const AFIData&, const AFIData&)>;
    using DATA_TABLE_EVENT_FUNCTOR = std::function<int(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&)>;
    using LITLE_DATA_TABLE_EVENT_FUNCTOR = std::function<int(const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&)>;
    using CLASS_EVENT_FUNCTOR = std::function<bool(const AFGUID&, const std::string&, const ArkEntityEvent, const AFIDataList&)>;
    using EVENT_PROCESS_FUNCTOR = std::function<int(const AFGUID&, const int, const AFIDataList&)>;
    using TIMER_FUNCTOR = std::function<void(const std::string&, const AFGUID&)>;
    using SCHEDULER_FUNCTOR = std::function<bool(const int, const int)>;

    using DATA_NODE_EVENT_FUNCTOR_PTR = ARK_SHARE_PTR<DATA_NODE_EVENT_FUNCTOR>;
    using DATA_TABLE_EVENT_FUNCTOR_PTR = ARK_SHARE_PTR<DATA_TABLE_EVENT_FUNCTOR>;
    using LITLE_DATA_TABLE_EVENT_FUNCTOR_PTR = ARK_SHARE_PTR<LITLE_DATA_TABLE_EVENT_FUNCTOR>;
    using CLASS_EVENT_FUNCTOR_PTR = ARK_SHARE_PTR<CLASS_EVENT_FUNCTOR>;
    using EVENT_PROCESS_FUNCTOR_PTR = ARK_SHARE_PTR<EVENT_PROCESS_FUNCTOR>;
    using TIMER_FUNCTOR_PTR = ARK_SHARE_PTR<TIMER_FUNCTOR>;
    using SCHEDULER_FUNCTOR_PTR = ARK_SHARE_PTR<SCHEDULER_FUNCTOR>;
}