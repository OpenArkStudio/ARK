// -------------------------------------------------------------------------
//    @FileName         :    NFCRecord.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFCRecord
//
// -------------------------------------------------------------------------

#ifndef _GT_RECORD_H
#define _GT_RECORD_H

#include <vector>
#include "NFIRecord.h"
#include "NFCDataList.h"
#include "NFMapEx.h"

/**
 * @class   NFCRecord
 *
 * @brief   Information about the nfc.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class NFCRecord : public NFIRecord
{
public:

    /**
     * @fn  NFCRecord::NFCRecord();
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    NFCRecord();

    /**
     * @fn  NFCRecord::NFCRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<NFIDataList>& valueList, const NF_SHARE_PTR<NFIDataList>& tagList, const int nMaxRow);
     *
     * @brief   Constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   self            The class instance that this method operates on.
     * @param   strRecordName   Name of the record.
     * @param   valueList       List of values.
     * @param   tagList         List of tags.
     * @param   nMaxRow         The maximum row.
     */

    NFCRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<NFIDataList>& valueList, const NF_SHARE_PTR<NFIDataList>& tagList, const int nMaxRow);

    /**
     * @fn  virtual NFCRecord::~NFCRecord();
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    virtual ~NFCRecord();

    /**
     * @fn  virtual bool NFCRecord::IsUsed(const int nRow) const;
     *
     * @brief   Query if 'nRow' is used.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     *
     * @return  True if used, false if not.
     */

    virtual bool IsUsed(const int nRow) const;

    /**
     * @fn  virtual bool NFCRecord::SetUsed(const int nRow, const int bUse);
     *
     * @brief   Sets an used.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   bUse    The use.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetUsed(const int nRow, const int bUse);

    /**
     * @fn  virtual int NFCRecord::GetCols() const;
     *
     * @brief   Gets the cols.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The cols.
     */

    virtual int GetCols() const;

    /**
     * @fn  virtual int NFCRecord::GetRows() const;
     *
     * @brief   Gets the rows.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The rows.
     */

    virtual int GetRows() const;

    /**
     * @fn  virtual TDATA_TYPE NFCRecord::GetColType(const int nCol) const;
     *
     * @brief   Gets col type.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nCol    The col.
     *
     * @return  The col type.
     */

    virtual TDATA_TYPE GetColType(const int nCol) const;

    /**
     * @fn  virtual const std::string& NFCRecord::GetColTag(const int nCol) const;
     *
     * @brief   Gets col tag.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nCol    The col.
     *
     * @return  The col tag.
     */

    virtual const std::string& GetColTag(const int nCol) const;

    /**
     * @fn  virtual int NFCRecord::AddRow(const int nRow);
     *
     * @brief   添加数据.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     *
     * @return  An int.
     */

    virtual int AddRow(const int nRow);

    /**
     * @fn  virtual int NFCRecord::AddRow(const int nRow, const NFIDataList& var);
     *
     * @brief   Adds a row to 'var'.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   var     The variable.
     *
     * @return  An int.
     */

    virtual int AddRow(const int nRow, const NFIDataList& var);

    /**
     * @fn  virtual bool NFCRecord::SetInt(const int nRow, const int nCol, const NFINT64 value);
     *
     * @brief   Sets an int.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetInt(const int nRow, const int nCol, const NFINT64 value);

    /**
     * @fn  virtual bool NFCRecord::SetDouble(const int nRow, const int nCol, const double value);
     *
     * @brief   Sets a double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetDouble(const int nRow, const int nCol, const double value);

    /**
     * @fn  virtual bool NFCRecord::SetString(const int nRow, const int nCol, const std::string& value);
     *
     * @brief   Sets a string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetString(const int nRow, const int nCol, const std::string& value);

    /**
     * @fn  virtual bool NFCRecord::SetObject(const int nRow, const int nCol, const NFGUID& value);
     *
     * @brief   Sets an object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetObject(const int nRow, const int nCol, const NFGUID& value);

    /**
     * @fn  virtual bool NFCRecord::SetPoint(const int nRow, const int nCol, const Point3D& value);
     *
     * @brief   Sets a point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPoint(const int nRow, const int nCol, const Point3D& value);

    /**
     * @fn  virtual bool NFCRecord::SetInt(const int nRow, const std::string& strColTag, const NFINT64 value);
     *
     * @brief   Sets an int.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     * @param   value       The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetInt(const int nRow, const std::string& strColTag, const NFINT64 value);

    /**
     * @fn  virtual bool NFCRecord::SetDouble(const int nRow, const std::string& strColTag, const double value);
     *
     * @brief   Sets a double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     * @param   value       The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetDouble(const int nRow, const std::string& strColTag, const double value);

    /**
     * @fn  virtual bool NFCRecord::SetString(const int nRow, const std::string& strColTag, const std::string& value);
     *
     * @brief   Sets a string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     * @param   value       The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetString(const int nRow, const std::string& strColTag, const std::string& value);

    /**
     * @fn  virtual bool NFCRecord::SetObject(const int nRow, const std::string& strColTag, const NFGUID& value);
     *
     * @brief   Sets an object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     * @param   value       The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetObject(const int nRow, const std::string& strColTag, const NFGUID& value);

    /**
     * @fn  virtual bool NFCRecord::SetPoint(const int nRow, const std::string& strColTag, const Point3D& value);
     *
     * @brief   Sets a point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     * @param   value       The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPoint(const int nRow, const std::string& strColTag, const Point3D& value);

    /**
     * @fn  virtual NFINT64 NFCRecord::GetInt(const int nRow, const int nCol) const;
     *
     * @brief   Gets an int.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  The int.
     */

    virtual NFINT64 GetInt(const int nRow, const int nCol) const;

    /**
     * @fn  virtual double NFCRecord::GetDouble(const int nRow, const int nCol) const;
     *
     * @brief   Gets a double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  The double.
     */

    virtual double GetDouble(const int nRow, const int nCol) const;

    /**
     * @fn  virtual const std::string& NFCRecord::GetString(const int nRow, const int nCol) const;
     *
     * @brief   Gets a string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  The string.
     */

    virtual const std::string& GetString(const int nRow, const int nCol) const;

    /**
     * @fn  virtual const NFGUID& NFCRecord::GetObject(const int nRow, const int nCol) const;
     *
     * @brief   Gets an object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  The object.
     */

    virtual const NFGUID& GetObject(const int nRow, const int nCol) const;

    /**
     * @fn  virtual const Point3D& NFCRecord::GetPoint(const int nRow, const int nCol) const;
     *
     * @brief   Gets a point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  The point.
     */

    virtual const Point3D& GetPoint(const int nRow, const int nCol) const;

    /**
     * @fn  virtual NFINT64 NFCRecord::GetInt(const int nRow, const std::string& strColTag) const;
     *
     * @brief   Gets an int.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     *
     * @return  The int.
     */

    virtual NFINT64 GetInt(const int nRow, const std::string& strColTag) const;

    /**
     * @fn  virtual double NFCRecord::GetDouble(const int nRow, const std::string& strColTag) const;
     *
     * @brief   Gets a double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     *
     * @return  The double.
     */

    virtual double GetDouble(const int nRow, const std::string& strColTag) const;

    /**
     * @fn  virtual const std::string& NFCRecord::GetString(const int nRow, const std::string& strColTag) const;
     *
     * @brief   Gets a string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     *
     * @return  The string.
     */

    virtual const std::string& GetString(const int nRow, const std::string& strColTag) const;

    /**
     * @fn  virtual const NFGUID& NFCRecord::GetObject(const int nRow, const std::string& strColTag) const;
     *
     * @brief   Gets an object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     *
     * @return  The object.
     */

    virtual const NFGUID& GetObject(const int nRow, const std::string& strColTag) const;

    /**
     * @fn  virtual const Point3D& NFCRecord::GetPoint(const int nRow, const std::string& strColTag) const;
     *
     * @brief   Gets a point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     *
     * @return  The point.
     */

    virtual const Point3D& GetPoint(const int nRow, const std::string& strColTag) const;

    /**
     * @fn  virtual int NFCRecord::FindRowByColValue(const int nCol, const NFIDataList& var, NFIDataList& varResult);
     *
     * @brief   Searches for the first row by col value.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           nCol        The col.
     * @param           var         The variable.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found row by col value.
     */

    virtual int FindRowByColValue(const int nCol, const NFIDataList& var, NFIDataList& varResult);

    /**
     * @fn  virtual int NFCRecord::FindInt(const int nCol, const NFINT64 value, NFIDataList& varResult);
     *
     * @brief   Searches for the first int.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           nCol        The col.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found int.
     */

    virtual int FindInt(const int nCol, const NFINT64 value, NFIDataList& varResult);

    /**
     * @fn  virtual int NFCRecord::FindDouble(const int nCol, const double value, NFIDataList& varResult);
     *
     * @brief   Searches for the first double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           nCol        The col.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found double.
     */

    virtual int FindDouble(const int nCol, const double value, NFIDataList& varResult);

    /**
     * @fn  virtual int NFCRecord::FindString(const int nCol, const std::string& value, NFIDataList& varResult);
     *
     * @brief   Searches for the first string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           nCol        The col.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found string.
     */

    virtual int FindString(const int nCol, const std::string& value, NFIDataList& varResult);

    /**
     * @fn  virtual int NFCRecord::FindObject(const int nCol, const NFGUID& value, NFIDataList& varResult);
     *
     * @brief   Searches for the first object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           nCol        The col.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found object.
     */

    virtual int FindObject(const int nCol, const NFGUID& value, NFIDataList& varResult);

    /**
     * @fn  virtual int NFCRecord::FindPoint(const int nCol, const Point3D& value, NFIDataList& varResult);
     *
     * @brief   Searches for the first point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           nCol        The col.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found point.
     */

    virtual int FindPoint(const int nCol, const Point3D& value, NFIDataList& varResult);

    /**
     * @fn  virtual int NFCRecord::FindRowByColValue(const std::string& strColTag, const NFIDataList& var, NFIDataList& varResult);
     *
     * @brief   Searches for the first row by col value.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           strColTag   The col tag.
     * @param           var         The variable.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found row by col value.
     */

    virtual int FindRowByColValue(const std::string& strColTag, const NFIDataList& var, NFIDataList& varResult);

    /**
     * @fn  virtual int NFCRecord::FindInt(const std::string& strColTag, const NFINT64 value, NFIDataList& varResult);
     *
     * @brief   Searches for the first int.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           strColTag   The col tag.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found int.
     */

    virtual int FindInt(const std::string& strColTag, const NFINT64 value, NFIDataList& varResult);

    /**
     * @fn  virtual int NFCRecord::FindDouble(const std::string& strColTag, const double value, NFIDataList& varResult);
     *
     * @brief   Searches for the first double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           strColTag   The col tag.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found double.
     */

    virtual int FindDouble(const std::string& strColTag, const double value, NFIDataList& varResult);

    /**
     * @fn  virtual int NFCRecord::FindString(const std::string& strColTag, const std::string& value, NFIDataList& varResult);
     *
     * @brief   Searches for the first string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           strColTag   The col tag.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found string.
     */

    virtual int FindString(const std::string& strColTag, const std::string& value, NFIDataList& varResult);

    /**
     * @fn  virtual int NFCRecord::FindObject(const std::string& strColTag, const NFGUID& value, NFIDataList& varResult);
     *
     * @brief   Searches for the first object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           strColTag   The col tag.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found object.
     */

    virtual int FindObject(const std::string& strColTag, const NFGUID& value, NFIDataList& varResult);

    /**
     * @fn  virtual int NFCRecord::FindPoint(const std::string& strColTag, const Point3D& value, NFIDataList& varResult);
     *
     * @brief   Searches for the first point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           strColTag   The col tag.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found point.
     */

    virtual int FindPoint(const std::string& strColTag, const Point3D& value, NFIDataList& varResult);

    /**
     * @fn  virtual bool NFCRecord::QueryRow(const int nRow, NFIDataList& varList);
     *
     * @brief   ///////////////////////////////////////////////////////////////////////.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           nRow    The row.
     * @param [in,out]  varList List of variables.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool QueryRow(const int nRow, NFIDataList& varList);

    /**
     * @fn  virtual bool NFCRecord::SwapRowInfo(const int nOriginRow, const int nTargetRow);
     *
     * @brief   Swap row information.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nOriginRow  The origin row.
     * @param   nTargetRow  Target row.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SwapRowInfo(const int nOriginRow, const int nTargetRow);

    /**
     * @fn  virtual bool NFCRecord::Remove(const int nRow);
     *
     * @brief   Removes the given nRow.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row to remove.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Remove(const int nRow);

    /**
     * @fn  virtual bool NFCRecord::Clear();
     *
     * @brief   Clears this object to its blank/initial state.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Clear();

    /**
     * @fn  virtual void NFCRecord::AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb);
     *
     * @brief   Adds a record hook.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   cb  The cb.
     */

    virtual void AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb);

    /**
     * @fn  virtual const bool NFCRecord::GetSave();
     *
     * @brief   Gets the save.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The save.
     */

    virtual const bool GetSave();

    /**
     * @fn  virtual const bool NFCRecord::GetCache();
     *
     * @brief   Gets the cache.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The cache.
     */

    virtual const bool GetCache();

    /**
     * @fn  virtual const bool NFCRecord::GetPublic();
     *
     * @brief   Gets the public.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The public.
     */

    virtual const bool GetPublic();

    /**
     * @fn  virtual const bool NFCRecord::GetPrivate();
     *
     * @brief   Gets the private.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The private.
     */

    virtual const bool GetPrivate();

    /**
     * @fn  virtual const std::string& NFCRecord::GetName() const;
     *
     * @brief   Gets the name.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The name.
     */

    virtual const std::string& GetName() const;

    /**
     * @fn  virtual void NFCRecord::SetSave(const bool bSave);
     *
     * @brief   Sets a save.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   bSave   The save.
     */

    virtual void SetSave(const bool bSave);

    /**
     * @fn  virtual void NFCRecord::SetCache(const bool bCache);
     *
     * @brief   Sets a cache.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   bCache  The cache.
     */

    virtual void SetCache(const bool bCache);

    /**
     * @fn  virtual void NFCRecord::SetPublic(const bool bPublic);
     *
     * @brief   Sets a public.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   bPublic The public.
     */

    virtual void SetPublic(const bool bPublic);

    /**
     * @fn  virtual void NFCRecord::SetPrivate(const bool bPrivate);
     *
     * @brief   Sets a private.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   bPrivate    The private.
     */

    virtual void SetPrivate(const bool bPrivate);

    /**
     * @fn  virtual void NFCRecord::SetName(const std::string& strName);
     *
     * @brief   Sets a name.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strName The name.
     */

    virtual void SetName(const std::string& strName);

    /**
     * @fn  virtual const NF_SHARE_PTR<NFIDataList> NFCRecord::GetInitData() const;
     *
     * @brief   Gets init data.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The init data.
     */

    virtual const NF_SHARE_PTR<NFIDataList> GetInitData() const;

    /**
     * @fn  virtual const NF_SHARE_PTR<NFIDataList> NFCRecord::GetTag() const;
     *
     * @brief   Gets the tag.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The tag.
     */

    virtual const NF_SHARE_PTR<NFIDataList> GetTag() const;

    /**
     * @fn  virtual const TRECORDVEC& NFCRecord::GetRecordVec() const;
     *
     * @brief   Gets record vector.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The record vector.
     */

    virtual const TRECORDVEC& GetRecordVec() const;
