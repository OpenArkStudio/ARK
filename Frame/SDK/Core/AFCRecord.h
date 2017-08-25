/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @FileName  AFCRecord.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFCRecord                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFCRecord                                                  *
*****************************************************************************/
#pragma once

#include <vector>
#include "AFIRecord.h"
#include "SDK/Base/AFCDataList.h"
#include "SDK/Base/AFMapEx.h"

class AFCRecord : public AFIRecord
{
public:
    AFCRecord();
    AFCRecord(const AFGUID& self, const std::string& strRecordName, const ARK_SHARE_PTR<AFIDataList>& valueList, const ARK_SHARE_PTR<AFIDataList>& tagList, const int nMaxRow);
    virtual ~AFCRecord();

    virtual bool IsUsed(const int nRow) const;
    virtual bool SetUsed(const int nRow, const int bUse);
    virtual int GetCols() const;
    virtual int GetRows() const;

    virtual int GetColType(const int nCol) const;
    virtual const std::string& GetColTag(const int nCol) const;

    virtual int AddRow(const int nRow);
    virtual int AddRow(const int nRow, const AFIDataList& var);

    virtual bool SetBool(const int nRow, const int nCol, const bool value);
    virtual bool SetInt(const int nRow, const int nCol, const int32_t value);
    virtual bool SetInt64(const int nRow, const int nCol, const int64_t value);
    virtual bool SetFloat(const int nRow, const int nCol, const float value);
    virtual bool SetDouble(const int nRow, const int nCol, const double value);
    virtual bool SetString(const int nRow, const int nCol, const std::string& value);
    virtual bool SetObject(const int nRow, const int nCol, const AFGUID& value);

    virtual bool SetBool(const int nRow, const std::string& strColTag, const bool value);
    virtual bool SetInt(const int nRow, const std::string& strColTag, const int32_t value);
    virtual bool SetInt64(const int nRow, const std::string& strColTag, const int64_t value);
    virtual bool SetFloat(const int nRow, const std::string& strColTag, const float value);
    virtual bool SetDouble(const int nRow, const std::string& strColTag, const double value);
    virtual bool SetString(const int nRow, const std::string& strColTag, const std::string& value);
    virtual bool SetObject(const int nRow, const std::string& strColTag, const AFGUID& value);

    virtual bool GetBool(const int nRow, const int nCol) const;
    virtual int32_t GetInt(const int nRow, const int nCol) const;
    virtual int64_t GetInt64(const int nRow, const int nCol) const;
    virtual float GetFloat(const int nRow, const int nCol) const;
    virtual double GetDouble(const int nRow, const int nCol) const;
    virtual const std::string& GetString(const int nRow, const int nCol) const;
    virtual const AFGUID& GetObject(const int nRow, const int nCol) const;

    virtual bool GetBool(const int nRow, const std::string& strColTag) const;
    virtual int32_t GetInt(const int nRow, const std::string& strColTag) const;
    virtual int64_t GetInt64(const int nRow, const std::string& strColTag) const;
    virtual float GetFloat(const int nRow, const std::string& strColTag) const;
    virtual double GetDouble(const int nRow, const std::string& strColTag) const;
    virtual const std::string& GetString(const int nRow, const std::string& strColTag) const;
    virtual const AFGUID& GetObject(const int nRow, const std::string& strColTag) const;

    virtual int FindRowByColValue(const int nCol, const AFIDataList& var, AFIDataList& varResult);

    virtual int FindBool(const int nCol, const bool value, AFIDataList& varResult);
    virtual int FindInt(const int nCol, const int32_t value, AFIDataList& varResult);
    virtual int FindInt64(const int nCol, const int64_t value, AFIDataList& varResult);
    virtual int FindFloat(const int nCol, const float value, AFIDataList& varResult);
    virtual int FindDouble(const int nCol, const double value, AFIDataList& varResult);
    virtual int FindString(const int nCol, const std::string& value, AFIDataList& varResult);
    virtual int FindObject(const int nCol, const AFGUID& value, AFIDataList& varResult);

    virtual int FindRowByColValue(const std::string& strColTag, const AFIDataList& var, AFIDataList& varResult);

    virtual int FindBool(const std::string& strColTag, const bool value, AFIDataList& varResult);
    virtual int FindInt(const std::string& strColTag, const int32_t value, AFIDataList& varResult);
    virtual int FindInt64(const std::string& strColTag, const int64_t value, AFIDataList& varResult);
    virtual int FindFloat(const std::string& strColTag, const float value, AFIDataList& varResult);
    virtual int FindDouble(const std::string& strColTag, const double value, AFIDataList& varResult);
    virtual int FindString(const std::string& strColTag, const std::string& value, AFIDataList& varResult);
    virtual int FindObject(const std::string& strColTag, const AFGUID& value, AFIDataList& varResult);

    virtual bool QueryRow(const int nRow, AFIDataList& varList);
    virtual bool SwapRowInfo(const int nOriginRow, const int nTargetRow);
    virtual bool Remove(const int nRow);
    virtual bool Clear();
    virtual void AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb);

    virtual const bool GetSave();
    virtual const bool GetCache();
    virtual const bool GetPublic();
    virtual const bool GetPrivate();
    virtual const std::string& GetName() const;

    virtual void SetSave(const bool bSave);
    virtual void SetCache(const bool bCache);
    virtual void SetPublic(const bool bPublic);
    virtual void SetPrivate(const bool bPrivate);
    virtual void SetName(const std::string& strName);

    virtual const ARK_SHARE_PTR<AFIDataList> GetInitData() const;
    virtual const ARK_SHARE_PTR<AFIDataList> GetTag() const;

    virtual const TRECORDVEC& GetRecordVec() const;

protected:

    int GetPos(int nRow, int nCol) const;
    int GetCol(const std::string& strTag) const;
    bool ValidPos(int nRow, int nCol) const;
    bool ValidRow(int nRow) const;
    bool ValidCol(int nCol) const;
    bool ValidCheck(AF_DATA_TYPE eType, const AFIData& var, ARK_SHARE_PTR<AFIData>& pVar);
    void OnEventHandler(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar);

protected:
    //记录这个表的Key类型，那样在读取和设置的时候才能保持正确
    /** @brief   初始值类型--应该引用静态的(或者智能指针)，节约大量内存. */
    ARK_SHARE_PTR<AFIDataList> mVarRecordType;
    /** @brief   col的tag值--应该引用静态的(或者智能指针)，节约大量内存. */
    ARK_SHARE_PTR<AFIDataList> mVarRecordTag;

    /** @brief   tag->col转换. */
    std::map<std::string, int> mmTag;

    /** @brief   /////////////////////////. */

    TRECORDVEC mtRecordVec;
    /** @brief   State of the vector used. */
    std::vector<int> mVecUsedState;
    /** @brief   The mn maximum row. */
    int mnMaxRow;

    /** @brief   The self. */
    AFGUID mSelf;
    /** @brief   True to megabytes save. */
    bool mbSave;
    /** @brief   True to megabytes public. */
    bool mbPublic;
    /** @brief   True to megabytes private. */
    bool mbPrivate;
    /** @brief   True to megabytes cache. */
    bool mbCache;
    /** @brief   Name of the mstr record. */
    std::string mstrRecordName;

    typedef std::vector<RECORD_EVENT_FUNCTOR_PTR> TRECORDCALLBACKEX;
    TRECORDCALLBACKEX mtRecordCallback;
};

