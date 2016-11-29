// -------------------------------------------------------------------------
//    @FileName         :    NFIRecordManager.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-17
//    @Module           :    NFIRecordManager
//
// -------------------------------------------------------------------------

#ifndef NFI_RECORDMANAGER_H
#define NFI_RECORDMANAGER_H

#include "NFIRecord.h"
#include "NFMap.h"

/**
 * @class   NFIRecordManager
 *
 * @brief   Manager for nfi records.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFIRecordManager
    : public NFMapEx<std::string, NFIRecord>
{
public:
    virtual ~NFIRecordManager() {}

    /**
     * @fn  virtual NF_SHARE_PTR<NFIRecord> NFIRecordManager::AddRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<NFIDataList>& TData, const NF_SHARE_PTR<NFIDataList>& tagData, const int nRows) = 0;
     *
     * @brief   Adds a record.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   self            The class instance that this method operates on.
     * @param   strRecordName   Name of the record.
     * @param   TData           The data.
     * @param   tagData         Information describing the tag.
     * @param   nRows           The rows.
     *
     * @return  A NF_SHARE_PTR&lt;NFIRecord&gt;
     */

    virtual NF_SHARE_PTR<NFIRecord> AddRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<NFIDataList>& TData, const NF_SHARE_PTR<NFIDataList>& tagData, const int nRows) = 0;

    /**
     * @fn  virtual const NFGUID& NFIRecordManager::Self() = 0;
     *
     * @brief   Gets the self.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A reference to a const NFGUID.
     */

    virtual const NFGUID& Self() = 0;

    /**
     * @fn  virtual bool NFIRecordManager::SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 nValue) = 0;
     *
     * @brief   ///////////////////////////////////////////////////////////////////////.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   nValue          The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 nValue) = 0;

    /**
     * @fn  virtual bool NFIRecordManager::SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue) = 0;
     *
     * @brief   Sets record double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   dwValue         The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue) = 0;

    /**
     * @fn  virtual bool NFIRecordManager::SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue) = 0;
     *
     * @brief   Sets record string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   strValue        The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue) = 0;

    /**
     * @fn  virtual bool NFIRecordManager::SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& obj) = 0;
     *
     * @brief   Sets record object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   obj             The object.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& obj) = 0;

    /**
     * @fn  virtual bool NFIRecordManager::SetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol, const Point3D& value) = 0;
     *
     * @brief   Sets record point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol, const Point3D& value) = 0;

    /**
     * @fn  virtual bool NFIRecordManager::SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value) = 0;
     *
     * @brief   Sets record int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value) = 0;

    /**
     * @fn  virtual bool NFIRecordManager::SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value) = 0;
     *
     * @brief   Sets record double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value) = 0;

    /**
     * @fn  virtual bool NFIRecordManager::SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value) = 0;
     *
     * @brief   Sets record string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value) = 0;

    /**
     * @fn  virtual bool NFIRecordManager::SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFGUID& value) = 0;
     *
     * @brief   Sets record object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFGUID& value) = 0;

    /**
     * @fn  virtual bool NFIRecordManager::SetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Point3D& value) = 0;
     *
     * @brief   Sets record point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Point3D& value) = 0;

    /**
     * @fn  virtual NFINT64 NFIRecordManager::GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol) = 0;
     *
     * @brief   Gets record int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record int.
     */

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol) = 0;

    /**
     * @fn  virtual double NFIRecordManager::GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol) = 0;
     *
     * @brief   Gets record double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record double.
     */

    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol) = 0;

    /**
     * @fn  virtual const std::string& NFIRecordManager::GetRecordString(const std::string& strRecordName, const int nRow, const int nCol) = 0;
     *
     * @brief   Gets record string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record string.
     */

    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const int nCol) = 0;

    /**
     * @fn  virtual const NFGUID& NFIRecordManager::GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol) = 0;
     *
     * @brief   Gets record object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record object.
     */

    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol) = 0;

    /**
     * @fn  virtual const Point3D& NFIRecordManager::GetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol) = 0;
     *
     * @brief   Gets record point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record point.
     */

    virtual const Point3D& GetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol) = 0;

    /**
     * @fn  virtual NFINT64 NFIRecordManager::GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
     *
     * @brief   Gets record int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record int.
     */

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

    /**
     * @fn  virtual double NFIRecordManager::GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
     *
     * @brief   Gets record double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record double.
     */

    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

    /**
     * @fn  virtual const std::string& NFIRecordManager::GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
     *
     * @brief   Gets record string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record string.
     */

    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

    /**
     * @fn  virtual const NFGUID& NFIRecordManager::GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
     *
     * @brief   Gets record object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record object.
     */

    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

    /**
     * @fn  virtual const Point3D& NFIRecordManager::GetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
     *
     * @brief   Gets record point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record point.
     */

    virtual const Point3D& GetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
};


#endif
