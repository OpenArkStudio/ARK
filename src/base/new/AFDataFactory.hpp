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

#include "interface/AFIDataEx.hpp"
#include "AFEnum.hpp"

namespace ark
{

    class AFDataFactory
    {
    public:
        static AFIDataEx* Create(ArkDataType type)
        {
            switch (type)
            {
            case DT_BOOLEAN:
                return ARK_NEW(AFBoolData);
                break;
            case DT_INT32:
                return ARK_NEW(AFInt32Data);
                break;
            case DT_UINT32:
                return ARK_NEW(AFUInt32Data);
                break;
            case DT_INT64:
                return ARK_NEW(AFInt64Data);
                break;
            case DT_UINT64:
                return ARK_NEW(AFUInt64Data);
                break;
            case DT_FLOAT:
                return ARK_NEW(AFFloatData);
                break;
            case DT_DOUBLE:
                return ARK_NEW(AFDoubleData);
                break;
            case DT_STRING:
                return ARK_NEW(AFStringData);
                break;
            case DT_VECTOR3D:
                return ARK_NEW(AFVector3D);
                break;
            case DT_STRUCT:
                return ARK_NEW(AFStructData);
                break;
            case DT_TABLE:
                return ARK_NEW(AFTableData);
                break;
            case DT_ARRAY:
                return ARK_NEW(AFArrayData);
                break;
            default:
                ARK_ASSERT_RET_VAL(0, nullptr);
                break;
            }

            return nullptr;
        }

        static AFIDataEx* Create(const std::string& type)
        {
            ArkDataType type = AFMisc::CovertDataType(type);
            return Create(type);
        }

        static void Release(AFIDataEx* data)
        {
            ARK_DELETE(data);
        }

        static AFIDataEx* CreateData(const AFDataSetting* data_setting)
        {
            //
        }

        static AFIDataEx* CreateData(const std::string& meta_class_name, const std::string& data_name)
        {
            //
        }

        static void InitData(AFIDataEx* data, const AFMetaClassSetting* meta_class, const AFDataSetting* data_setting)
        {
            //
        }
    };

}