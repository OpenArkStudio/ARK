/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "AFRecord.h"

AFRecord::AFRecord()
    : mstrName(NULL_STR.c_str())
    , feature(0)
{
}

AFRecord::~AFRecord()
{
    ReleaseAll();
}

void AFRecord::ReleaseRow(RowData* row_data, size_t col_num)
{
    for(size_t i = 0; i < col_num; ++i)
    {
        row_data[i].~AFBaseData();
    }

    delete row_data;
    row_data = NULL;
}

void AFRecord::ReleaseAll()
{
    size_t col_num = mxColTypes.size();
    for(size_t i = 0; i < mxRowDatas.size(); ++i)
    {
        if(NULL != mxRowDatas[i])
        {
            ReleaseRow(mxRowDatas[i], col_num);
        }
    }

    mxRowDatas.clear();
}

void AFRecord::SetName(const char* value)
{
    assert(NULL != value);
    mstrName = value;
}

const char* AFRecord::GetName() const
{
    return mstrName.c_str();
}

size_t AFRecord::GetRowCount() const
{
    return mxRowDatas.size();
}

void AFRecord::SetColCount(size_t value)
{
    assert(value > 0);
    if(mxColTypes.size() > 0)
    {
        ReleaseAll();
    }

    mxColTypes.resize(value);
}

size_t AFRecord::GetColCount() const
{
    return mxColTypes.size();
}

bool AFRecord::SetColType(size_t index, int type)
{
    assert(index < mxColTypes.size());
    assert(type > DT_UNKNOWN);

    mxColTypes[index] = type;
    return true;
}

int AFRecord::GetColType(int col) const
{
    assert(col < mxColTypes.size());

    return mxColTypes[col];
}

bool AFRecord::AddRow()
{
    //default insert row
    size_t col_num = GetColCount();
    RowData* row_data = new RowData[col_num];
    mxRowDatas.push_back(row_data);
    return true;
}

bool AFRecord::AddRow(size_t row)
{
    size_t col_num = GetColCount();
    RowData* row_data = new RowData[col_num];
    if(row >= GetRowCount())
    {
        mxRowDatas.push_back(row_data);
    }
    else
    {
        mxRowDatas.insert(row, row_data);
    }

    return true;
}

bool AFRecord::AddRow(size_t row, const AFIDataList& data)
{
    size_t col_num = GetColCount();
    if(data.GetCount() != col_num)
    {
        ARK_ASSERT(0, "data size is not equal with col_num, please check your arg.", __FILE__, __FUNCTION__);
        return false;
    }

    RowData* row_data = new RowData[col_num];
    for(size_t i = 0; i < data.GetCount(); ++i)
    {
        int type = GetColType(i);
        switch(type)
        {
        case DT_BOOLEAN:
            row_data[i].SetBool(data.Bool(i));
            break;
        case DT_INT:
            row_data[i].SetInt(data.Int(i));
            break;
        case DT_INT64:
            row_data[i].SetInt64(data.Int64(i));
            break;
        case DT_FLOAT:
            row_data[i].SetFloat(data.Float(i));
            break;
        case DT_DOUBLE:
            row_data[i].SetDouble(data.Double(i));
            break;
        case DT_STRING:
            row_data[i].SetString(data.String(i));
            break;
        case DT_OBJECT:
            row_data[i].SetObject(data.Object(i));
            break;
        default:
            {
                delete[] row_data;
                return false;
            }
            break;
        }
    }

    if(row >= GetRowCount())
    {
        mxRowDatas.push_back(row_data);
    }
    else
    {
        mxRowDatas.insert(row, row_data);
    }

    return true;
}

bool AFRecord::DeleteRow(size_t row)
{
    assert(row < mxRowDatas.size());

    ReleaseRow(mxRowDatas[row], mxColTypes.size());
    mxRowDatas.remove(row);

    return true;
}

void AFRecord::Clear()
{
    ReleaseAll();
}

void AFRecord::SetFeature(int8_t new_feature)
{
    this->feature = new_feature;
}

int8_t AFRecord::GetFeature() const
{
    return feature;
}

void AFRecord::SetPublic()
{
    BitValue<int8_t>::SetBitValue(feature, RF_PUBLIC);
}

bool AFRecord::IsPublic() const
{
    return BitValue<int8_t>::HaveBitValue(feature, RF_PUBLIC);
}

void AFRecord::SetPrivate()
{
    BitValue<int8_t>::SetBitValue(feature, RF_PRIVATE);
}

