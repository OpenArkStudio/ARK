/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include "AFCDataList.hpp"
#include "AFArrayPod.hpp"
#include "AFCoreDef.hpp"
#include "AFString.hpp"
#include "AFCData.hpp"
#include "AFDefine.hpp"

namespace ark
{

    class AFDataTable
    {
    private:
        using RowData = AFCData;

    public:
        enum DATA_TABLE_FEATURE
        {
            TABLE_PUBLIC = 0, //send to others
            TABLE_PRIVATE = 1, //send to self
            TABLE_REAL_TIME = 2, //send real-time when changed
            TABLE_SAVE = 3, //if need save to database
        };

        enum DATA_TABLE_OP_TYPE
        {
            TABLE_ADD = 0,  //add whole row data
            TABLE_DELETE,       //delete whole row data
            TABLE_UPDATE,       //update row & col cell data
            TABLE_COVERAGE,     //coverage whole row data
            TABLE_SWAP,         //swap two whole row data
        };

        AFDataTable() noexcept
            : mstrName(NULL_STR.c_str())
            , feature(0)
        {
        }

        virtual ~AFDataTable()
        {
            ReleaseAll();
        }

        void SetName(const char* value)
        {
            ARK_ASSERT_RET_NONE(value != nullptr);
            mstrName = value;
        }

        const char* GetName() const
        {
            return mstrName.c_str();
        }

        size_t GetRowCount() const
        {
            return mxRowDatas.size();
        }

        void SetColCount(size_t value)
        {
            assert(value > 0);

            if (mxColTypes.size() > 0)
            {
                ReleaseAll();
            }

            mxColTypes.resize(value);
        }

        size_t GetColCount() const
        {
            return mxColTypes.size();
        }

        bool SetColType(size_t index, int type)
        {
            ARK_ASSERT_RET_VAL(index < mxColTypes.size(), false);
            ARK_ASSERT_RET_VAL(type < DT_UNKNOWN, false);

            mxColTypes[index] = type;
            return true;
        }

        int GetColType(size_t col) const
        {
            ARK_ASSERT_RET_VAL(col < mxColTypes.size(), NULL_INT);
            return mxColTypes[col];
        }

        int AddRow()
        {
            //default insert row
            size_t col_num = GetColCount();
            RowData* row_data = new RowData[col_num];

            size_t nIndex = FindEmptyRow();
            mxRowDatas[nIndex] = row_data;

            DATA_TABLE_EVENT_DATA xTableEventData;
            xTableEventData.nOpType = AFDataTable::TABLE_ADD;
            xTableEventData.nRow = nIndex;
            xTableEventData.nCol = 0;
            xTableEventData.strName = mstrName;

            OnEventHandler(xTableEventData, row_data[nIndex], AFCData(DT_BOOLEAN, false));

            return nIndex;
        }

        bool AddRow(size_t row)
        {
            size_t col_num = GetColCount();
            RowData* row_data = new RowData[col_num];

            while (GetRowCount() <= row)
            {
                mxRowDatas.push_back(nullptr);
            }

            RowData* old_row_data = mxRowDatas[row];
            if (nullptr != old_row_data)
            {
                ARK_DELETE_ARRAY(RowData, row_data);
                return false;
            }

            mxRowDatas[row] = row_data;

            DATA_TABLE_EVENT_DATA xTableEventData;
            xTableEventData.nOpType = AFDataTable::TABLE_ADD;
            xTableEventData.nRow = row;
            xTableEventData.nCol = 0;
            xTableEventData.strName = mstrName;

            OnEventHandler(xTableEventData, row_data[row], AFCData(DT_BOOLEAN, false));

            return true;
        }

        bool AddRow(size_t row, const AFIDataList& data)
        {
            size_t col_num = GetColCount();
            if (data.GetCount() != col_num)
            {
                ARK_ASSERT(0, "data size is not equal with col_num, please check your arg.", __FILE__, __FUNCTION__);
                return false;
            }

            RowData* row_data = new RowData[col_num];
            for (size_t i = 0; i < data.GetCount(); ++i)
            {
                int type = GetColType(i);

                switch (type)
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
                default:
                    {
                        ReleaseRow(row_data, col_num);
                        return false;
                    }
                    break;
                }
            }

            while (GetRowCount() <= row)
            {
                mxRowDatas.push_back(nullptr);
            }

            RowData* old_row_data = mxRowDatas[row];
            if (nullptr != old_row_data)
            {
                ARK_DELETE_ARRAY(RowData, row_data);
                return false;
            }

            mxRowDatas[row] = row_data;

