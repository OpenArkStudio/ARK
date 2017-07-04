// -------------------------------------------------------------------------
//    @FileName         :    NFIRecordManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-17
//    @Module           :    NFIRecordManager
//
// -------------------------------------------------------------------------

#pragma once

#include "NFIRecord.h"
#include "NFMap.h"

using namespace ArkFrame;

class NFIRecordManager
    : public NFMapEx<std::string, NFIRecord>
{
public:
    virtual ~NFIRecordManager() {}

    virtual NF_SHARE_PTR<NFIRecord> AddRecord(const AFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<AFIDataList>& TData, const NF_SHARE_PTR<AFIDataList>& tagData, const int nRows) = 0;

    virtual const AFGUID& Self() = 0;

    virtual bool SetRecordBool(const std::string& strRecordName, const int nRow, const int nCol, const bool value) = 0;
    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int32_t value) = 0;
    virtual bool SetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol, const int64_t value) = 0;
    virtual bool SetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const float value) = 0;
    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double value) = 0;
    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& value) = 0;
    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const AFGUID& value) = 0;

    virtual bool SetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag, const bool value) = 0;
    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int32_t value) = 0;
    virtual bool SetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64_t value) = 0;
    virtual bool SetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const float value) = 0;
    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value) = 0;
    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value) = 0;
    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const AFGUID& value) = 0;

    virtual bool GetRecordBool(const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual int32_t GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual int64_t GetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual float GetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual const AFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol) = 0;

    virtual bool GetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual int32_t GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual int64_t GetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual float GetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual const AFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
};