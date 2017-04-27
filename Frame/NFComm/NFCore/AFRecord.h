// -------------------------------------------------------------------------
//    @FileName         :    AFRecord.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    AFRecord
//
// -------------------------------------------------------------------------

#pragma once

#include "AFDataList.hpp"
#include "AFArrayPod.hpp"
#include "AFCoreDef.hpp"
#include "AFString.hpp"

class AFRecord
{
private:
    using row_data_t = AFDataList::TData;

public:
    AFRecord();
    ~AFRecord();

    void SetName(const char* value);
    const char* GetName() const;
    
    void SetColCount(size_t value);
    size_t GetColCount() const;

    bool SetColType(size_t index, int type);
    int GetColType() const;

    size_t GetRowCount() const;



//    //////////////////////////////////////////////////////////////////////////
//    bool IsUsed(const int nRow) const;
//    bool SetUsed(const int nRow, const int bUse);
//    int GetCols() const;
//    int GetRows() const;
//    TDATA_TYPE GetColType(const int nCol) const;
//    const std::string& GetColTag(const int nCol) const;
//    int AddRow(const int nRow);
//    int AddRow(const int nRow, const AFDataList& var);
//    bool SetInt(const int nRow, const int nCol, const NFINT64 value);
//    bool SetDouble(const int nRow, const int nCol, const double value);
//    bool SetString(const int nRow, const int nCol, const std::string& value);
//    bool SetObject(const int nRow, const int nCol, const NFGUID& value);
//    bool SetPoint(const int nRow, const int nCol, const Point3D& value);
//    bool SetInt(const int nRow, const std::string& strColTag, const NFINT64 value);
//    bool SetDouble(const int nRow, const std::string& strColTag, const double value);
//    bool SetString(const int nRow, const std::string& strColTag, const std::string& value);
//    bool SetObject(const int nRow, const std::string& strColTag, const NFGUID& value);
//    bool SetPoint(const int nRow, const std::string& strColTag, const Point3D& value);
//    NFINT64 GetInt(const int nRow, const int nCol) const;
//    double GetDouble(const int nRow, const int nCol) const;
//    const std::string& GetString(const int nRow, const int nCol) const;
//    const NFGUID& GetObject(const int nRow, const int nCol) const;
//    const Point3D& GetPoint(const int nRow, const int nCol) const;
//    NFINT64 GetInt(const int nRow, const std::string& strColTag) const;
//    double GetDouble(const int nRow, const std::string& strColTag) const;
//    const std::string& GetString(const int nRow, const std::string& strColTag) const;
//    const NFGUID& GetObject(const int nRow, const std::string& strColTag) const;
//    const Point3D& GetPoint(const int nRow, const std::string& strColTag) const;
//    int FindRowByColValue(const int nCol, const AFDataList& var, AFDataList& varResult);
//    int FindInt(const int nCol, const NFINT64 value, AFDataList& varResult);
//    int FindDouble(const int nCol, const double value, AFDataList& varResult);
//    int FindString(const int nCol, const std::string& value, AFDataList& varResult);
//    int FindObject(const int nCol, const NFGUID& value, AFDataList& varResult);
//    int FindPoint(const int nCol, const Point3D& value, AFDataList& varResult);
//
//    bool QueryRow(const int nRow, AFDataList& varList);
//    bool SwapRowInfo(const int nOriginRow, const int nTargetRow);
//    bool Remove(const int nRow);
//    bool Clear();
//    void AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb);
//    const bool GetSave();
//    const bool GetCache();
//    const bool GetPublic();
//    const bool GetPrivate();
//    const std::string& GetName() const;
//    void SetSave(const bool bSave);
//    void SetCache(const bool bCache);
//    void SetPublic(const bool bPublic);
//    void SetPrivate(const bool bPrivate);
//    void SetName(const std::string& strName);
//
//    const NF_SHARE_PTR<AFDataList> GetInitData() const;
//
//protected:
//
//    int GetPos(int nRow, int nCol) const;
//    int GetCol(const std::string& strTag) const;
//    bool ValidPos(int nRow, int nCol) const;
//    bool ValidRow(int nRow) const;
//    bool ValidCol(int nCol) const;
//    bool ValidCheck(TDATA_TYPE eType, const AFDataList::TData& var, NF_SHARE_PTR<AFDataList::TData>& pVar);
//    void OnEventHandler(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFDataList::TData& oldVar, const AFDataList::TData& newVar);

protected:

    RecordName mstrName;
    ArraryPod<int, 1, CoreAlloc> mxColTypes;
    ArraryPod<row_data_t*, 1, CoreAlloc> mxRowDatas;

    bool mbSave;
    bool mbPublic;
    bool mbPrivate;
    bool mbCache;

    //std::vector<RECORD_EVENT_FUNCTOR_PTR> mtRecordCallback;
};