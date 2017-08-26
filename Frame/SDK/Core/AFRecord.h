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
// * @file  	AFRecord.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFRecord                                                  *
*****************************************************************************/
#pragma once

#include "SDK/Base/AFCDataList.h"
#include "SDK/Base/AFArrayPod.hpp"
#include "SDK/Base/AFCoreDef.hpp"
#include "SDK/Base/AFString.hpp"
#include "SDK/Base/AFIData.h"

using namespace ArkFrame;

class AFRecord
{
private:
    using row_data_t = AFIData;

public:
    AFRecord();
    virtual ~AFRecord();

    const char* GetName() const;

    virtual int GetCols() const;

    virtual int GetRows() const;

    virtual int GetRowMax() const;

    virtual int GetColType(int col) const;

    virtual bool AddRow(const int row);
    virtual bool AddRow(const int row, const AFIDataList& data);

    virtual bool SetInt(const int nRow, const int nCol, const int64_t value);
    virtual bool SetDouble(const int nRow, const int nCol, const double value);
    virtual bool SetString(const int nRow, const int nCol, const std::string& value);
    virtual bool SetObject(const int nRow, const int nCol, const AFGUID& value);

    virtual int64_t GetInt(const int nRow, const int nCol) const;
    virtual double GetDouble(const int nRow, const int nCol) const;
    virtual const std::string& GetString(const int nRow, const int nCol) const;
    virtual const AFGUID& GetObject(const int nRow, const int nCol) const;

    virtual int FindInt(const int nCol, const int64_t value, AFIDataList& varResult);
    virtual int FindDouble(const int nCol, const double value, AFIDataList& varResult);
    virtual int FindString(const int nCol, const std::string& value, AFIDataList& varResult);
    virtual int FindObject(const int nCol, const AFGUID& value, AFIDataList& varResult);

    virtual bool QueryRow(const int nRow, AFIDataList& varList);

protected:

    RecordName mstrName;
    ArraryPod<int, 1, CoreAlloc> mxColTypes;
    ArraryPod<row_data_t*, 1, CoreAlloc> mxRowDatas;

    bool mbSave;
    bool mbPublic;
    bool mbPrivate;
    bool mbRealtime;

    //std::vector<RECORD_EVENT_FUNCTOR_PTR> mtRecordCallback;
};

