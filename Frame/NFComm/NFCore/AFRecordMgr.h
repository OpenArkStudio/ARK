// -------------------------------------------------------------------------
//    @FileName         :    AFRecordMgr.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-17
//    @Module           :    AFRecordMgr
//
// -------------------------------------------------------------------------
#pragma once

#include "AFString.hpp"
#include "AFArrayPod.hpp"
#include "AFDataList.hpp"

class AFRecordMgr
{
private:
    

public:
    AFRecordMgr();
    ~AFRecordMgr();
    NF_SHARE_PTR<NFIRecord> AddRecord(const std::string& strRecordName, const NF_SHARE_PTR<AFDataList>& TData, const NF_SHARE_PTR<AFDataList>& tagData, const int nRows);
    bool SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 value);
    bool SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double value);
    bool SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& value);
    bool SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& value);
    bool SetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol, const Point3D& value);

    NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol);
    double GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol);
    const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const int nCol);
    const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol);
    const Point3D& GetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol);

private:

};