// -------------------------------------------------------------------------
//    @FileName         :    AFCRecordManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-17
//    @Module           :    AFCRecordManager
//
// -------------------------------------------------------------------------

#include "AFCRecordManager.h"

AFCRecordManager::~AFCRecordManager()
{
    ClearAll();
}

NF_SHARE_PTR<AFIRecord> AFCRecordManager::AddRecord(const AFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<AFIDataList>& ValueList, const NF_SHARE_PTR<AFIDataList>& tagList, const int nRows)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr == pRecord)
    {
        //NF_SHARE_PTR<AFIRecord>
        pRecord = NF_SHARE_PTR<AFIRecord>(NF_NEW AFCRecord(self, strRecordName, ValueList, tagList, nRows));
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
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetBool(nRow, nCol, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag, const bool value)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetBool(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int32_t value)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetInt(nRow, nCol, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int32_t value)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetInt(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol, const int64_t value)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetInt64(nRow, nCol, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64_t value)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetInt64(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const float value)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetFloat(nRow, nCol, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const float value)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetFloat(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetDouble(nRow, nCol, dwValue);
    }

    return false;
}

bool AFCRecordManager::SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetDouble(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetString(nRow, nCol, strValue);
    }

    return false;
}

bool AFCRecordManager::SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetString(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const AFGUID& obj)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetObject(nRow, nCol, obj);
    }

    return false;
}

bool AFCRecordManager::SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const AFGUID& value)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetObject(nRow, strColTag, value);
    }

    return false;
}

bool AFCRecordManager::GetRecordBool(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetBool(nRow, nCol);
    }

    return NULL_BOOLEAN;
}

bool AFCRecordManager::GetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetBool(nRow, strColTag);
    }

    return NULL_BOOLEAN;
}

int32_t AFCRecordManager::GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetInt(nRow, nCol);
    }

    return NULL_INT;
}

int32_t AFCRecordManager::GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetInt(nRow, strColTag);
    }

    return NULL_INT;
}

int64_t AFCRecordManager::GetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetInt64(nRow, nCol);
    }

    return NULL_INT64;
}

int64_t AFCRecordManager::GetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetInt64(nRow, strColTag);
    }

    return NULL_INT64;
}

float AFCRecordManager::GetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetFloat(nRow, nCol);
    }

    return NULL_FLOAT;
}

float AFCRecordManager::GetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetFloat(nRow, strColTag);
    }

    return NULL_FLOAT;
}

double AFCRecordManager::GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetDouble(nRow, nCol);
    }

    return NULL_DOUBLE;
}

double AFCRecordManager::GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetDouble(nRow, strColTag);
    }

    return NULL_DOUBLE;
}

const std::string& AFCRecordManager::GetRecordString(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetString(nRow, nCol);
    }

    return NULL_STR;
}

const std::string& AFCRecordManager::GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetString(nRow, strColTag);
    }

    return NULL_STR;
}

const AFGUID& AFCRecordManager::GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetObject(nRow, nCol);
    }

    return NULL_GUID;
}

const AFGUID& AFCRecordManager::GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<AFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetObject(nRow, strColTag);
    }

    return NULL_GUID;
}