            DATA_TABLE_EVENT_DATA xTableEventData;
            xTableEventData.nOpType = AFDataTable::TABLE_ADD;
            xTableEventData.nRow = row;
            xTableEventData.nCol = 0;
            xTableEventData.strName = mstrName;

            OnEventHandler(xTableEventData, row_data[row], AFCData(DT_BOOLEAN, false));
            return true;
        }

        int AddRow(const AFIDataList& data)
        {
            size_t nIndex = FindEmptyRow();
            if (!AddRow(nIndex, data))
            {
                return -1;
            }

            return nIndex;
        }

        size_t FindEmptyRow()
        {
            for (size_t i = 0; i < GetRowCount(); ++i)
            {
                RowData* row_data = mxRowDatas[i];
                if (row_data == nullptr)
                {
                    return i;
                }
            }

            size_t nPos = GetRowCount();
            mxRowDatas.push_back(nullptr);
            return nPos;
        }

        bool DeleteRow(size_t row)
        {
            assert(row < mxRowDatas.size());

            DATA_TABLE_EVENT_DATA xTableEventData;
            xTableEventData.nOpType = AFDataTable::TABLE_ADD;
            xTableEventData.nRow = row;
            xTableEventData.nCol = 0;
            xTableEventData.strName = mstrName;

            OnEventHandler(xTableEventData, AFCData(DT_BOOLEAN, false), AFCData(DT_BOOLEAN, false));

            ReleaseRow(mxRowDatas[row], mxColTypes.size());
            mxRowDatas[row] = nullptr;

            return true;
        }

        void Clear()
        {
            ReleaseAll();
        }

        void SetFeature(const AFFeatureType& new_feature)
        {
            this->feature = new_feature;
        }

        const AFFeatureType& GetFeature() const
        {
            return feature;
        }

        void SetPublic()
        {
            feature[TABLE_PUBLIC] = 1;
        }

        bool IsPublic() const
        {
            return feature.test(TABLE_PUBLIC);
        }

        void SetPrivate()
        {
            feature[TABLE_PRIVATE] = 1;
        }

        bool IsPrivate() const
        {
            return feature.test(TABLE_PRIVATE);
        }

        void SetRealTime()
        {
            feature[TABLE_REAL_TIME] = 1;
        }

        bool IsRealTime() const
        {
            return feature.test(TABLE_REAL_TIME);
        }

        void SetSave()
        {
            feature[TABLE_SAVE] = 1;
        }

        bool IsSave() const
        {
            return feature.test(TABLE_SAVE);
        }

        bool IsUsed(size_t row)
        {
            if (row >= GetRowCount())
            {
                return false;
            }

            RowData* row_data = mxRowDatas[row];
            if (nullptr == row_data)
            {
                return false;
            }

            return true;
        }

