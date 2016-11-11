// -------------------------------------------------------------------------
//    @FileName         :    NFCRecordManager.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-17
//    @Module           :    NFCRecordManager
//
// -------------------------------------------------------------------------

#ifndef NFC_RECORDMANAGER_H
#define NFC_RECORDMANAGER_H

#include <map>
#include <list>
#include <string>
#include <vector>
#include "NFCRecord.h"
#include "NFIRecordManager.h"

class NFCRecordManager : public NFIRecordManager
{
public:
    NFCRecordManager(const NFGUID& self)
    {
        mSelf = self;
    }

    virtual ~NFCRecordManager();

    virtual NF_SHARE_PTR<NFIRecord> AddRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<NFIDataList>& TData, const NF_SHARE_PTR<NFIDataList>& tagData, const int nRows);

    virtual const NFGUID& Self();

    //////////////////////////////////////////////////////////////////////////

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 value);
    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double value);
    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& value);
    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& value);
    virtual bool SetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol, const Point3D& value);

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value);
    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value);
    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value);
    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFGUID& value);
    virtual bool SetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Point3D& value);

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol);
    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol);
    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const int nCol);
    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol);
    virtual const Point3D& GetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol);

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual const Point3D& GetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    //////////////////////////////////////////////////////////////////////////
private:
    NFGUID mSelf;

};

#endif