bool AFRecord::IsPrivate() const
{
    return BitValue<int8_t>::HaveBitValue(feature, RF_PRIVATE);
}

void AFRecord::SetRealTime()
{
    BitValue<int8_t>::SetBitValue(feature, RF_REAL_TIME);
}

bool AFRecord::IsRealTime() const
{
    return BitValue<int8_t>::HaveBitValue(feature, RF_REAL_TIME);
}

void AFRecord::SetSave()
{
    BitValue<int8_t>::SetBitValue(feature, RF_SAVE);
}

bool AFRecord::IsSave() const
{
    return BitValue<int8_t>::HaveBitValue(feature, RF_SAVE);
}

bool AFRecord::SetValue(size_t row, size_t col, const AFIData& value)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return false;
    }

    RowData* row_data = mxRowDatas[row];

    row_data[col].Assign(value);
    return true;
}

bool AFRecord::SetBool(size_t row, size_t col, const bool value)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return false;
    }

    RowData* row_data = mxRowDatas[row];

    row_data[col].SetBool(value);
    return true;
}

bool AFRecord::SetInt(size_t row, size_t col, const int value)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return false;
    }

    RowData* row_data = mxRowDatas[row];

    row_data[col].SetInt(value);
    return true;
}

bool AFRecord::SetInt64(size_t row, size_t col, const int64_t value)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return false;
    }

    RowData* row_data = mxRowDatas[row];

    row_data[col].SetInt64(value);
    return true;
}

bool AFRecord::SetFloat(size_t row, size_t col, const float value)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return false;
    }

    RowData* row_data = mxRowDatas[row];

    row_data[col].SetFloat(value);
    return true;
}

bool AFRecord::SetDouble(size_t row, size_t col, const double value)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return false;
    }

    RowData* row_data = mxRowDatas[row];

    row_data[col].SetDouble(value);
    return true;
}

bool AFRecord::SetString(size_t row, size_t col, const char* value)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return false;
    }

    RowData* row_data = mxRowDatas[row];

    row_data[col].SetString(value);
    return true;
}

bool AFRecord::SetObject(size_t row, size_t col, const AFGUID& value)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return false;
    }

    RowData* row_data = mxRowDatas[row];

    row_data[col].SetObject(value);
    return true;
}

bool AFRecord::GetValue(size_t row, size_t col, AFIData& value)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return false;
    }

    RowData* row_data = mxRowDatas[row];

    value.Assign(row_data[col]);
    return true;
}

bool AFRecord::GetBool(size_t row, size_t col)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return NULL_BOOLEAN;
    }

    RowData* row_data = mxRowDatas[row];
    return row_data[col].GetBool();
}

int AFRecord::GetInt(size_t row, size_t col)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return NULL_INT;
    }

    RowData* row_data = mxRowDatas[row];
    return row_data[col].GetInt();
}

int64_t AFRecord::GetInt64(size_t row, size_t col)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return NULL_INT64;
    }

    RowData* row_data = mxRowDatas[row];
    return row_data[col].GetInt64();
}

float AFRecord::GetFloat(size_t row, size_t col)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return NULL_FLOAT;
    }

    RowData* row_data = mxRowDatas[row];
    return row_data[col].GetFloat();
}

double AFRecord::GetDouble(size_t row, size_t col)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return NULL_DOUBLE;
    }

    RowData* row_data = mxRowDatas[row];
    return row_data[col].GetDouble();
}

const char* AFRecord::GetString(size_t row, size_t col)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return NULL_STR.c_str();
    }

    RowData* row_data = mxRowDatas[row];
    return row_data[col].GetString();
}

const AFGUID& AFRecord::GetObject(size_t row, size_t col)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
    {
        return NULL_GUID;
    }

    RowData* row_data = mxRowDatas[row];
    return row_data[col].GetObject();
}

const char* AFRecord::GetStringValue(size_t row, size_t col)
{
    //AFCData data;
    //if (!GetValue(row, col, data))
    //{
    //    return NULL_STR.c_str();
    //}

    //return data.ToString();

    //TODO:增加AFIData的ToString接口
    return NULL_STR.c_str();
}

bool AFRecord::GetColTypeList(AFIDataList& col_type_list)
{
    int col_count = GetColCount();
    for(int i = 0; i < col_count; ++i)
    {
        AFCData data;
        int col_type = GetColType(i);
        data.SetDefaultValue(col_type);
        col_type_list.Append(data);
    }

    return true;
}

