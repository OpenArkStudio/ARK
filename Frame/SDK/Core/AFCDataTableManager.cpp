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

    for (size_t i = 0; i < mxTableCallbacks.GetCount(); ++i)
    {
        delete mxTableCallbacks[i];
    }

    mxTables.Clear();
    mxTableCallbacks.Clear();
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

void AFCDataTableManager::OnEventHandler(const AFGUID& entity_id, const DATA_TABLE_EVENT_DATA& xEventData, const AFCData& oldData, const AFCData& newData)
{
    for (auto& iter : mxTableCommonCallbacks)
    {
        (*iter)(entity_id, xEventData, oldData, newData);
    }

    AFTableCallBack* pTableCallBack = mxTableCallbacks.GetElement(xEventData.strName.c_str());

    if (nullptr != pTableCallBack)
    {
        for (auto& iter : pTableCallBack->mxCallbackList)
        {
            (*iter)(entity_id, xEventData, oldData, newData);
        }
    }
}

bool AFCDataTableManager::AddTableInternal(AFDataTable* pTable)
{
    assert(pTable != nullptr);

    return mxTables.AddElement(pTable->GetName(), pTable);
}

bool AFCDataTableManager::AddTable(const AFGUID& self_id, const char* table_name, const AFIDataList& col_type_list, const AFFeatureType feature)
{
    ARK_ASSERT(table_name != nullptr && sizeof(table_name) > 0, "Table name is invalid", __FILE__, __FUNCTION__);

    AFDataTable* pTable = ARK_NEW AFDataTable();
    pTable->SetName(table_name);
    pTable->SetColCount(col_type_list.GetCount());

    for (size_t i = 0; i < col_type_list.GetCount(); ++i)
    {
        pTable->SetColType(i, col_type_list.GetType(i));
    }

    pTable->SetFeature(feature);

    return AddTableInternal(pTable);
}

bool AFCDataTableManager::AddTableCallback(const char* table_name, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb)
{
    AFTableCallBack* pCallBackList = mxTableCallbacks.GetElement(table_name);

    if (!pCallBackList)
    {
        pCallBackList = new AFTableCallBack();
        mxTableCallbacks.AddElement(table_name, pCallBackList);
    }

    pCallBackList->mxCallbackList.push_back(cb);
    return true;
}

bool AFCDataTableManager::AddTableCommonCallback(const DATA_TABLE_EVENT_FUNCTOR_PTR& cb)
{
    mxTableCommonCallbacks.push_back(cb);
    return true;
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

    //callback
    do
    {
        AFCData oldData;

        if (!GetTableData(name, row, col, oldData))
        {
            ARK_ASSERT_RET_VAL(0, false);
        }

        if (oldData.GetBool() == value)
        {
            return false;
        }

        if (!mxTableCommonCallbacks.empty() || mxTableCallbacks.ExistElement(name))
        {
            AFCData newData;
            newData.SetBool(value);

            DATA_TABLE_EVENT_DATA xTableEventData;
            xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
            xTableEventData.nRow = row;
            xTableEventData.nCol = col;
            xTableEventData.strName = name;

            OnEventHandler(self, xTableEventData, oldData, newData);
        }
    } while (0);

    return pTable->SetBool(row, col, value);
}

bool AFCDataTableManager::SetTableInt(const char* name, const int row, const int col, const int32_t value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    //callback
    do
    {
        AFCData oldData;

        if (!GetTableData(name, row, col, oldData))
        {
            ARK_ASSERT_RET_VAL(0, false);
        }

        if (oldData.GetInt() == value)
        {
            return false;
        }

        if (!mxTableCommonCallbacks.empty() || mxTableCallbacks.ExistElement(name))
        {
            AFCData newData;
            newData.SetInt(value);

            DATA_TABLE_EVENT_DATA xTableEventData;
            xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
            xTableEventData.nRow = row;
            xTableEventData.nCol = col;
            xTableEventData.strName = name;

            OnEventHandler(self, xTableEventData, oldData, newData);
        }
    } while (0);

    return pTable->SetInt(row, col, value);
}

