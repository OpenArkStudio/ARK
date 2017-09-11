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

#include "SDK/Base/AFPlatform.hpp"
#include "SDK/Base/AFGUID.h"

class AFRecord;

class AFIRecordMgr
{
public:
    virtual ~AFIRecordMgr() {}

    virtual bool Exist(const char* name) const = 0;
    virtual bool Exist(const char* name, size_t& index) const = 0;

    virtual bool Add(AFRecord* record) = 0;
    virtual void Clear() = 0;
    virtual AFRecord* Get(const char* name) = 0;
    virtual size_t GetCount() const = 0;
    virtual AFRecord* GetRecord(size_t index) const = 0;

    virtual bool SetRecordBool(const char* name, const int row, const int col, const bool value) = 0;
    virtual bool SetRecordInt(const char* name, const int row, const int col, const int32_t value) = 0;
    virtual bool SetRecordInt64(const char* name, const int row, const int col, const int64_t value) = 0;
    virtual bool SetRecordFloat(const char* name, const int row, const int col, const float value) = 0;
    virtual bool SetRecordDouble(const char* name, const int row, const int col, const double value) = 0;
    virtual bool SetRecordString(const char* name, const int row, const int col, const char* value) = 0;
    virtual bool SetRecordObject(const char* name, const int row, const int col, const AFGUID& value) = 0;

    virtual bool GetRecordBool(const char* name, const int row, const int col) = 0;
    virtual int32_t GetRecordInt(const char* name, const int row, const int col) = 0;
    virtual int64_t GetRecordInt64(const char* name, const int row, const int col) = 0;
    virtual float GetRecordFloat(const char* name, const int row, const int col) = 0;
    virtual double GetRecordDouble(const char* name, const int row, const int col) = 0;
    virtual const char* GetRecordString(const char* name, const int row, const int col) = 0;
    virtual const AFGUID& GetRecordObject(const char* name, const int row, const int col) = 0;
};