int AFRecord::FindRow(size_t col, const AFIData& key, size_t begin_row /*= 0*/)
{
    if(col >= GetColCount())
    {
        return -1;
    }

    switch(key.GetType())
    {
    case DT_BOOLEAN:
        return FindBool(col, key.GetBool(), begin_row);
        break;
    case DT_INT:
        return FindInt(col, key.GetInt(), begin_row);
        break;
    case DT_INT64:
        return FindInt64(col, key.GetInt64(), begin_row);
        break;
    case DT_FLOAT:
        return FindFloat(col, key.GetFloat(), begin_row);
        break;
    case DT_DOUBLE:
        return FindDouble(col, key.GetDouble(), begin_row);
        break;
    case DT_STRING:
        return FindString(col, key.GetString(), begin_row);
        break;
    case DT_OBJECT:
        return FindObject(col, key.GetObject(), begin_row);
        break;
    default:
        break;
    }

    return -1;
}

int AFRecord::FindBool(size_t col, const bool key, size_t begin_row /*= 0*/)
{
    if(col >= GetColCount())
    {
        return -1;
    }

    size_t row_num = GetRowCount();
    if(begin_row >= row_num)
    {
        return -1;
    }

    for(size_t i = begin_row; i < row_num; ++i)
    {
        RowData* row_data = mxRowDatas[i];
        if(row_data[col].GetBool() == key)
        {
            return i;
        }
    }

    return -1;
}

int AFRecord::FindInt(size_t col, const int key, size_t begin_row /*= 0*/)
{
    if(col >= GetColCount())
    {
        return -1;
    }

    size_t row_num = GetRowCount();
    if(begin_row >= row_num)
    {
        return -1;
    }

    for(size_t i = begin_row; i < row_num; ++i)
    {
        RowData* row_data = mxRowDatas[i];
        if(row_data[col].GetInt() == key)
        {
            return i;
        }
    }

    return -1;
}

int AFRecord::FindInt64(size_t col, const int64_t key, size_t begin_row /*= 0*/)
{
    if(col >= GetColCount())
    {
        return -1;
    }

    size_t row_num = GetRowCount();
    if(begin_row >= row_num)
    {
        return -1;
    }

    for(size_t i = begin_row; i < row_num; ++i)
    {
        RowData* row_data = mxRowDatas[i];
        if(row_data[col].GetInt64() == key)
        {
            return i;
        }
    }

    return -1;
}

int AFRecord::FindFloat(size_t col, const float key, size_t begin_row /*= 0*/)
{
    if(col >= GetColCount())
    {
        return -1;
    }

    size_t row_num = GetRowCount();
    if(begin_row >= row_num)
    {
        return -1;
    }

    for(size_t i = begin_row; i < row_num; ++i)
    {
        RowData* row_data = mxRowDatas[i];
        if(IsFloatEqual(row_data[col].GetFloat(), key))
        {
            return i;
        }
    }

    return -1;
}

int AFRecord::FindDouble(size_t col, const double key, size_t begin_row /*= 0*/)
{
    if(col >= GetColCount())
    {
        return -1;
    }

    size_t row_num = GetRowCount();
    if(begin_row >= row_num)
    {
        return -1;
    }

    for(size_t i = begin_row; i < row_num; ++i)
    {
        RowData* row_data = mxRowDatas[i];
        if(IsDoubleEqual(row_data[col].GetDouble(), key))
        {
            return i;
        }
    }

    return -1;
}

int AFRecord::FindString(size_t col, const char* key, size_t begin_row /*= 0*/)
{
    if(col >= GetColCount())
    {
        return -1;
    }

    size_t row_num = GetRowCount();
    if(begin_row >= row_num)
    {
        return -1;
    }

    for(size_t i = begin_row; i < row_num; ++i)
    {
        RowData* row_data = mxRowDatas[i];
        if(ARK_STRICMP(row_data[col].GetString(), key) == 0)
        {
            return i;
        }
    }

    return -1;
}

int AFRecord::FindObject(size_t col, const AFGUID& key, size_t begin_row /*= 0*/)
{
    if(col >= GetColCount())
    {
        return -1;
    }

    size_t row_num = GetRowCount();
    if(begin_row >= row_num)
    {
        return -1;
    }

    for(size_t i = begin_row; i < row_num; ++i)
    {
        RowData* row_data = mxRowDatas[i];
        if(row_data[col].GetObject() == key)
        {
            return i;
        }
    }

    return -1;
}

bool AFRecord::QueryRow(const int nRow, AFIDataList& varList)
{
    //TODO
    return false;
}