bool AFCDataTableManager::SetTableInt64(const char* name, const int row, const int col, const int64_t value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    //callback
    do
    {
        AFCData oldData;

        if (!GetTableData(name, row, col, oldData))
        {
            ARK_ASSERT_RET_VAL(0, false);
        }

        if (oldData.GetInt64() == value)
        {
            return false;
        }

        if (!mxTableCommonCallbacks.empty() || mxTableCallbacks.ExistElement(name))
        {
            AFCData newData;
            newData.SetInt64(value);

            DATA_TABLE_EVENT_DATA xTableEventData;
            xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
            xTableEventData.nRow = row;
            xTableEventData.nCol = col;
            xTableEventData.strName = name;

            OnEventHandler(self, xTableEventData, oldData, newData);
        }
    } while (0);

    return pTable->SetInt64(row, col, value);
}

bool AFCDataTableManager::SetTableFloat(const char* name, const int row, const int col, const float value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    //callback
    do
    {
        AFCData oldData;

        if (!GetTableData(name, row, col, oldData))
        {
            ARK_ASSERT_RET_VAL(0, false);
        }

        if (AFMisc::IsFloatEqual(oldData.GetFloat(), value))
        {
            return false;
        }

        if (!mxTableCommonCallbacks.empty() || mxTableCallbacks.ExistElement(name))
        {
            AFCData newData;
            newData.SetFloat(value);

            DATA_TABLE_EVENT_DATA xTableEventData;
            xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
            xTableEventData.nRow = row;
            xTableEventData.nCol = col;
            xTableEventData.strName = name;

            OnEventHandler(self, xTableEventData, oldData, newData);
        }
    } while (0);

    return pTable->SetFloat(row, col, value);
}

bool AFCDataTableManager::SetTableDouble(const char* name, const int row, const int col, const double value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    //callback
    do
    {
        AFCData oldData;

        if (!GetTableData(name, row, col, oldData))
        {
            ARK_ASSERT_RET_VAL(0, false);
        }

        if (AFMisc::IsDoubleEqual(oldData.GetDouble(), value))
        {
            return false;
        }

        if (!mxTableCommonCallbacks.empty() || mxTableCallbacks.ExistElement(name))
        {
            AFCData newData;
            newData.SetDouble(value);

            DATA_TABLE_EVENT_DATA xTableEventData;
            xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
            xTableEventData.nRow = row;
            xTableEventData.nCol = col;
            xTableEventData.strName = name;

            OnEventHandler(self, xTableEventData, oldData, newData);
        }
    } while (0);

    return pTable->SetDouble(row, col, value);
}

bool AFCDataTableManager::SetTableString(const char* name, const int row, const int col, const char* value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    //callback
    do
    {
        AFCData oldData;

        if (!GetTableData(name, row, col, oldData))
        {
            ARK_ASSERT_RET_VAL(0, false);
        }

        if (ARK_STRICMP(oldData.GetString(), value) == 0)
        {
            return false;
        }

        if (!mxTableCommonCallbacks.empty() || mxTableCallbacks.ExistElement(name))
        {
            AFCData newData;
            newData.SetString(value);

            DATA_TABLE_EVENT_DATA xTableEventData;
            xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
            xTableEventData.nRow = row;
            xTableEventData.nCol = col;
            xTableEventData.strName = name;

            OnEventHandler(self, xTableEventData, oldData, newData);
        }
    } while (0);

    return pTable->SetString(row, col, value);
}

bool AFCDataTableManager::SetTableObject(const char* name, const int row, const int col, const AFGUID& value)
{
    AFDataTable* pTable = GetTable(name);

    if (pTable == nullptr)
    {
        return false;
    }

    //callback
    do
    {
        AFCData oldData;

        if (!GetTableData(name, row, col, oldData))
        {
            ARK_ASSERT_RET_VAL(0, false);
        }

        if (oldData.GetObject() == value)
        {
            return false;
        }

        if (!mxTableCommonCallbacks.empty() || mxTableCallbacks.ExistElement(name))
        {
            AFCData newData;
            newData.SetObject(value);

            DATA_TABLE_EVENT_DATA xTableEventData;
            xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
            xTableEventData.nRow = row;
            xTableEventData.nCol = col;
            xTableEventData.strName = name;

            OnEventHandler(self, xTableEventData, oldData, newData);
        }
    } while (0);

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