protected:

    /**
     * @fn  int NFCRecord::GetPos(int nRow, int nCol) const;
     *
     * @brief   Gets a position.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  The position.
     */

    int GetPos(int nRow, int nCol) const;

    /**
     * @fn  int NFCRecord::GetCol(const std::string& strTag) const;
     *
     * @brief   Gets a col.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strTag  The tag.
     *
     * @return  The col.
     */

    int GetCol(const std::string& strTag) const;

    /**
     * @fn  bool NFCRecord::ValidPos(int nRow, int nCol) const;
     *
     * @brief   Valid position.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool ValidPos(int nRow, int nCol) const;

    /**
     * @fn  bool NFCRecord::ValidRow(int nRow) const;
     *
     * @brief   Valid row.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nRow    The row.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool ValidRow(int nRow) const;

    /**
     * @fn  bool NFCRecord::ValidCol(int nCol) const;
     *
     * @brief   Valid col.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nCol    The col.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool ValidCol(int nCol) const;

    /**
     * @fn  bool NFCRecord::ValidCheck(TDATA_TYPE eType, const NFIDataList::TData& var, NF_SHARE_PTR<NFIDataList::TData>& pVar);
     *
     * @brief   Valid check.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           eType   The type.
     * @param           var     The variable.
     * @param [in,out]  pVar    The variable.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool ValidCheck(TDATA_TYPE eType, const NFIDataList::TData& var, NF_SHARE_PTR<NFIDataList::TData>& pVar);

    /**
     * @fn  void NFCRecord::OnEventHandler(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFIDataList::TData& oldVar, const NFIDataList::TData& newVar);
     *
     * @brief   Executes the event handler action.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   self        The class instance that this method operates on.
     * @param   xEventData  Information describing the event.
     * @param   oldVar      The old variable.
     * @param   newVar      The new variable.
     */

    void OnEventHandler(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFIDataList::TData& oldVar, const NFIDataList::TData& newVar);

