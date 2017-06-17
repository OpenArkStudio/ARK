// -------------------------------------------------------------------------
//    @FileName         :    NFCRecord.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    NFCRecord
//
// -------------------------------------------------------------------------

#ifndef _GT_RECORD_H
#define _GT_RECORD_H

#include <vector>
#include "NFIRecord.h"
#include "AFCDataList.h"
#include "NFMapEx.h"

class NFCRecord : public NFIRecord
{
public:
    NFCRecord();
    NFCRecord(const AFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<AFIDataList>& valueList, const NF_SHARE_PTR<AFIDataList>& tagList, const int nMaxRow);
    virtual ~NFCRecord();

    virtual bool IsUsed(const int nRow) const;
    virtual bool SetUsed(const int nRow, const int bUse);
    virtual int GetCols() const;
    virtual int GetRows() const;

    virtual int GetColType(const int nCol) const;
    virtual const std::string& GetColTag(const int nCol) const;

    virtual int AddRow(const int nRow);
    virtual int AddRow(const int nRow, const AFIDataList& var);

    virtual bool SetInt(const int nRow, const int nCol, const NFINT32 value);
    virtual bool SetDouble(const int nRow, const int nCol, const double value);
    virtual bool SetString(const int nRow, const int nCol, const std::string& value);
    virtual bool SetObject(const int nRow, const int nCol, const AFGUID& value);

    virtual bool SetInt(const int nRow, const std::string& strColTag, const NFINT32 value);
    virtual bool SetDouble(const int nRow, const std::string& strColTag, const double value);
    virtual bool SetString(const int nRow, const std::string& strColTag, const std::string& value);
    virtual bool SetObject(const int nRow, const std::string& strColTag, const AFGUID& value);

    virtual NFINT32 GetInt(const int nRow, const int nCol) const;
    virtual double GetDouble(const int nRow, const int nCol) const;
    virtual const std::string& GetString(const int nRow, const int nCol) const;
    virtual const AFGUID& GetObject(const int nRow, const int nCol) const;

    virtual NFINT32 GetInt(const int nRow, const std::string& strColTag) const;
    virtual double GetDouble(const int nRow, const std::string& strColTag) const;
    virtual const std::string& GetString(const int nRow, const std::string& strColTag) const;
    virtual const AFGUID& GetObject(const int nRow, const std::string& strColTag) const;

    virtual int FindRowByColValue(const int nCol, const AFIDataList& var, AFIDataList& varResult);

    virtual int FindInt(const int nCol, const NFINT32 value, AFIDataList& varResult);
    virtual int FindDouble(const int nCol, const double value, AFIDataList& varResult);
    virtual int FindString(const int nCol, const std::string& value, AFIDataList& varResult);
    virtual int FindObject(const int nCol, const AFGUID& value, AFIDataList& varResult);

    virtual int FindRowByColValue(const std::string& strColTag, const AFIDataList& var, AFIDataList& varResult);
    virtual int FindInt(const std::string& strColTag, const NFINT32 value, AFIDataList& varResult);
    virtual int FindDouble(const std::string& strColTag, const double value, AFIDataList& varResult);
    virtual int FindString(const std::string& strColTag, const std::string& value, AFIDataList& varResult);
    virtual int FindObject(const std::string& strColTag, const AFGUID& value, AFIDataList& varResult);

    virtual bool QueryRow(const int nRow, AFIDataList& varList);

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
     * @fn  virtual const NF_SHARE_PTR<AFDataList> NFCRecord::GetInitData() const;
     *
     * @brief   Gets init data.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The init data.
     */

    virtual const NF_SHARE_PTR<AFIDataList> GetInitData() const;

    /**
     * @fn  virtual const NF_SHARE_PTR<AFDataList> NFCRecord::GetTag() const;
     *
     * @brief   Gets the tag.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The tag.
     */

    virtual const NF_SHARE_PTR<AFIDataList> GetTag() const;

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
     * @fn  bool NFCRecord::ValidCheck(TDATA_TYPE eType, const AFDataList::TData& var, NF_SHARE_PTR<AFDataList::TData>& pVar);
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

    bool ValidCheck(AF_DATA_TYPE eType, const AFIData& var, NF_SHARE_PTR<AFIData>& pVar);

    /**
     * @fn  void NFCRecord::OnEventHandler(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFDataList::TData& oldVar, const AFDataList::TData& newVar);
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

    void OnEventHandler(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar);

protected:
    //记录这个表的Key类型，那样在读取和设置的时候才能保持正确
    /** @brief   初始值类型--应该引用静态的(或者智能指针)，节约大量内存. */
    NF_SHARE_PTR<AFIDataList> mVarRecordType;
    /** @brief   col的tag值--应该引用静态的(或者智能指针)，节约大量内存. */
    NF_SHARE_PTR<AFIDataList> mVarRecordTag;

    /** @brief   tag->col转换. */
    std::map<std::string, int> mmTag;

    /** @brief   /////////////////////////. */

    TRECORDVEC mtRecordVec;
    /** @brief   State of the vector used. */
    std::vector<int> mVecUsedState;
    /** @brief   The mn maximum row. */
    int mnMaxRow;

    /** @brief   The self. */
    AFGUID mSelf;
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
