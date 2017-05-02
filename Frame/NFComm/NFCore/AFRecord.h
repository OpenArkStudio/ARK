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

    const char* GetName() const;

    virtual int GetCols() const;

    virtual int GetRows() const;

    virtual int GetRowMax() const;

    virtual int GetColType(int col) const;

    virtual bool AddRow(const int row);
    virtual bool AddRow(const int row, const AFDataList& data);

    virtual bool SetInt(const int nRow, const int nCol, const NFINT64 value);
    virtual bool SetDouble(const int nRow, const int nCol, const double value);
    virtual bool SetString(const int nRow, const int nCol, const std::string& value);
    virtual bool SetObject(const int nRow, const int nCol, const NFGUID& value);
    virtual bool SetPoint(const int nRow, const int nCol, const Point3D& value);

    virtual NFINT64 GetInt(const int nRow, const int nCol) const;
    virtual double GetDouble(const int nRow, const int nCol) const;
    virtual const std::string& GetString(const int nRow, const int nCol) const;
    virtual const NFGUID& GetObject(const int nRow, const int nCol) const;
    virtual const Point3D& GetPoint(const int nRow, const int nCol) const;

    virtual int FindInt(const int nCol, const NFINT64 value, AFDataList& varResult);
    virtual int FindDouble(const int nCol, const double value, AFDataList& varResult);
    virtual int FindString(const int nCol, const std::string& value, AFDataList& varResult);
    virtual int FindObject(const int nCol, const NFGUID& value, AFDataList& varResult);
    virtual int FindPoint(const int nCol, const Point3D& value, AFDataList& varResult);

    virtual bool QueryRow(const int nRow, AFDataList& varList);

protected:

    RecordName mstrName;
    ArraryPod<int, 1, CoreAlloc> mxColTypes;
    ArraryPod<row_data_t*, 1, CoreAlloc> mxRowDatas;

    bool mbSave;
    bool mbPublic;
    bool mbPrivate;
    bool mbRealtime;

    //std::vector<RECORD_EVENT_FUNCTOR_PTR> mtRecordCallback;
};