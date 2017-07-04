// -------------------------------------------------------------------------
//    @FileName         :    NFCRecordManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-17
//    @Module           :    NFCRecordManager
//
// -------------------------------------------------------------------------

#include "NFCRecordManager.h"

NFCRecordManager::~NFCRecordManager()
{
    ClearAll();
}

NF_SHARE_PTR<NFIRecord> NFCRecordManager::AddRecord(const AFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<AFIDataList>& ValueList, const NF_SHARE_PTR<AFIDataList>& tagList, const int nRows)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr == pRecord)
    {
        //NF_SHARE_PTR<NFIRecord>
        pRecord = NF_SHARE_PTR<NFIRecord>(NF_NEW NFCRecord(self, strRecordName, ValueList, tagList, nRows));
        this->AddElement(strRecordName, pRecord);
    }

    return pRecord;
}

const AFGUID& NFCRecordManager::Self()
{
    return mSelf;
}

bool NFCRecordManager::SetRecordBool(const std::string& strRecordName, const int nRow, const int nCol, const bool value)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->SetBool(nRow, nCol, value);
    }

    return false;
}

bool NFCRecordManager::SetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag, const bool value)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->SetBool(nRow, strColTag, value);
    }

    return false;
}

bool NFCRecordManager::SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int32_t value)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetInt(nRow, nCol, value);
    }

    return false;
}

bool NFCRecordManager::SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int32_t value)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetInt(nRow, strColTag, value);
    }

    return false;
}

bool NFCRecordManager::SetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol, const int64_t value)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->SetInt64(nRow, nCol, value);
    }

    return false;
}

bool NFCRecordManager::SetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64_t value)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->SetInt64(nRow, strColTag, value);
    }

    return false;
}

bool NFCRecordManager::SetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const float value)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->SetFloat(nRow, nCol, value);
    }

    return false;
}

bool NFCRecordManager::SetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const float value)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->SetFloat(nRow, strColTag, value);
    }

    return false;
}

bool NFCRecordManager::SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetDouble(nRow, nCol, dwValue);
    }

    return false;
}

bool NFCRecordManager::SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetDouble(nRow, strColTag, value);
    }

    return false;
}

bool NFCRecordManager::SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetString(nRow, nCol, strValue);
    }

    return false;
}

bool NFCRecordManager::SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetString(nRow, strColTag, value);
    }

    return false;
}

bool NFCRecordManager::SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const AFGUID& obj)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetObject(nRow, nCol, obj);
    }

    return false;
}

bool NFCRecordManager::SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const AFGUID& value)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->SetObject(nRow, strColTag, value);
    }

    return false;
}

bool NFCRecordManager::GetRecordBool(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->GetBool(nRow, nCol);
    }

    return NULL_BOOLEAN;
}

bool NFCRecordManager::GetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->GetBool(nRow, strColTag);
    }

    return NULL_BOOLEAN;
}

int32_t NFCRecordManager::GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetInt(nRow, nCol);
    }

    return NULL_INT;
}

int32_t NFCRecordManager::GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetInt(nRow, strColTag);
    }

    return NULL_INT;
}

int64_t NFCRecordManager::GetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->GetInt64(nRow, nCol);
    }

    return NULL_INT64;
}

int64_t NFCRecordManager::GetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->GetInt64(nRow, strColTag);
    }

    return NULL_INT64;
}

float NFCRecordManager::GetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->GetFloat(nRow, nCol);
    }

    return NULL_FLOAT;
}

float NFCRecordManager::GetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (nullptr != pRecord)
    {
        return pRecord->GetFloat(nRow, strColTag);
    }

    return NULL_FLOAT;
}

double NFCRecordManager::GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetDouble(nRow, nCol);
    }

    return NULL_DOUBLE;
}

double NFCRecordManager::GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetDouble(nRow, strColTag);
    }

    return NULL_DOUBLE;
}

const std::string& NFCRecordManager::GetRecordString(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetString(nRow, nCol);
    }

    return NULL_STR;
}

const std::string& NFCRecordManager::GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetString(nRow, strColTag);
    }

    return NULL_STR;
}

const AFGUID& NFCRecordManager::GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetObject(nRow, nCol);
    }

    return NULL_GUID;
}

const AFGUID& NFCRecordManager::GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->GetObject(nRow, strColTag);
    }

    return NULL_GUID;
}