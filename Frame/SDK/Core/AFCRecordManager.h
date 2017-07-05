// -------------------------------------------------------------------------
//    @FileName         :    AFCRecordManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-17
//    @Module           :    AFCRecordManager
//
// -------------------------------------------------------------------------

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

    virtual NF_SHARE_PTR<AFIRecord> AddRecord(const AFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<AFIDataList>& TData, const NF_SHARE_PTR<AFIDataList>& tagData, const int nRows);

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