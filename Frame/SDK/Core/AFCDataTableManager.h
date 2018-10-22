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

#include "SDK/Core/AFCoreDef.hpp"
#include "SDK/Core/AFArrayMap.hpp"
#include "AFIDataTableManager.h"

namespace ark
{

    class AFCDataTableManager : public AFIDataTableManager
    {
    protected:

    public:
        AFCDataTableManager() = delete;

        explicit AFCDataTableManager(const AFGUID& guid);
        ~AFCDataTableManager() override;

        const AFGUID& Self() override;

        bool Exist(const char* name) const override;
        bool Exist(const char* name, size_t& index) const override;

        bool AddTable(const AFGUID& self_id, const char* table_name, const AFIDataList& col_type_list, const AFFeatureType feature) override;
        bool RegisterCallback(const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) override;

        void Clear() final override;
        AFDataTable* GetTable(const char* name) override;
        size_t GetCount() const override;
        AFDataTable* GetTableByIndex(size_t index) override;

        bool SetTableBool(const char* name, const int row, const int col, const bool value) override;
        bool SetTableInt(const char* name, const int row, const int col, const int32_t value) override;
        bool SetTableInt64(const char* name, const int row, const int col, const int64_t value) override;
        bool SetTableFloat(const char* name, const int row, const int col, const float value) override;
        bool SetTableDouble(const char* name, const int row, const int col, const double value) override;
        bool SetTableString(const char* name, const int row, const int col, const char* value) override;
        bool SetTableObject(const char* name, const int row, const int col, const AFGUID& value) override;

        bool GetTableBool(const char* name, const int row, const int col) override;
        int32_t GetTableInt(const char* name, const int row, const int col) override;
        int64_t GetTableInt64(const char* name, const int row, const int col) override;
        float GetTableFloat(const char* name, const int row, const int col) override;
        double GetTableDouble(const char* name, const int row, const int col) override;
        const char* GetTableString(const char* name, const int row, const int col) override;
        const AFGUID GetTableObject(const char* name, const int row, const int col) override;

    protected:
        bool GetTableData(const char* name, const int row, const int col, AFIData& value);
        int OnEventHandler(const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldData, const AFIData& newData);

        bool AddTableInternal(AFDataTable* pTable);
        void ReleaseAll();

    private:
        AFGUID self;

        AFArrayMap<std::string, AFDataTable> mxTables;
        std::vector<DATA_TABLE_EVENT_FUNCTOR_PTR> mxTablecallbacks;

    };

}