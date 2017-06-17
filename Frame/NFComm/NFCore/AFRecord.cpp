// -------------------------------------------------------------------------
//    @FileName         :    NFCRecord.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    NFCRecord
//
// -------------------------------------------------------------------------

#include "AFRecord.h"

AFRecord::AFRecord()
{

}

AFRecord::~AFRecord()
{
    //Clear();
}

const char* AFRecord::GetName() const
{
    return mstrName.c_str();
}

int AFRecord::GetCols() const
{
    return 0;
}

int AFRecord::GetRows() const
{
    return 0;
}

int AFRecord::GetRowMax() const
{
    return 0;
}

int AFRecord::GetColType(int col) const
{
    return 0;
}

bool AFRecord::AddRow(const int row)
{
    return true;
}

bool AFRecord::AddRow(const int row, const AFDataList& data)
{
    return true;
}

bool AFRecord::SetInt(const int nRow, const int nCol, const NFINT64 value)
{
    return true;
}

bool AFRecord::SetDouble(const int nRow, const int nCol, const double value)
{
    return true;
}

bool AFRecord::SetString(const int nRow, const int nCol, const std::string& value)
{
    return true;
}

bool AFRecord::SetObject(const int nRow, const int nCol, const AFGUID& value)
{
    return true;
}

bool AFRecord::SetPoint(const int nRow, const int nCol, const Point3D& value)
{
    return true;
}

NFINT64 AFRecord::GetInt(const int nRow, const int nCol) const
{
    return NULL_INT;
}

double AFRecord::GetDouble(const int nRow, const int nCol) const
{
    return NULL_DOUBLE;
}

const std::string& AFRecord::GetString(const int nRow, const int nCol) const
{
    return NULL_STR;
}

const AFGUID& AFRecord::GetObject(const int nRow, const int nCol) const
{
    return NULL_GUID;
}

const Point3D& AFRecord::GetPoint(const int nRow, const int nCol) const
{
    return NULL_POINT;
}

int AFRecord::FindInt(const int nCol, const NFINT64 value, AFDataList& varResult)
{
    return 0;
}

int AFRecord::FindDouble(const int nCol, const double value, AFDataList& varResult)
{
    return 0;
}

int AFRecord::FindString(const int nCol, const std::string& value, AFDataList& varResult)
{
    return 0;
}

int AFRecord::FindObject(const int nCol, const AFGUID& value, AFDataList& varResult)
{
    return 0;
}

int AFRecord::FindPoint(const int nCol, const Point3D& value, AFDataList& varResult)
{
    return 0;
}

bool AFRecord::QueryRow(const int nRow, AFDataList& varList)
{
    return true;
}
