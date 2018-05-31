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

#include "SDK/Core/Base/AFCDataList.h"
#include "SDK/Core/Base/AFArrayPod.hpp"
#include "SDK/Core/Base/AFCoreDef.hpp"
#include "SDK/Core/Base/AFString.hpp"
#include "SDK/Core/Base/AFCData.h"
#include "SDK/Core/Base/AFBitValue.hpp"
#include "SDK/Core/Base/AFDefine.h"

class AFDataTable
{
private:
    using RowData = AFCData;

public:
    enum DATA_TABLE_FEATURE
    {
        TABLE_PUBLIC       = 0, //send to others
        TABLE_PRIVATE      = 1, //send to self
        TABLE_REAL_TIME    = 2, //send real-time when changed
        TABLE_SAVE         = 3, //if need save to database
    };

    enum DATA_TABLE_OP_TYPE
    {
        TABLE_ADD     = 0 , //add whole row data
        TABLE_DELETE      , //delete whole row data
        TABLE_UPDATE      , //update row & col cell data
        TABLE_COVERAGE    , //coverage whole row data
        TABLE_SWAP        , //swap two whole row data
    };

public:
    AFDataTable() noexcept;
    virtual ~AFDataTable();

    void SetName(const char* value);
    const char* GetName() const;

    size_t GetRowCount() const;

    void SetColCount(size_t value);
    size_t GetColCount() const;

    bool SetColType(size_t index, int type);
    int GetColType(int col) const;

    bool AddRow();
    bool AddRow(size_t row);
    bool AddRow(size_t row, const AFIDataList& data);

    bool DeleteRow(size_t row);

    void Clear();

    void SetFeature(int8_t new_feature);
    int8_t GetFeature() const;

    void SetPublic();
    bool IsPublic() const;
    void SetPrivate();
    bool IsPrivate() const;
    void SetRealTime();
    bool IsRealTime() const;
    void SetSave();
    bool IsSave() const;

    bool SetValue(size_t row, size_t col, const AFIData& value);
    bool SetBool(size_t row, size_t col, const bool value);
    bool SetInt(size_t row, size_t col, const int value);
    bool SetInt64(size_t row, size_t col, const int64_t value);
    bool SetFloat(size_t row, size_t col, const float value);
    bool SetDouble(size_t row, size_t col, const double value);
    bool SetString(size_t row, size_t col, const char* value);
    bool SetObject(size_t row, size_t col, const AFGUID& value);

    bool GetValue(size_t row, size_t col, AFIData& value);
    bool GetBool(size_t row, size_t col);
    int GetInt(size_t row, size_t col);
    int64_t GetInt64(size_t row, size_t col);
    float GetFloat(size_t row, size_t col);
    double GetDouble(size_t row, size_t col);
    const char* GetString(size_t row, size_t col);
    const AFGUID& GetObject(size_t row, size_t col);

    const char* GetStringValue(size_t row, size_t col);
    bool GetColTypeList(AFIDataList& col_type_list);

    int FindRow(size_t col, const AFIData& key, size_t begin_row = 0);
    int FindBool(size_t col, const bool key, size_t begin_row = 0);
    int FindInt(size_t col, const int key, size_t begin_row = 0);
    int FindInt64(size_t col, const int64_t key, size_t begin_row = 0);
    int FindFloat(size_t col, const float key, size_t begin_row = 0);
    int FindDouble(size_t col, const double key, size_t begin_row = 0);
    int FindString(size_t col, const char* key, size_t begin_row = 0);
    int FindObject(size_t col, const AFGUID& key, size_t begin_row = 0);

    bool QueryRow(const int row, AFIDataList& varList);

public:
    void ReleaseRow(RowData* row_data, size_t col_num);
    void ReleaseAll();

private:
    DataTableName mstrName;                         //DataTable name
    int8_t feature;                                 //DataTable feature
	ArrayPod<int, 1, CoreAlloc> mxColTypes;        //DataTable column type array
	ArrayPod<RowData*, 1, CoreAlloc> mxRowDatas;   //DataTable data array
};