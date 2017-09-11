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
// * @file  	AFCRecordManager.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFCRecordManager                                                  *
*****************************************************************************/
#pragma once

#include <map>
#include <list>
#include <string>
#include <vector>
#include "AFCRecord.h"
#include "AFIRecordManager.h"

class AFCRecordManager : public AFIRecordManager
{
public:
    AFCRecordManager(const AFGUID& self)
    {
        mSelf = self;
    }

    virtual ~AFCRecordManager();

    virtual ARK_SHARE_PTR<AFIRecord> AddRecord(const AFGUID& self, const std::string& strRecordName, const ARK_SHARE_PTR<AFIDataList>& ValueList, const ARK_SHARE_PTR<AFIDataList>& tagData, const int nRows);

    virtual const AFGUID& Self();

    virtual bool SetRecordBool(const std::string& strRecordName, const int nRow, const int nCol, const bool value);
    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int32_t value);
    virtual bool SetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol, const int64_t value);
    virtual bool SetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const float value);
    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double value);
    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& value);
    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const AFGUID& value);

    virtual bool SetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag, const bool value);
    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int32_t value);
    virtual bool SetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64_t value);
    virtual bool SetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const float value);
    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value);
    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value);
    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const AFGUID& value);

    virtual bool GetRecordBool(const std::string& strRecordName, const int nRow, const int nCol);
    virtual int32_t GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol);
    virtual int64_t GetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol);
    virtual float GetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol);
    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol);
    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const int nCol);
    virtual const AFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol);

    virtual bool GetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual int32_t GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual int64_t GetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual float GetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual const AFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag);

private:
    AFGUID mSelf;
};

