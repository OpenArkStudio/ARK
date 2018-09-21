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

#include "SDK/Core/AFMisc.hpp"
#include "AFCDataTableManager.h"
#include "AFDataTable.h"

AFCDataTableManager::AFCDataTableManager(const AFGUID& guid)
    : self(guid)
{

}

AFCDataTableManager::~AFCDataTableManager()
{
    ReleaseAll();
}

const AFGUID& AFCDataTableManager::Self()
{
    return self;
}

void AFCDataTableManager::ReleaseAll()
{
    for (size_t i = 0; i < mxTables.GetCount(); ++i)
    {
        delete mxTables[i];
    }
    mxTables.Clear();
}

bool AFCDataTableManager::Exist(const char* name) const
{
    return mxTables.ExistElement(name);
}

bool AFCDataTableManager::Exist(const char* name, size_t& index) const
{
    return mxTables.ExistElement(name, index);
}

bool AFCDataTableManager::GetTableData(const char* name, const int row, const int col, AFIData& value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return NULL;
    }

    return pTable->GetValue(row, col, value);
}

int AFCDataTableManager::OnEventHandler(const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldData, const AFIData& newData)
{
    for (auto& iter : mxTablecallbacks)
    {
        (*iter)(self, xEventData, oldData, newData);
    }

    return 0;
}

bool AFCDataTableManager::RegisterCallback(const DATA_TABLE_EVENT_FUNCTOR_PTR& cb)
{
    mxTablecallbacks.push_back(cb);
    return true;
}

bool AFCDataTableManager::AddTableInternal(AFDataTable* pTable)
{
    assert(pTable != nullptr);

    LITLE_DATA_TABLE_EVENT_FUNCTOR functor = std::bind(&AFCDataTableManager::OnEventHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    pTable->RegisterCallback(std::make_shared<LITLE_DATA_TABLE_EVENT_FUNCTOR>(functor));

    return mxTables.AddElement(pTable->GetName(), pTable);
}

bool AFCDataTableManager::AddTable(const AFGUID& self_id, const char* table_name, const AFIDataList& col_type_list, const AFFeatureType feature)
{
    ARK_ASSERT(table_name != nullptr && sizeof(table_name) > 0, "Table name is invalid", __FILE__, __FUNCTION__);

    AFDataTable* pTable = ARK_NEW AFDataTable();
    pTable->SetName(table_name);
    pTable->SetColCount(col_type_list.GetCount());

    LITLE_DATA_TABLE_EVENT_FUNCTOR functor = std::bind(&AFCDataTableManager::OnEventHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    pTable->RegisterCallback(std::make_shared<LITLE_DATA_TABLE_EVENT_FUNCTOR>(functor));
    for (size_t i = 0; i < col_type_list.GetCount(); ++i)
    {
        pTable->SetColType(i, col_type_list.GetType(i));
    }

    pTable->SetFeature(feature);
    return AddTableInternal(pTable);
}

void AFCDataTableManager::Clear()
{
    ReleaseAll();
}

AFDataTable* AFCDataTableManager::GetTable(const char* name)
{
    return mxTables.GetElement(name);
}

size_t AFCDataTableManager::GetCount() const
{
    return mxTables.GetCount();
}

AFDataTable* AFCDataTableManager::GetTableByIndex(size_t index)
{
    ARK_ASSERT_RET_VAL(index < GetCount(), NULL);
    return mxTables[index];
}

bool AFCDataTableManager::SetTableBool(const char* name, const int row, const int col, const bool value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    return pTable->SetBool(row, col, value);
}

bool AFCDataTableManager::SetTableInt(const char* name, const int row, const int col, const int32_t value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    return pTable->SetInt(row, col, value);
}

bool AFCDataTableManager::SetTableInt64(const char* name, const int row, const int col, const int64_t value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    return pTable->SetInt64(row, col, value);
}

bool AFCDataTableManager::SetTableFloat(const char* name, const int row, const int col, const float value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    return pTable->SetFloat(row, col, value);
}

bool AFCDataTableManager::SetTableDouble(const char* name, const int row, const int col, const double value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    return pTable->SetDouble(row, col, value);
}

bool AFCDataTableManager::SetTableString(const char* name, const int row, const int col, const char* value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    return pTable->SetString(row, col, value);
}

bool AFCDataTableManager::SetTableObject(const char* name, const int row, const int col, const AFGUID& value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    return pTable->SetObject(row, col, value);
}

bool AFCDataTableManager::GetTableBool(const char* name, const int row, const int col)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    return pTable->GetBool(row, col);
}

int32_t AFCDataTableManager::GetTableInt(const char* name, const int row, const int col)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return NULL_INT;
    }

    return pTable->GetInt(row, col);
}

int64_t AFCDataTableManager::GetTableInt64(const char* name, const int row, const int col)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return NULL_INT64;
    }

    return pTable->GetInt64(row, col);
}

float AFCDataTableManager::GetTableFloat(const char* name, const int row, const int col)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return NULL_FLOAT;
    }

    return pTable->GetFloat(row, col);
}

double AFCDataTableManager::GetTableDouble(const char* name, const int row, const int col)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return NULL_DOUBLE;
    }

    return pTable->GetDouble(row, col);
}

const char* AFCDataTableManager::GetTableString(const char* name, const int row, const int col)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return NULL_STR.c_str();
    }

    return pTable->GetString(row, col);
}

const AFGUID AFCDataTableManager::GetTableObject(const char* name, const int row, const int col)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return NULL_GUID;
    }

    return pTable->GetObject(row, col);
}