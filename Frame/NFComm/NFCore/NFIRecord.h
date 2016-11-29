// -------------------------------------------------------------------------
//    @FileName         :    NFIRecord.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFIRecord
//
// -------------------------------------------------------------------------

#ifndef NFI_RECORD_H
#define NFI_RECORD_H

#include "NFDefine.h"
#include "NFIDataList.h"

/**
 * @class   NFIRecord
 *
 * @brief   Information about the nfi.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFIRecord
{
public:
    enum RecordOptype
    {
        Add = 0,
        Del,
        Swap,
        Create,
        Update,
        Cleared,
        Sort,
        Cover,
    };

    typedef std::vector< NF_SHARE_PTR<NFIDataList::TData> > TRECORDVEC;
    typedef TRECORDVEC::const_iterator TRECORDVECCONSTITER;

    virtual ~NFIRecord() {}

    /**
     * @fn  virtual bool NFIRecord::IsUsed(const int nRow) const = 0;
     *
     * @brief   Query if 'nRow' is used.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     *
     * @return  True if used, false if not.
     */

    virtual bool IsUsed(const int nRow) const  = 0;

    /**
     * @fn  virtual bool NFIRecord::SetUsed(const int nRow, const int bUse) = 0;
     *
     * @brief   Sets an used.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   bUse    The use.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetUsed(const int nRow, const int bUse)  = 0;

    /**
     * @fn  virtual int NFIRecord::GetCols() const = 0;
     *
     * @brief   Gets the cols.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The cols.
     */

    virtual int GetCols() const  = 0;

    /**
     * @fn  virtual int NFIRecord::GetRows() const = 0;
     *
     * @brief   Gets the rows.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The rows.
     */

    virtual int GetRows() const  = 0;

    /**
     * @fn  virtual TDATA_TYPE NFIRecord::GetColType(const int nCol) const = 0;
     *
     * @brief   Gets col type.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nCol    The col.
     *
     * @return  The col type.
     */

    virtual TDATA_TYPE GetColType(const int nCol) const = 0;

    /**
     * @fn  virtual const std::string& NFIRecord::GetColTag(const int nCol) const = 0;
     *
     * @brief   Gets col tag.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nCol    The col.
     *
     * @return  The col tag.
     */

    virtual const std::string& GetColTag(const int nCol) const = 0;

    /**
     * @fn  virtual int NFIRecord::AddRow(const int nRow) = 0;
     *
     * @brief   添加数据.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     *
     * @return  An int.
     */

    virtual int AddRow(const int nRow) = 0;

    /**
     * @fn  virtual int NFIRecord::AddRow(const int nRow, const NFIDataList& var) = 0;
     *
     * @brief   Adds a row to 'var'.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   var     The variable.
     *
     * @return  An int.
     */

    virtual int AddRow(const int nRow, const NFIDataList& var) = 0;

    /**
     * @fn  virtual bool NFIRecord::SetInt(const int nRow, const int nCol, const NFINT64 value) = 0;
     *
     * @brief   Sets an int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetInt(const int nRow, const int nCol, const NFINT64 value) = 0;

    /**
     * @fn  virtual bool NFIRecord::SetDouble(const int nRow, const int nCol, const double value) = 0;
     *
     * @brief   Sets a double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetDouble(const int nRow, const int nCol, const double value) = 0;

    /**
     * @fn  virtual bool NFIRecord::SetString(const int nRow, const int nCol, const std::string& value) = 0;
     *
     * @brief   Sets a string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetString(const int nRow, const int nCol, const std::string& value) = 0;

    /**
     * @fn  virtual bool NFIRecord::SetObject(const int nRow, const int nCol, const NFGUID& value) = 0;
     *
     * @brief   Sets an object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetObject(const int nRow, const int nCol, const NFGUID& value) = 0;

    /**
     * @fn  virtual bool NFIRecord::SetPoint(const int nRow, const int nCol, const Point3D& value) = 0;
     *
     * @brief   Sets a point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPoint(const int nRow, const int nCol, const Point3D& value) = 0;

    /**
     * @fn  virtual bool NFIRecord::SetInt(const int nRow, const std::string& strColTag, const NFINT64 value) = 0;
     *
     * @brief   Sets an int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     * @param   value       The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetInt(const int nRow, const std::string& strColTag, const NFINT64 value) = 0;

    /**
     * @fn  virtual bool NFIRecord::SetDouble(const int nRow, const std::string& strColTag, const double value) = 0;
     *
     * @brief   Sets a double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     * @param   value       The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetDouble(const int nRow, const std::string& strColTag, const double value) = 0;

    /**
     * @fn  virtual bool NFIRecord::SetString(const int nRow, const std::string& strColTag, const std::string& value) = 0;
     *
     * @brief   Sets a string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     * @param   value       The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetString(const int nRow, const std::string& strColTag, const std::string& value) = 0;

    /**
     * @fn  virtual bool NFIRecord::SetObject(const int nRow, const std::string& strColTag, const NFGUID& value) = 0;
     *
     * @brief   Sets an object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     * @param   value       The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetObject(const int nRow, const std::string& strColTag, const NFGUID& value) = 0;

    /**
     * @fn  virtual bool NFIRecord::SetPoint(const int nRow, const std::string& strColTag, const Point3D& value) = 0;
     *
     * @brief   Sets a point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     * @param   value       The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPoint(const int nRow, const std::string& strColTag, const Point3D& value) = 0;

    /**
     * @fn  virtual NFINT64 NFIRecord::GetInt(const int nRow, const int nCol) const = 0;
     *
     * @brief   Gets an int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  The int.
     */

    virtual NFINT64 GetInt(const int nRow, const int nCol) const = 0;

    /**
     * @fn  virtual double NFIRecord::GetDouble(const int nRow, const int nCol) const = 0;
     *
     * @brief   Gets a double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  The double.
     */

    virtual double GetDouble(const int nRow, const int nCol) const = 0;

    /**
     * @fn  virtual const std::string& NFIRecord::GetString(const int nRow, const int nCol) const = 0;
     *
     * @brief   Gets a string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  The string.
     */

    virtual const std::string& GetString(const int nRow, const int nCol) const = 0;

    /**
     * @fn  virtual const NFGUID& NFIRecord::GetObject(const int nRow, const int nCol) const = 0;
     *
     * @brief   Gets an object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  The object.
     */

    virtual const NFGUID& GetObject(const int nRow, const int nCol) const = 0;

    /**
     * @fn  virtual const Point3D& NFIRecord::GetPoint(const int nRow, const int nCol) const = 0;
     *
     * @brief   Gets a point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row.
     * @param   nCol    The col.
     *
     * @return  The point.
     */

    virtual const Point3D& GetPoint(const int nRow, const int nCol) const = 0;

    /**
     * @fn  virtual NFINT64 NFIRecord::GetInt(const int nRow, const std::string& strColTag) const = 0;
     *
     * @brief   Gets an int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     *
     * @return  The int.
     */

    virtual NFINT64 GetInt(const int nRow, const std::string& strColTag) const = 0;

    /**
     * @fn  virtual double NFIRecord::GetDouble(const int nRow, const std::string& strColTag) const = 0;
     *
     * @brief   Gets a double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     *
     * @return  The double.
     */

    virtual double GetDouble(const int nRow, const std::string& strColTag) const = 0;

    /**
     * @fn  virtual const std::string& NFIRecord::GetString(const int nRow, const std::string& strColTag) const = 0;
     *
     * @brief   Gets a string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     *
     * @return  The string.
     */

    virtual const std::string& GetString(const int nRow, const std::string& strColTag) const = 0;

    /**
     * @fn  virtual const NFGUID& NFIRecord::GetObject(const int nRow, const std::string& strColTag) const = 0;
     *
     * @brief   Gets an object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     *
     * @return  The object.
     */

    virtual const NFGUID& GetObject(const int nRow, const std::string& strColTag) const = 0;

    /**
     * @fn  virtual const Point3D& NFIRecord::GetPoint(const int nRow, const std::string& strColTag) const = 0;
     *
     * @brief   Gets a point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow        The row.
     * @param   strColTag   The col tag.
     *
     * @return  The point.
     */

    virtual const Point3D& GetPoint(const int nRow, const std::string& strColTag) const = 0;

    /**
     * @fn  virtual int NFIRecord::FindRowByColValue(const int nCol, const NFIDataList& var, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first row by col value.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           nCol        The col.
     * @param           var         The variable.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found row by col value.
     */

    virtual int FindRowByColValue(const int nCol, const NFIDataList& var, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::FindInt(const int nCol, const NFINT64 value, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           nCol        The col.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found int.
     */

    virtual int FindInt(const int nCol, const NFINT64 value, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::FindDouble(const int nCol, const double value, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           nCol        The col.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found double.
     */

    virtual int FindDouble(const int nCol, const double value, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::FindString(const int nCol, const std::string& value, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           nCol        The col.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found string.
     */

    virtual int FindString(const int nCol, const std::string& value, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::FindObject(const int nCol, const NFGUID& value, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           nCol        The col.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found object.
     */

    virtual int FindObject(const int nCol, const NFGUID& value, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::FindPoint(const int nCol, const Point3D& value, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           nCol        The col.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found point.
     */

    virtual int FindPoint(const int nCol, const Point3D& value, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::FindRowByColValue(const std::string& strColTag, const NFIDataList& var, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first row by col value.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           strColTag   The col tag.
     * @param           var         The variable.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found row by col value.
     */

    virtual int FindRowByColValue(const std::string& strColTag, const NFIDataList& var, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::FindInt(const std::string& strColTag, const NFINT64 value, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           strColTag   The col tag.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found int.
     */

    virtual int FindInt(const std::string& strColTag, const NFINT64 value, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::FindDouble(const std::string& strColTag, const double value, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           strColTag   The col tag.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found double.
     */

    virtual int FindDouble(const std::string& strColTag, const double value, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::FindString(const std::string& strColTag, const std::string& value, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           strColTag   The col tag.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found string.
     */

    virtual int FindString(const std::string& strColTag, const std::string& value, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::FindObject(const std::string& strColTag, const NFGUID& value, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           strColTag   The col tag.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found object.
     */

    virtual int FindObject(const std::string& strColTag, const NFGUID& value, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::FindPoint(const std::string& strColTag, const Point3D& value, NFIDataList& varResult) = 0;
     *
     * @brief   Searches for the first point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           strColTag   The col tag.
     * @param           value       The value.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The found point.
     */

    virtual int FindPoint(const std::string& strColTag, const Point3D& value, NFIDataList& varResult) = 0;

    /**
     * @fn  virtual int NFIRecord::SortByCol(const int nCol, const bool bOrder, NFIDataList& varResult)
     *
     * @brief   Sort by col.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           nCol        The col.
     * @param           bOrder      The order.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The sorted by col.
     */

    virtual int SortByCol(const int nCol, const bool bOrder, NFIDataList& varResult)
    {
        return 0;
    };

    /**
     * @fn  virtual int NFIRecord::SortByTag(const std::string& strColTag, const bool bOrder, NFIDataList& varResult)
     *
     * @brief   Sort by tag.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           strColTag   The col tag.
     * @param           bOrder      The order.
     * @param [in,out]  varResult   The variable result.
     *
     * @return  The sorted by tag.
     */

    virtual int SortByTag(const std::string& strColTag, const bool bOrder,  NFIDataList& varResult)
    {
        return 0;
    };

    /**
     * @fn  virtual bool NFIRecord::QueryRow(const int nRow, NFIDataList& varList) = 0;
     *
     * @brief   获得数据.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           nRow    The row.
     * @param [in,out]  varList List of variables.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool QueryRow(const int nRow, NFIDataList& varList) = 0;

    /**
     * @fn  virtual bool NFIRecord::SwapRowInfo(const int nOriginRow, const int nTargetRow) = 0;
     *
     * @brief   Swap row information.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nOriginRow  The origin row.
     * @param   nTargetRow  Target row.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SwapRowInfo(const int nOriginRow, const int nTargetRow) = 0;

    /**
     * @fn  virtual bool NFIRecord::Remove(const int nRow) = 0;
     *
     * @brief   Removes the given nRow.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nRow    The row to remove.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Remove(const int nRow) = 0;

    /**
     * @fn  virtual bool NFIRecord::Remove(NFIDataList& varRows)
     *
     * @brief   Removes the given varRows.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param [in,out]  varRows The Variable rows to remove.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Remove(NFIDataList& varRows) //need to optimize
    {
        for(int i  = 0; i < varRows.GetCount(); ++i)
        {
            Remove((int)varRows.Int(i));
        }

        return true;
    }

    /**
     * @fn  virtual bool NFIRecord::Clear() = 0;
     *
     * @brief   Clears this object to its blank/initial state.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Clear() = 0;

    /**
     * @fn  virtual void NFIRecord::AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;
     *
     * @brief   Adds a record hook.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   cb  The cb.
     */

    virtual void AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;

    /**
     * @fn  virtual const bool NFIRecord::GetSave() = 0;
     *
     * @brief   Gets the save.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The save.
     */

    virtual const bool GetSave() = 0;

    /**
     * @fn  virtual const bool NFIRecord::GetPublic() = 0;
     *
     * @brief   Gets the public.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The public.
     */

    virtual const bool GetPublic() = 0;

    /**
     * @fn  virtual const bool NFIRecord::GetPrivate() = 0;
     *
     * @brief   Gets the private.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The private.
     */

    virtual const bool GetPrivate() = 0;

    /**
     * @fn  virtual const bool NFIRecord::GetCache() = 0;
     *
     * @brief   Gets the cache.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The cache.
     */

    virtual const bool GetCache() = 0;

    /**
     * @fn  virtual const std::string& NFIRecord::GetName() const = 0;
     *
     * @brief   Gets the name.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The name.
     */

    virtual const std::string& GetName() const = 0;

    /**
     * @fn  virtual const NF_SHARE_PTR<NFIDataList> NFIRecord::GetInitData() const = 0;
     *
     * @brief   Gets init data.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The init data.
     */

    virtual const NF_SHARE_PTR<NFIDataList> GetInitData() const = 0;

    /**
     * @fn  virtual const NF_SHARE_PTR<NFIDataList> NFIRecord::GetTag() const = 0;
     *
     * @brief   Gets the tag.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The tag.
     */

    virtual const NF_SHARE_PTR<NFIDataList> GetTag() const = 0;

    /**
     * @fn  virtual void NFIRecord::SetSave(const bool bSave) = 0;
     *
     * @brief   Sets a save.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   bSave   The save.
     */

    virtual void SetSave(const bool bSave) = 0;

    /**
     * @fn  virtual void NFIRecord::SetCache(const bool bCache) = 0;
     *
     * @brief   Sets a cache.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   bCache  The cache.
     */

    virtual void SetCache(const bool bCache) = 0;

    /**
     * @fn  virtual void NFIRecord::SetPublic(const bool bPublic) = 0;
     *
     * @brief   Sets a public.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   bPublic The public.
     */

    virtual void SetPublic(const bool bPublic) = 0;

    /**
     * @fn  virtual void NFIRecord::SetPrivate(const bool bPrivate) = 0;
     *
     * @brief   Sets a private.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   bPrivate    The private.
     */

    virtual void SetPrivate(const bool bPrivate) = 0;

    /**
     * @fn  virtual void NFIRecord::SetName(const std::string& strName) = 0;
     *
     * @brief   Sets a name.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strName The name.
     */

    virtual void SetName(const std::string& strName) = 0;

    /**
     * @fn  virtual const TRECORDVEC& NFIRecord::GetRecordVec() const = 0;
     *
     * @brief   Gets record vector.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The record vector.
     */

    virtual const TRECORDVEC& GetRecordVec() const = 0;
};

#endif