        bool SetValue(size_t row, size_t col, const AFIData& value)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return false;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, false);

            if (!row_data[col].equal(value))
            {
                DATA_TABLE_EVENT_DATA xTableEventData;
                xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
                xTableEventData.nRow = row;
                xTableEventData.nCol = col;
                xTableEventData.strName = mstrName;

                OnEventHandler(xTableEventData, row_data[col], value);
            }

            row_data[col].Assign(value);
            return true;
        }

        bool SetBool(size_t row, size_t col, const bool value)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return false;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, false);

            if (!row_data[col].equal(value))
            {
                DATA_TABLE_EVENT_DATA xTableEventData;
                xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
                xTableEventData.nRow = row;
                xTableEventData.nCol = col;
                xTableEventData.strName = mstrName;

                OnEventHandler(xTableEventData, row_data[col], AFCData(DT_BOOLEAN, value));
            }

            row_data[col].SetBool(value);

            return true;
        }

        bool SetInt(size_t row, size_t col, const int value)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return false;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, false);

            if (!row_data[col].equal(value))
            {
                DATA_TABLE_EVENT_DATA xTableEventData;
                xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
                xTableEventData.nRow = row;
                xTableEventData.nCol = col;
                xTableEventData.strName = mstrName;

                OnEventHandler(xTableEventData, row_data[col], AFCData(DT_INT, value));
            }

            row_data[col].SetInt(value);
            return true;
        }

        bool SetInt64(size_t row, size_t col, const int64_t value)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return false;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, false);
            if (!row_data[col].equal(value))
            {
                DATA_TABLE_EVENT_DATA xTableEventData;
                xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
                xTableEventData.nRow = row;
                xTableEventData.nCol = col;
                xTableEventData.strName = mstrName;

                OnEventHandler(xTableEventData, row_data[col], AFCData(DT_INT64, value));
            }

            row_data[col].SetInt64(value);
            return true;
        }

        bool SetFloat(size_t row, size_t col, const float value)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return false;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, false);
            if (!row_data[col].equal(value))
            {
                DATA_TABLE_EVENT_DATA xTableEventData;
                xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
                xTableEventData.nRow = row;
                xTableEventData.nCol = col;
                xTableEventData.strName = mstrName;

                OnEventHandler(xTableEventData, row_data[col], AFCData(DT_FLOAT, value));
            }
            row_data[col].SetFloat(value);
            return true;
        }

        bool SetDouble(size_t row, size_t col, const double value)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return false;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, false);
            if (!row_data[col].equal(value))
            {
                DATA_TABLE_EVENT_DATA xTableEventData;
                xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
                xTableEventData.nRow = row;
                xTableEventData.nCol = col;
                xTableEventData.strName = mstrName;

                OnEventHandler(xTableEventData, row_data[col], AFCData(DT_DOUBLE, value));
            }

            row_data[col].SetDouble(value);
            return true;
        }

        bool SetString(size_t row, size_t col, const char* value)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return false;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, false);
            if (!row_data[col].equal(value))
            {
                DATA_TABLE_EVENT_DATA xTableEventData;
                xTableEventData.nOpType = AFDataTable::TABLE_UPDATE;
                xTableEventData.nRow = row;
                xTableEventData.nCol = col;
                xTableEventData.strName = mstrName;

                OnEventHandler(xTableEventData, row_data[col], AFCData(DT_STRING, value));
            }
            row_data[col].SetString(value);
            return true;
        }

        bool GetValue(size_t row, size_t col, AFIData& value)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return false;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, false);

            value.Assign(row_data[col]);
            return true;
        }

        bool GetBool(size_t row, size_t col)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return NULL_BOOLEAN;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, false);
            return row_data[col].GetBool();
        }

        int GetInt(size_t row, size_t col)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return NULL_INT;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, 0);
            return row_data[col].GetInt();
        }

        int64_t GetInt64(size_t row, size_t col)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return NULL_INT64;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, 0);
            return row_data[col].GetInt64();
        }

        float GetFloat(size_t row, size_t col)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return NULL_FLOAT;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, 0);
            return row_data[col].GetFloat();
        }

        double GetDouble(size_t row, size_t col)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return NULL_DOUBLE;
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, 0);
            return row_data[col].GetDouble();
        }

        const char* GetString(size_t row, size_t col)
        {
            if ((row >= GetRowCount()) || (col >= GetColCount()))
            {
                return NULL_STR.c_str();
            }

            RowData* row_data = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != row_data, NULL_STR.c_str());
            return row_data[col].GetString();
        }

        bool GetColTypeList(AFIDataList& col_type_list)
        {
            int col_count = GetColCount();
            for (int i = 0; i < col_count; ++i)
            {
                AFCData data;
                int col_type = GetColType(i);
                data.SetDefaultValue(col_type);
                col_type_list.Append(data);
            }

            return true;
        }

        int FindRow(size_t col, const AFIData& key, size_t begin_row = 0)
        {
            if (col >= GetColCount())
            {
                return -1;
            }

            switch (key.GetType())
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
            default:
                ARK_ASSERT_NO_EFFECT(false);
                break;
            }

            return -1;
        }

        int FindBool(size_t col, const bool key, size_t begin_row = 0)
        {
            if (col >= GetColCount())
            {
                return -1;
            }

            size_t row_num = GetRowCount();
            if (begin_row >= row_num)
            {
                return -1;
            }

            for (size_t i = begin_row; i < row_num; ++i)
            {
                RowData* row_data = mxRowDatas[i];
                ARK_ASSERT_CONTINUE(nullptr != row_data);
                if (row_data[col].GetBool() == key)
                {
                    return i;
                }
            }

            return -1;
        }

        int FindInt(size_t col, const int key, size_t begin_row = 0)
        {
            if (col >= GetColCount())
            {
                return -1;
            }

            size_t row_num = GetRowCount();

            if (begin_row >= row_num)
            {
                return -1;
            }

            for (size_t i = begin_row; i < row_num; ++i)
            {
                RowData* row_data = mxRowDatas[i];
                ARK_ASSERT_CONTINUE(nullptr != row_data);

                if (row_data[col].GetInt() == key)
                {
                    return i;
                }
            }

            return -1;
        }

        int FindInt64(size_t col, const int64_t key, size_t begin_row = 0)
        {
            if (col >= GetColCount())
            {
                return -1;
            }

            size_t row_num = GetRowCount();
            if (begin_row >= row_num)
            {
                return -1;
            }

            for (size_t i = begin_row; i < row_num; ++i)
            {
                RowData* row_data = mxRowDatas[i];
                ARK_ASSERT_CONTINUE(nullptr != row_data);

                if (row_data[col].GetInt64() == key)
                {
                    return i;
                }
            }

            return -1;
        }

        int FindFloat(size_t col, const float key, size_t begin_row = 0)
        {
            if (col >= GetColCount())
            {
                return -1;
            }

            size_t row_num = GetRowCount();
            if (begin_row >= row_num)
            {
                return -1;
            }

            for (size_t i = begin_row; i < row_num; ++i)
            {
                RowData* row_data = mxRowDatas[i];
                ARK_ASSERT_CONTINUE(nullptr != row_data);

                if (AFMisc::IsFloatEqual(row_data[col].GetFloat(), key))
                {
                    return i;
                }
            }

            return -1;
        }

        int FindDouble(size_t col, const double key, size_t begin_row = 0)
        {
            if (col >= GetColCount())
            {
                return -1;
            }

            size_t row_num = GetRowCount();
            if (begin_row >= row_num)
            {
                return -1;
            }

            for (size_t i = begin_row; i < row_num; ++i)
            {
                RowData* row_data = mxRowDatas[i];
                ARK_ASSERT_CONTINUE(nullptr != row_data);

                if (AFMisc::IsDoubleEqual(row_data[col].GetDouble(), key))
                {
                    return i;
                }
            }

            return -1;
        }

        int FindString(size_t col, const char* key, size_t begin_row = 0)
        {
            if (col >= GetColCount())
            {
                return -1;
            }

            size_t row_num = GetRowCount();
            if (begin_row >= row_num)
            {
                return -1;
            }

            for (size_t i = begin_row; i < row_num; ++i)
            {
                RowData* row_data = mxRowDatas[i];
                ARK_ASSERT_CONTINUE(nullptr != row_data);

                if (ARK_STRICMP(row_data[col].GetString(), key) == 0)
                {
                    return i;
                }
            }

            return -1;
        }

        bool QueryRow(const size_t row, AFIDataList& varList)
        {
            ARK_ASSERT_RET_VAL_NO_EFFECT(row < mxRowDatas.size(), false);

            RowData* rowData = mxRowDatas[row];
            ARK_ASSERT_RET_VAL_NO_EFFECT(nullptr != rowData, false);

            for (size_t i = 0; i < mxColTypes.size(); ++i)
            {
                RowData& subData = rowData[i];

                switch (subData.GetType())
                {
                case DT_BOOLEAN:
                    varList.AddBool(subData.GetBool());
                    break;
                case DT_INT:
                    varList.AddInt(subData.GetInt());
                    break;
                case DT_INT64:
                    varList.AddInt64(subData.GetInt64());
                    break;
                case DT_FLOAT:
                    varList.AddFloat(subData.GetFloat());
                    break;
                case DT_DOUBLE:
                    varList.AddDouble(subData.GetDouble());
                    break;
                case DT_STRING:
                    varList.AddString(subData.GetString());
                    break;
                default:
                    ARK_ASSERT_NO_EFFECT(false);
                    break;
                }
            }

            return true;
        }

        bool RegisterCallback(const LITLE_DATA_TABLE_EVENT_FUNCTOR_PTR& cb)
        {
            mxTablecallbacks = cb;
            return true;
        }

    protected:
        void ReleaseRow(RowData* row_data, size_t col_num)
        {
            for (size_t i = 0; i < col_num; ++i)
            {
                row_data[i].Release();
            }
        }

        void ReleaseAll()
        {
            size_t col_num = mxColTypes.size();

            for (size_t i = 0; i < mxRowDatas.size(); ++i)
            {
                if (mxRowDatas[i] != nullptr)
                {
                    ReleaseRow(mxRowDatas[i], col_num);
                }
            }

            mxRowDatas.clear();
        }

        void OnEventHandler(const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldData, const AFIData& newData)
        {
            if (nullptr != mxTablecallbacks)
            {
                (*mxTablecallbacks)(xEventData, oldData, newData);
            }
        }

    private:
        DataTableName mstrName;                         //DataTable name
        AFFeatureType feature;                          //DataTable feature
        ArrayPod<int, 1, CoreAlloc> mxColTypes;        //DataTable column type array
        ArrayPod<RowData*, 1, CoreAlloc> mxRowDatas;   //DataTable data array
        LITLE_DATA_TABLE_EVENT_FUNCTOR_PTR mxTablecallbacks = nullptr;// callback
    };

}