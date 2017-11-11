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

#pragma once

#include "SDK/Base/AFCoreDef.hpp"
#include "SDK/Base/AFArrayPod.hpp"
#include "SDK/Base/AFStringPod.hpp"
#include "AFIRecordMgr.h"

class AFCRecordMgr : public AFIRecordMgr
{
protected:
    using RecordCallbacks = std::vector<RECORD_EVENT_FUNCTOR_PTR>;

public:
    AFCRecordMgr(const AFGUID& guid);
    virtual ~AFCRecordMgr();

    virtual const AFGUID& Self();

    virtual bool Exist(const char* name) const;
    virtual bool Exist(const char* name, size_t& index) const;

    virtual bool AddRecord(const AFGUID& self_id, const char* record_name, const AFIDataList& col_type_list, const int8_t feature);
    virtual bool AddRecordCallback(const char* record_name, const RECORD_EVENT_FUNCTOR_PTR& cb);

    virtual void Clear();
    virtual AFRecord* GetRecord(const char* name);
    virtual size_t GetCount() const;
    virtual AFRecord* GetRecordByIndex(size_t index) const;

    virtual bool SetRecordBool(const char* name, const int row, const int col, const bool value);
    virtual bool SetRecordInt(const char* name, const int row, const int col, const int32_t value);
    virtual bool SetRecordInt64(const char* name, const int row, const int col, const int64_t value);
    virtual bool SetRecordFloat(const char* name, const int row, const int col, const float value);
    virtual bool SetRecordDouble(const char* name, const int row, const int col, const double value);
    virtual bool SetRecordString(const char* name, const int row, const int col, const char* value);
    virtual bool SetRecordObject(const char* name, const int row, const int col, const AFGUID& value);

    virtual bool GetRecordBool(const char* name, const int row, const int col);
    virtual int32_t GetRecordInt(const char* name, const int row, const int col);
    virtual int64_t GetRecordInt64(const char* name, const int row, const int col);
    virtual float GetRecordFloat(const char* name, const int row, const int col);
    virtual double GetRecordDouble(const char* name, const int row, const int col);
    virtual const char* GetRecordString(const char* name, const int row, const int col);
    virtual const AFGUID& GetRecordObject(const char* name, const int row, const int col);

protected:
    bool GetRecordData(const char* name, const int row, const int col, AFIData& value);

    void OnEventHandler(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFCData& oldData, const AFCData& newData);

    bool AddRecordInternal(AFRecord* record);
    void ReleaseAll();

private:
    AFGUID self;

    ArraryPod<AFRecord*, 1, CoreAlloc> mxRecords;
    StringPod<char, size_t, StringTraits<char>, CoreAlloc> mxIndices;

    //AFArrayMap<std::string, AFRecord> mxRecords;

    RecordCallbacks mxRecordCallbacks;
};