// -------------------------------------------------------------------------
//    @FileName         :    AFRecord.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    AFRecord
//
// -------------------------------------------------------------------------

#pragma once

#include "AFCDataList.h"
#include "AFArrayPod.hpp"
#include "AFCoreDef.hpp"
#include "AFString.hpp"
#include "AFIData.h"

using namespace ArkFrame;

class AFRecord
{
private:
    using row_data_t = AFIData;

public:
    AFRecord();
    virtual ~AFRecord();

    const char* GetName() const;

    virtual int GetCols() const;

    virtual int GetRows() const;

    virtual int GetRowMax() const;

    virtual int GetColType(int col) const;

    virtual bool AddRow(const int row);
    virtual bool AddRow(const int row, const AFIDataList& data);

    virtual bool SetInt(const int nRow, const int nCol, const NFINT64 value);
    virtual bool SetDouble(const int nRow, const int nCol, const double value);
    virtual bool SetString(const int nRow, const int nCol, const std::string& value);
    virtual bool SetObject(const int nRow, const int nCol, const AFGUID& value);

    virtual NFINT64 GetInt(const int nRow, const int nCol) const;
    virtual double GetDouble(const int nRow, const int nCol) const;
    virtual const std::string& GetString(const int nRow, const int nCol) const;
    virtual const AFGUID& GetObject(const int nRow, const int nCol) const;

    virtual int FindInt(const int nCol, const NFINT64 value, AFIDataList& varResult);
    virtual int FindDouble(const int nCol, const double value, AFIDataList& varResult);
    virtual int FindString(const int nCol, const std::string& value, AFIDataList& varResult);
    virtual int FindObject(const int nCol, const AFGUID& value, AFIDataList& varResult);

    virtual bool QueryRow(const int nRow, AFIDataList& varList);

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