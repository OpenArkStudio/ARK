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

AFCRecordMgr::AFCRecordMgr()
{

}

AFCRecordMgr::~AFCRecordMgr()
{
    ReleaseAll();
}

void AFCRecordMgr::ReleaseAll()
{
    for (size_t i = 0; i < GetCount(); ++i)
    {
        delete mxRecords[i];
        mxRecords[i] = NULL;
    }

    mxRecords.clear();
}

bool AFCRecordMgr::Exist(const char* name) const
{
    return mxIndices.exists(name);
}

bool AFCRecordMgr::Exist(const char* name, size_t& index) const
{
    return mxIndices.GetData(name, index);
}

bool AFCRecordMgr::Add(AFRecord* record)
{
    assert(record != NULL);

    return mxIndices.Add(record->GetName(), mxRecords.size());
}

void AFCRecordMgr::Clear()
{
    ReleaseAll();
    mxIndices.Clear();
}

AFRecord* AFCRecordMgr::Get(const char* name)
{
    size_t index;
    if (!mxIndices.GetData(name, index))
    {
        return NULL;
    }

    return mxRecords[index];
}

size_t AFCRecordMgr::GetCount() const
{
    return mxRecords.size();
}

AFRecord* AFCRecordMgr::GetRecord(size_t index) const
{
    assert(index < GetCount());
    return mxRecords[index];
}

bool AFCRecordMgr::SetRecordBool(const char* name, const int row, const int col, const bool value)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return false;
    }

    return record->SetBool(row, col, value);
}

bool AFCRecordMgr::SetRecordInt(const char* name, const int row, const int col, const int32_t value)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return false;
    }

    return record->SetInt(row, col, value);
}

bool AFCRecordMgr::SetRecordInt64(const char* name, const int row, const int col, const int64_t value)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return false;
    }

    return record->SetInt64(row, col, value);
}

bool AFCRecordMgr::SetRecordFloat(const char* name, const int row, const int col, const float value)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return false;
    }

    return record->SetFloat(row, col, value);
}

bool AFCRecordMgr::SetRecordDouble(const char* name, const int row, const int col, const double value)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return false;
    }

    return record->SetDouble(row, col, value);
}

bool AFCRecordMgr::SetRecordString(const char* name, const int row, const int col, const char* value)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return false;
    }

    return record->SetString(row, col, value);
}

bool AFCRecordMgr::SetRecordObject(const char* name, const int row, const int col, const AFGUID& value)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return false;
    }

    return record->SetObject(row, col, value);
}

bool AFCRecordMgr::GetRecordBool(const char* name, const int row, const int col)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return false;
    }

    return record->GetBool(row, col);
}

int32_t AFCRecordMgr::GetRecordInt(const char* name, const int row, const int col)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return NULL_INT;
    }

    return record->GetInt(row, col);
}

int64_t AFCRecordMgr::GetRecordInt64(const char* name, const int row, const int col)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return NULL_INT64;
    }

    return record->GetInt64(row, col);
}

float AFCRecordMgr::GetRecordFloat(const char* name, const int row, const int col)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return NULL_FLOAT;
    }

    return record->GetFloat(row, col);
}

double AFCRecordMgr::GetRecordDouble(const char* name, const int row, const int col)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return NULL_DOUBLE;
    }

    return record->GetDouble(row, col);
}

const char* AFCRecordMgr::GetRecordString(const char* name, const int row, const int col)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return NULL_STR.c_str();
    }

    return record->GetString(row, col);
}

const AFGUID& AFCRecordMgr::GetRecordObject(const char* name, const int row, const int col)
{
    AFRecord* record = Get(name);
    if (NULL == record)
    {
        return NULL_GUID;
    }

    return record->GetObject(row, col);
}