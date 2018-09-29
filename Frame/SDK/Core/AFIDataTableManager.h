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

#include "SDK/Core/AFGUID.hpp"
#include "SDK/Core/AFIDataList.hpp"
#include "SDK/Core/AFDefine.hpp"

namespace ark
{

    class AFDataTable;

    class AFIDataTableManager
    {
    public:
        virtual ~AFIDataTableManager() = default;

        virtual const AFGUID& Self() = 0;

        virtual bool Exist(const char* name) const = 0;
        virtual bool Exist(const char* name, size_t& index) const = 0;

        template<typename BaseType>
        bool RegisterCallback(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
        {
            DATA_TABLE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
            return RegisterCallback(std::make_shared<DATA_TABLE_EVENT_FUNCTOR>(functor));
        }

        virtual bool RegisterCallback(const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) = 0;
        virtual bool AddTable(const AFGUID& self_id, const char* table_name, const AFIDataList& col_type_list, const AFFeatureType feature) = 0;

        virtual void Clear() = 0;
        virtual AFDataTable* GetTable(const char* name) = 0;
        virtual size_t GetCount() const = 0;
        virtual AFDataTable* GetTableByIndex(size_t index) = 0;

        virtual bool SetTableBool(const char* name, const int row, const int col, const bool value) = 0;
        virtual bool SetTableInt(const char* name, const int row, const int col, const int32_t value) = 0;
        virtual bool SetTableInt64(const char* name, const int row, const int col, const int64_t value) = 0;
        virtual bool SetTableFloat(const char* name, const int row, const int col, const float value) = 0;
        virtual bool SetTableDouble(const char* name, const int row, const int col, const double value) = 0;
        virtual bool SetTableString(const char* name, const int row, const int col, const char* value) = 0;
        virtual bool SetTableObject(const char* name, const int row, const int col, const AFGUID& value) = 0;

        virtual bool GetTableBool(const char* name, const int row, const int col) = 0;
        virtual int32_t GetTableInt(const char* name, const int row, const int col) = 0;
        virtual int64_t GetTableInt64(const char* name, const int row, const int col) = 0;
        virtual float GetTableFloat(const char* name, const int row, const int col) = 0;
        virtual double GetTableDouble(const char* name, const int row, const int col) = 0;
        virtual const char* GetTableString(const char* name, const int row, const int col) = 0;
        virtual const AFGUID GetTableObject(const char* name, const int row, const int col) = 0;
    };

}