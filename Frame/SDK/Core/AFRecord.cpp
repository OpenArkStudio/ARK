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
// * @file  	AFRecord.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFRecord                                                  *
*****************************************************************************/
#include "AFRecord.h"

AFRecord::AFRecord()
{

}

AFRecord::~AFRecord()
{
    //Clear();
}

const char* AFRecord::GetName() const
{
    return mstrName.c_str();
}

int AFRecord::GetCols() const
{
    return 0;
}

int AFRecord::GetRows() const
{
    return 0;
}

int AFRecord::GetRowMax() const
{
    return 0;
}

int AFRecord::GetColType(int col) const
{
    return 0;
}

bool AFRecord::AddRow(const int row)
{
    return true;
}

bool AFRecord::AddRow(const int row, const AFIDataList& data)
{
    return true;
}

bool AFRecord::SetInt(const int nRow, const int nCol, const int64_t value)
{
    return true;
}

bool AFRecord::SetDouble(const int nRow, const int nCol, const double value)
{
    return true;
}

bool AFRecord::SetString(const int nRow, const int nCol, const std::string& value)
{
    return true;
}

bool AFRecord::SetObject(const int nRow, const int nCol, const AFGUID& value)
{
    return true;
}

int64_t AFRecord::GetInt(const int nRow, const int nCol) const
{
    return NULL_INT;
}

double AFRecord::GetDouble(const int nRow, const int nCol) const
{
    return NULL_DOUBLE;
}

const std::string& AFRecord::GetString(const int nRow, const int nCol) const
{
    return NULL_STR;
}

const AFGUID& AFRecord::GetObject(const int nRow, const int nCol) const
{
    return NULL_GUID;
}

int AFRecord::FindInt(const int nCol, const int64_t value, AFIDataList& varResult)
{
    return 0;
}

int AFRecord::FindDouble(const int nCol, const double value, AFIDataList& varResult)
{
    return 0;
}

int AFRecord::FindString(const int nCol, const std::string& value, AFIDataList& varResult)
{
    return 0;
}

int AFRecord::FindObject(const int nCol, const AFGUID& value, AFIDataList& varResult)
{
    return 0;
}

bool AFRecord::QueryRow(const int nRow, AFIDataList& varList)
{
    return true;
}

