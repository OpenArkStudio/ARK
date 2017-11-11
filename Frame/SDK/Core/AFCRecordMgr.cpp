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

#include "AFCRecordMgr.h"
#include "AFRecord.h"

AFCRecordMgr::AFCRecordMgr(const AFGUID& guid)
    : self(guid)
{

}

AFCRecordMgr::~AFCRecordMgr()
{
    ReleaseAll();
}

const AFGUID& AFCRecordMgr::Self()
{
    return self;
}

void AFCRecordMgr::ReleaseAll()
{
    for(size_t i = 0; i < GetCount(); ++i)
    {
        delete mxRecords[i];
        mxRecords[i] = NULL;
    }

    mxRecords.clear();
    mxRecordCallbacks.clear();
}

bool AFCRecordMgr::Exist(const char* name) const
{
    return mxIndices.exists(name);
}

bool AFCRecordMgr::Exist(const char* name, size_t& index) const
{
    return mxIndices.GetData(name, index);
}

bool AFCRecordMgr::GetRecordData(const char* name, const int row, const int col, AFIData& value)
{
    AFRecord* pRecord = GetRecord(name);
    if (NULL == pRecord)
    {
        return NULL;
    }

    return pRecord->GetValue(row, col, value);
}

void AFCRecordMgr::OnEventHandler(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFCData& oldData, const AFCData& newData)
{
    for (auto& iter : mxRecordCallbacks)
    {
        //TODO:check name from xEventData
        (*iter)(self, xEventData, oldData, newData);
    }
}

bool AFCRecordMgr::AddRecordInternal(AFRecord* record)
{
    assert(record != NULL);

    mxIndices.Add(record->GetName(), mxRecords.size());
    mxRecords.push_back(record);

    return true;
}

bool AFCRecordMgr::AddRecord(const AFGUID& self_id, const char* record_name, const AFIDataList& col_type_list, const int8_t feature)
{
    ARK_ASSERT(record_name != NULL && sizeof(record_name) > 0, "record name is invalid", __FILE__, __FUNCTION__);

    self = self_id;

    AFRecord* pRecord = ARK_NEW AFRecord();
    pRecord->SetName(record_name);
    pRecord->SetColCount(col_type_list.GetCount());
    for(int i = 0; i < col_type_list.GetCount(); ++i)
    {
        pRecord->SetColType(i, col_type_list.GetType(i));
    }

    pRecord->SetFeature(feature);

    return AddRecordInternal(pRecord);
}

bool AFCRecordMgr::AddRecordCallback(const char* record_name, const RECORD_EVENT_FUNCTOR_PTR& cb)
{
    //TODO:根据名字区分

    mxRecordCallbacks.push_back(cb);
    return true;
}

void AFCRecordMgr::Clear()
{
    ReleaseAll();
    mxIndices.Clear();
}

AFRecord* AFCRecordMgr::GetRecord(const char* name)
{
    size_t index;
    if(!mxIndices.GetData(name, index))
    {
        return NULL;
    }

    return mxRecords[index];
}

size_t AFCRecordMgr::GetCount() const
{
    return mxRecords.size();
}

AFRecord* AFCRecordMgr::GetRecordByIndex(size_t index) const
{
    assert(index < GetCount());
    return mxRecords[index];
}

bool AFCRecordMgr::SetRecordBool(const char* name, const int row, const int col, const bool value)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return false;
    }

    //callback
    do 
    {
        AFCData oldData;
        if (!GetRecordData(name, row, col, oldData))
        {
            ARK_ASSERT_RET_VAL(0, false);
        }

        if (oldData.GetBool() == value)
        {
            return false;
        }

        if (!mxRecordCallbacks.empty())
        {
            AFCData newData;
            newData.SetBool(value);

            RECORD_EVENT_DATA xRecordEventData;
            xRecordEventData.nOpType = AFRecord::Update;
            xRecordEventData.nRow = row;
            xRecordEventData.nCol = col;
            xRecordEventData.strRecordName = name;

            OnEventHandler(self, xRecordEventData, oldData, newData);
        }
    } while (0);

    return record->SetBool(row, col, value);
}

bool AFCRecordMgr::SetRecordInt(const char* name, const int row, const int col, const int32_t value)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return false;
    }

    return record->SetInt(row, col, value);
}

bool AFCRecordMgr::SetRecordInt64(const char* name, const int row, const int col, const int64_t value)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return false;
    }

    return record->SetInt64(row, col, value);
}

bool AFCRecordMgr::SetRecordFloat(const char* name, const int row, const int col, const float value)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return false;
    }

    return record->SetFloat(row, col, value);
}

bool AFCRecordMgr::SetRecordDouble(const char* name, const int row, const int col, const double value)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return false;
    }

    return record->SetDouble(row, col, value);
}

bool AFCRecordMgr::SetRecordString(const char* name, const int row, const int col, const char* value)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return false;
    }

    return record->SetString(row, col, value);
}

bool AFCRecordMgr::SetRecordObject(const char* name, const int row, const int col, const AFGUID& value)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return false;
    }

    return record->SetObject(row, col, value);
}

bool AFCRecordMgr::GetRecordBool(const char* name, const int row, const int col)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return false;
    }

    return record->GetBool(row, col);
}

int32_t AFCRecordMgr::GetRecordInt(const char* name, const int row, const int col)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return NULL_INT;
    }

    return record->GetInt(row, col);
}

int64_t AFCRecordMgr::GetRecordInt64(const char* name, const int row, const int col)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return NULL_INT64;
    }

    return record->GetInt64(row, col);
}

float AFCRecordMgr::GetRecordFloat(const char* name, const int row, const int col)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return NULL_FLOAT;
    }

    return record->GetFloat(row, col);
}

double AFCRecordMgr::GetRecordDouble(const char* name, const int row, const int col)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return NULL_DOUBLE;
    }

    return record->GetDouble(row, col);
}

const char* AFCRecordMgr::GetRecordString(const char* name, const int row, const int col)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return NULL_STR.c_str();
    }

    return record->GetString(row, col);
}

const AFGUID& AFCRecordMgr::GetRecordObject(const char* name, const int row, const int col)
{
    AFRecord* record = GetRecord(name);
    if(NULL == record)
    {
        return NULL_GUID;
    }

    return record->GetObject(row, col);
}