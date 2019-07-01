/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2019 ArkNX authors.
*
* Licensed under the Apache License, Version 2.0 (the "License").
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

#include "AFCoreDef.hpp"
#include "AFArrayMap.hpp"
#include "AFDataTable.hpp"
#include "interface/AFIDataTableManager.h"

namespace ark
{

    class AFCDataTableManager : public AFIDataTableManager
    {
    protected:

    public:
        AFCDataTableManager() = delete;

        explicit AFCDataTableManager(const AFGUID& guid) :
            self(guid)
        {
        }

        ~AFCDataTableManager() override
        {
            ReleaseAll();
        }

        const AFGUID& Self() override
        {
            return self;
        }

        bool Exist(const char* name) const override
        {
            return mxTables.ExistElement(name);
        }

        bool Exist(const char* name, size_t& index) const override
        {
            return mxTables.ExistElement(name, index);
        }

        bool AddTable(const AFGUID& self_id, const char* table_name, const AFIDataList& col_type_list, const AFFeatureType feature) override
        {
            ARK_ASSERT(table_name != nullptr && sizeof(table_name) > 0, "Table name is invalid", __FILE__, __FUNCTION__);

            AFDataTable* pTable = ARK_NEW AFDataTable();
            pTable->SetName(table_name);
            pTable->SetColCount(col_type_list.GetCount());

            LITLE_DATA_TABLE_EVENT_FUNCTOR functor = std::bind(&AFCDataTableManager::OnEventHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            pTable->RegisterCallback(std::move(functor));
            for (size_t i = 0; i < col_type_list.GetCount(); ++i)
            {
                pTable->SetColType(i, col_type_list.GetType(i));
            }

            pTable->SetFeature(feature);
            return AddTableInternal(pTable);
        }

        bool RegisterCallback(DATA_TABLE_EVENT_FUNCTOR&& cb) override
        {
            mxTablecallbacks.push_back(std::forward<DATA_TABLE_EVENT_FUNCTOR>(cb));
            return true;
        }

        void Clear() final override
        {
            ReleaseAll();
        }

        AFDataTable* GetTable(const char* name) override
        {
            return mxTables.GetElement(name);
        }

        size_t GetCount() const override
        {
            return mxTables.GetCount();
        }

        AFDataTable* GetTableByIndex(size_t index) override
        {
            ARK_ASSERT_RET_VAL(index < GetCount(), nullptr);
            return mxTables[index];
        }

        bool SetTableBool(const char* name, const int row, const int col, const bool value) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? false : pTable->SetBool(row, col, value));
        }

        bool SetTableInt(const char* name, const int row, const int col, const int32_t value) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? false : pTable->SetInt(row, col, value));
        }

        bool SetTableInt64(const char* name, const int row, const int col, const int64_t value) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? false : pTable->SetInt64(row, col, value));
        }

        bool SetTableFloat(const char* name, const int row, const int col, const float value) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? false : pTable->SetFloat(row, col, value));
        }

        bool SetTableDouble(const char* name, const int row, const int col, const double value) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? false : pTable->SetDouble(row, col, value));
        }

        bool SetTableString(const char* name, const int row, const int col, const char* value) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? false : pTable->SetString(row, col, value));
        }


        bool GetTableBool(const char* name, const int row, const int col) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? false : pTable->GetBool(row, col));
        }

        int32_t GetTableInt(const char* name, const int row, const int col) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? NULL_INT : pTable->GetInt(row, col));
        }

        int64_t GetTableInt64(const char* name, const int row, const int col) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? NULL_INT64 : pTable->GetInt64(row, col));
        }

        float GetTableFloat(const char* name, const int row, const int col) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? NULL_FLOAT : pTable->GetFloat(row, col));
        }

        double GetTableDouble(const char* name, const int row, const int col) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? NULL_DOUBLE : pTable->GetDouble(row, col));
        }

        const char* GetTableString(const char* name, const int row, const int col) override
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable == nullptr) ? NULL_STR.c_str() : pTable->GetString(row, col));
        }

    protected:
        bool GetTableData(const char* name, const int row, const int col, AFIData& value)
        {
            AFDataTable* pTable = GetTable(name);
            return ((pTable != nullptr) ? pTable->GetValue(row, col, value) : false);
        }

        int OnEventHandler(const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldData, const AFIData& newData)
        {
            for (auto& cb : mxTablecallbacks)
            {
                cb(self, xEventData, oldData, newData);
            }

            return 0;
        }

        bool AddTableInternal(AFDataTable* pTable)
        {
            assert(pTable != nullptr);

            LITLE_DATA_TABLE_EVENT_FUNCTOR functor = std::bind(&AFCDataTableManager::OnEventHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            pTable->RegisterCallback(std::move(functor));

            return mxTables.AddElement(pTable->GetName(), pTable);
        }

        void ReleaseAll()
        {
            for (size_t i = 0; i < mxTables.GetCount(); ++i)
            {
                AFDataTable* pTable = mxTables[i];
                ARK_DELETE(pTable);
            }

            mxTables.Clear();
        }

    private:
        AFGUID self;

        AFArrayMap<std::string, AFDataTable> mxTables;
        std::vector<DATA_TABLE_EVENT_FUNCTOR> mxTablecallbacks;

    };

}