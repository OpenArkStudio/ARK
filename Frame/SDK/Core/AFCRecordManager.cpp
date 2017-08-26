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
// * @file  	AFCRecordManager.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFCRecordManager                                                  *
*****************************************************************************/
#include "AFCRecordManager.h"

AFCRecordManager::~AFCRecordManager()
{
    ClearAll();
}

ARK_SHARE_PTR<AFIRecord> AFCRecordManager::AddRecord(const AFGUID& self, const std::string& strRecordName, const ARK_SHARE_PTR<AFIDataList>& ValueList, const ARK_SHARE_PTR<AFIDataList>& tagList, const int nRows)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr == pRecord)
    {
        pRecord = ARK_SHARE_PTR<AFIRecord>(ARK_NEW AFCRecord(self, strRecordName, ValueList, tagList, nRows));
        this->AddElement(strRecordName, pRecord);
    }

    return pRecord;
}

const AFGUID& AFCRecordManager::Self()
{
    return mSelf;
}

bool AFCRecordManager::SetRecordBool(const std::string& strRecordName, const int nRow, const int nCol, const bool value)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetBool(nRow, nCol, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag, const bool value)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetBool(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int32_t value)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetInt(nRow, nCol, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int32_t value)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetInt(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol, const int64_t value)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetInt64(nRow, nCol, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64_t value)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetInt64(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const float value)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetFloat(nRow, nCol, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const float value)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetFloat(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetDouble(nRow, nCol, dwValue);
    }

    return false;
}

bool AFCRecordManager::SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetDouble(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetString(nRow, nCol, strValue);
    }

    return false;
}

bool AFCRecordManager::SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetString(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const AFGUID& obj)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetObject(nRow, nCol, obj);
    }

    return false;
}

bool AFCRecordManager::SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const AFGUID& value)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetObject(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::GetRecordBool(const std::string& strRecordName, const int nRow, const int nCol)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetBool(nRow, nCol);
    }

    return NULL_BOOLEAN;
}

bool AFCRecordManager::GetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetBool(nRow, strColTag);
    }

    return NULL_BOOLEAN;
}

int32_t AFCRecordManager::GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetInt(nRow, nCol);
    }

    return NULL_INT;
}

int32_t AFCRecordManager::GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetInt(nRow, strColTag);
    }

    return NULL_INT;
}

int64_t AFCRecordManager::GetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetInt64(nRow, nCol);
    }

    return NULL_INT64;
}

int64_t AFCRecordManager::GetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetInt64(nRow, strColTag);
    }

    return NULL_INT64;
}

float AFCRecordManager::GetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetFloat(nRow, nCol);
    }

    return NULL_FLOAT;
}

float AFCRecordManager::GetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetFloat(nRow, strColTag);
    }

    return NULL_FLOAT;
}

double AFCRecordManager::GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetDouble(nRow, nCol);
    }

    return NULL_DOUBLE;
}

double AFCRecordManager::GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetDouble(nRow, strColTag);
    }

    return NULL_DOUBLE;
}

const std::string& AFCRecordManager::GetRecordString(const std::string& strRecordName, const int nRow, const int nCol)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetString(nRow, nCol);
    }

    return NULL_STR;
}

const std::string& AFCRecordManager::GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetString(nRow, strColTag);
    }

    return NULL_STR;
}

const AFGUID& AFCRecordManager::GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetObject(nRow, nCol);
    }

    return NULL_GUID;
}

const AFGUID& AFCRecordManager::GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    ARK_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetObject(nRow, strColTag);
    }

    return NULL_GUID;
}