protected:
    //记录这个表的Key类型，那样在读取和设置的时候才能保持正确
    /** @brief   初始值类型--应该引用静态的(或者智能指针)，节约大量内存. */
    NF_SHARE_PTR<NFIDataList> mVarRecordType;
    /** @brief   col的tag值--应该引用静态的(或者智能指针)，节约大量内存. */
    NF_SHARE_PTR<NFIDataList> mVarRecordTag;

    /** @brief   tag->col转换. */
    std::map<std::string, int> mmTag;

    /** @brief   /////////////////////////. */

    TRECORDVEC mtRecordVec;
    /** @brief   State of the vector used. */
    std::vector<int> mVecUsedState;
    /** @brief   The mn maximum row. */
    int mnMaxRow;

    /** @brief   The self. */
    NFGUID mSelf;
    /** @brief   True to megabytes save. */
    bool mbSave;
    /** @brief   True to megabytes public. */
    bool mbPublic;
    /** @brief   True to megabytes private. */
    bool mbPrivate;
    /** @brief   True to megabytes cache. */
    bool mbCache;
    /** @brief   Name of the mstr record. */
    std::string mstrRecordName;

    /**
     * @typedef std::vector<RECORD_EVENT_FUNCTOR_PTR> TRECORDCALLBACKEX
     *
     * @brief   Defines an alias representing the trecordcallbackex.
     */

    typedef std::vector<RECORD_EVENT_FUNCTOR_PTR> TRECORDCALLBACKEX;
    /** @brief   The mt record callback. */
    TRECORDCALLBACKEX mtRecordCallback;
};

#endif
