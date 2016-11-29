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

/**
 * @class   NFCRecordManager
 *
 * @brief   Manager for nfc records.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFCRecordManager : public NFIRecordManager
{
public:

    /**
     * @fn  NFCRecordManager::NFCRecordManager(const NFGUID& self)
     *
     * @brief   Constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   self    The class instance that this method operates on.
     */

    NFCRecordManager(const NFGUID& self)
    {
        mSelf = self;
    }

    /**
     * @fn  virtual NFCRecordManager::~NFCRecordManager();
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    virtual ~NFCRecordManager();

    /**
     * @fn  virtual NF_SHARE_PTR<NFIRecord> NFCRecordManager::AddRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<NFIDataList>& TData, const NF_SHARE_PTR<NFIDataList>& tagData, const int nRows);
     *
     * @brief   Adds a record.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   self            The class instance that this method operates on.
     * @param   strRecordName   Name of the record.
     * @param   TData           The data.
     * @param   tagData         Information describing the tag.
     * @param   nRows           The rows.
     *
     * @return  A NF_SHARE_PTR&lt;NFIRecord&gt;
     */

    virtual NF_SHARE_PTR<NFIRecord> AddRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<NFIDataList>& TData, const NF_SHARE_PTR<NFIDataList>& tagData, const int nRows);

    /**
     * @fn  virtual const NFGUID& NFCRecordManager::Self();
     *
     * @brief   Gets the self.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  A reference to a const NFGUID.
     */

    virtual const NFGUID& Self();

    /**
     * @fn  virtual bool NFCRecordManager::SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 value);
     *
     * @brief   ///////////////////////////////////////////////////////////////////////.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 value);

    /**
     * @fn  virtual bool NFCRecordManager::SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double value);
     *
     * @brief   Sets record double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double value);

    /**
     * @fn  virtual bool NFCRecordManager::SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& value);
     *
     * @brief   Sets record string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& value);

    /**
     * @fn  virtual bool NFCRecordManager::SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& value);
     *
     * @brief   Sets record object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& value);

    /**
     * @fn  virtual bool NFCRecordManager::SetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol, const Point3D& value);
     *
     * @brief   Sets record point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol, const Point3D& value);

    /**
     * @fn  virtual bool NFCRecordManager::SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value);
     *
     * @brief   Sets record int.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value);

    /**
     * @fn  virtual bool NFCRecordManager::SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value);
     *
     * @brief   Sets record double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value);

    /**
     * @fn  virtual bool NFCRecordManager::SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value);
     *
     * @brief   Sets record string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value);

    /**
     * @fn  virtual bool NFCRecordManager::SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFGUID& value);
     *
     * @brief   Sets record object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFGUID& value);

    /**
     * @fn  virtual bool NFCRecordManager::SetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Point3D& value);
     *
     * @brief   Sets record point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Point3D& value);

    /**
     * @fn  virtual NFINT64 NFCRecordManager::GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol);
     *
     * @brief   Gets record int.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record int.
     */

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol);

    /**
     * @fn  virtual double NFCRecordManager::GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol);
     *
     * @brief   Gets record double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record double.
     */

    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol);

    /**
     * @fn  virtual const std::string& NFCRecordManager::GetRecordString(const std::string& strRecordName, const int nRow, const int nCol);
     *
     * @brief   Gets record string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record string.
     */

    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const int nCol);

    /**
     * @fn  virtual const NFGUID& NFCRecordManager::GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol);
     *
     * @brief   Gets record object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record object.
     */

    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol);

    /**
     * @fn  virtual const Point3D& NFCRecordManager::GetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol);
     *
     * @brief   Gets record point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record point.
     */

    virtual const Point3D& GetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol);

    /**
     * @fn  virtual NFINT64 NFCRecordManager::GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag);
     *
     * @brief   Gets record int.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record int.
     */

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag);

    /**
     * @fn  virtual double NFCRecordManager::GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag);
     *
     * @brief   Gets record double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record double.
     */

    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag);

    /**
     * @fn  virtual const std::string& NFCRecordManager::GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag);
     *
     * @brief   Gets record string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record string.
     */

    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag);

    /**
     * @fn  virtual const NFGUID& NFCRecordManager::GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag);
     *
     * @brief   Gets record object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record object.
     */

    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag);

    /**
     * @fn  virtual const Point3D& NFCRecordManager::GetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag);
     *
     * @brief   Gets record point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record point.
     */

    virtual const Point3D& GetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    /** @brief   ///////////////////////////////////////////////////////////////////////. */
private:
    NFGUID mSelf;

};

#endif
