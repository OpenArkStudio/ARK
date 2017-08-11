// -------------------------------------------------------------------------
//    @FileName         :    AFCRecord.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    AFCRecord
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

bool AFRecord::AddRow(const int row, const AFIDataList& data)
{
    return true;
}

bool AFRecord::SetInt(const int nRow, const int nCol, const int64_t value)
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

int64_t AFRecord::GetInt(const int nRow, const int nCol) const
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

int AFRecord::FindInt(const int nCol, const int64_t value, AFIDataList& varResult)
{
    return 0;
}

int AFRecord::FindDouble(const int nCol, const double value, AFIDataList& varResult)
{
    return 0;
}

int AFRecord::FindString(const int nCol, const std::string& value, AFIDataList& varResult)
{
    return 0;
}

int AFRecord::FindObject(const int nCol, const AFGUID& value, AFIDataList& varResult)
{
    return 0;
}

bool AFRecord::QueryRow(const int nRow, AFIDataList& varList)
{
    return true;
}
