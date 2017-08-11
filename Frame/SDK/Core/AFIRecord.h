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

#include "SDK/Base/AFDefine.h"
#include "SDK/Base/AFCData.h"
#include "SDK/Base/AFCDataList.h"
#include "SDK/Base/AFMacros.hpp"

using namespace ArkFrame;

class AFIRecord
{
public:
    enum RecordOptype
    {
        Add = 0,
        Del,
        Swap,
        Create,
        Update,
        Cleared,
        Sort,
        Cover,
    };

    typedef std::vector<ARK_SHARE_PTR<AFIData>> TRECORDVEC;
    typedef TRECORDVEC::const_iterator TRECORDVECCONSTITER;

    virtual ~AFIRecord() {}

    virtual bool IsUsed(const int nRow) const  = 0;
    virtual bool SetUsed(const int nRow, const int bUse)  = 0;

    virtual int GetCols() const  = 0;
    virtual int GetRows() const  = 0;

    virtual int GetColType(const int nCol) const = 0;
    virtual const std::string& GetColTag(const int nCol) const = 0;

    virtual int AddRow(const int nRow) = 0;
    virtual int AddRow(const int nRow, const AFIDataList& var) = 0;

    virtual bool SetBool(const int nRow, const int nCol, const bool value) = 0;
    virtual bool SetInt(const int nRow, const int nCol, const int32_t value) = 0;
    virtual bool SetInt64(const int nRow, const int nCol, const int64_t value) = 0;
    virtual bool SetFloat(const int nRow, const int nCol, const float value) = 0;
    virtual bool SetDouble(const int nRow, const int nCol, const double value) = 0;
    virtual bool SetString(const int nRow, const int nCol, const std::string& value) = 0;
    virtual bool SetObject(const int nRow, const int nCol, const AFGUID& value) = 0;

    virtual bool SetBool(const int nRow, const std::string& strColTag, const bool value) = 0;
    virtual bool SetInt(const int nRow, const std::string& strColTag, const int32_t value) = 0;
    virtual bool SetInt64(const int nRow, const std::string& strColTag, const int64_t value) = 0;
    virtual bool SetFloat(const int nRow, const std::string& strColTag, const float value) = 0;
    virtual bool SetDouble(const int nRow, const std::string& strColTag, const double value) = 0;
    virtual bool SetString(const int nRow, const std::string& strColTag, const std::string& value) = 0;
    virtual bool SetObject(const int nRow, const std::string& strColTag, const AFGUID& value) = 0;

    virtual bool GetBool(const int nRow, const int nCol) const = 0;
    virtual int32_t GetInt(const int nRow, const int nCol) const = 0;
    virtual int64_t GetInt64(const int nRow, const int nCol) const = 0;
    virtual float GetFloat(const int nRow, const int nCol) const = 0;
    virtual double GetDouble(const int nRow, const int nCol) const = 0;
    virtual const std::string& GetString(const int nRow, const int nCol) const = 0;
    virtual const AFGUID& GetObject(const int nRow, const int nCol) const = 0;

    virtual bool GetBool(const int nRow, const std::string& strColTag) const = 0;
    virtual int32_t GetInt(const int nRow, const std::string& strColTag) const = 0;
    virtual int64_t GetInt64(const int nRow, const std::string& strColTag) const = 0;
    virtual float GetFloat(const int nRow, const std::string& strColTag) const = 0;
    virtual double GetDouble(const int nRow, const std::string& strColTag) const = 0;
    virtual const std::string& GetString(const int nRow, const std::string& strColTag) const = 0;
    virtual const AFGUID& GetObject(const int nRow, const std::string& strColTag) const = 0;

    virtual int FindRowByColValue(const int nCol, const AFIDataList& var, AFIDataList& varResult) = 0;

    virtual int FindBool(const int nCol, const bool value, AFIDataList& varResult) = 0;
    virtual int FindInt(const int nCol, const int32_t value, AFIDataList& varResult) = 0;
    virtual int FindInt64(const int nCol, const int64_t value, AFIDataList& varResult) = 0;
    virtual int FindFloat(const int nCol, const float value, AFIDataList& varResult) = 0;
    virtual int FindDouble(const int nCol, const double value, AFIDataList& varResult) = 0;
    virtual int FindString(const int nCol, const std::string& value, AFIDataList& varResult) = 0;
    virtual int FindObject(const int nCol, const AFGUID& value, AFIDataList& varResult) = 0;

    virtual int FindRowByColValue(const std::string& strColTag, const AFIDataList& var, AFIDataList& varResult) = 0;

    virtual int FindBool(const std::string& strColTag, const bool value, AFIDataList& varResult) = 0;
    virtual int FindInt(const std::string& strColTag, const int32_t value, AFIDataList& varResult) = 0;
    virtual int FindInt64(const std::string& strColTag, const int64_t value, AFIDataList& varResult) = 0;
    virtual int FindFloat(const std::string& strColTag, const float value, AFIDataList& varResult) = 0;
    virtual int FindDouble(const std::string& strColTag, const double value, AFIDataList& varResult) = 0;
    virtual int FindString(const std::string& strColTag, const std::string& value, AFIDataList& varResult) = 0;
    virtual int FindObject(const std::string& strColTag, const AFGUID& value, AFIDataList& varResult) = 0;

    virtual int SortByCol(const int nCol, const bool bOrder, AFIDataList& varResult)
    {
        return 0;
    }

    virtual int SortByTag(const std::string& strColTag, const bool bOrder, AFIDataList& varResult)
    {
        return 0;
    }

    virtual bool QueryRow(const int nRow, AFIDataList& varList) = 0;
    virtual bool SwapRowInfo(const int nOriginRow, const int nTargetRow) = 0;
    virtual bool Remove(const int nRow) = 0;

    virtual bool Remove(AFIDataList& varRows) //need to optimize
    {
        for(int i  = 0; i < varRows.GetCount(); ++i)
        {
            Remove((int)varRows.Int(i));
        }

        return true;
    }

    virtual bool Clear() = 0;

    virtual void AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;

    virtual const bool GetSave() = 0;
    virtual const bool GetPublic() = 0;
    virtual const bool GetPrivate() = 0;
    virtual const bool GetCache() = 0;
    virtual const std::string& GetName() const = 0;

    virtual const ARK_SHARE_PTR<AFIDataList> GetInitData() const = 0;
    virtual const ARK_SHARE_PTR<AFIDataList> GetTag() const = 0;
    
    virtual void SetSave(const bool bSave) = 0;
    virtual void SetCache(const bool bCache) = 0;
    virtual void SetPublic(const bool bPublic) = 0;
    virtual void SetPrivate(const bool bPrivate) = 0;
    virtual void SetName(const std::string& strName) = 0;

    virtual const TRECORDVEC& GetRecordVec() const = 0;
};