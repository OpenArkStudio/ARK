// -------------------------------------------------------------------------
//    @FileName         :    AFCRecord.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    AFCRecord
//
// -------------------------------------------------------------------------

#include <exception>
#include "AFCDataList.h"
#include "AFCRecord.h"

AFCRecord::AFCRecord()
{
    mSelf = NULL_GUID;

    mbSave = false;
    mbPublic = false;
    mbPrivate = false;
    mbCache = false;

    mstrRecordName = "";
    mnMaxRow = 0;

}

AFCRecord::AFCRecord(const AFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<AFIDataList>& valueList, const NF_SHARE_PTR<AFIDataList>& tagList, const int nMaxRow)
{
    mVarRecordType = valueList;
    mVarRecordTag = tagList;

    mSelf = self;

    mbSave = false;
    mbPublic = false;
    mbPrivate = false;
    mbCache = false;

    mstrRecordName = strRecordName;

    mnMaxRow = nMaxRow;
    //确定大小
    mVecUsedState.resize(mnMaxRow);

    for(int i = 0; i < mnMaxRow; i++)
    {
        mVecUsedState[i] = 0;
    }

    //init share_pointer for all data
    for(int i = 0; i < GetRows() * GetCols(); i++)
    {
        mtRecordVec.push_back(NF_SHARE_PTR<AFIData>());
    }

    //optimize would be better, it should be applied memory space only once
    for(int i = 0; i < mVarRecordTag->GetCount(); ++i)
    {
        if(mVarRecordTag->String(i) == NULL_STR.c_str())
        {
            mmTag[mVarRecordTag->String(i)] = i;
        }
    }
}

AFCRecord::~AFCRecord()
{
    for(TRECORDVEC::iterator iter = mtRecordVec.begin(); iter != mtRecordVec.end(); ++iter)
    {
        iter->reset();
    }

    for(TRECORDCALLBACKEX::iterator iter = mtRecordCallback.begin(); iter != mtRecordCallback.end(); ++iter)
    {
        iter->reset();
    }

    mtRecordVec.clear();
    mVecUsedState.clear();
    mtRecordCallback.clear();
}

int AFCRecord::GetCols() const
{
    return mVarRecordType->GetCount();
}

int AFCRecord::GetRows() const
{
    return mnMaxRow;
}

int AFCRecord::GetColType(const int nCol) const
{
    return mVarRecordType->GetType(nCol);
}

const std::string& AFCRecord::GetColTag(const int nCol) const
{
    return mVarRecordTag->String(nCol);
}

// 添加数据
int AFCRecord::AddRow(const int nRow)
{
    return AddRow(nRow, *mVarRecordType);
}

int AFCRecord::AddRow(const int nRow, const AFIDataList& var)
{
    bool bCover = false;
    int nFindRow = nRow;
    if(nFindRow >= mnMaxRow)
    {
        return -1;
    }

    if(var.GetCount() != GetCols())
    {
        return -1;
    }

    if(nFindRow < 0)
    {
        for(int i = 0; i < mnMaxRow; i++)
        {
            if(!IsUsed(i))
            {
                nFindRow = i;
                break;
            }
        }
    }
    else
    {
        if(IsUsed(nFindRow))
        {
            bCover = true;
        }
    }

    if(nFindRow < 0)
    {
        return -1;
    }

    for(int i = 0; i < GetCols(); ++i)
    {
        if(var.GetType(i) != GetColType(i))
        {
            return -1;
        }
    }

    SetUsed(nFindRow, 1);

    for(int i = 0; i < GetCols(); ++i)
    {
        NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nFindRow, i));
        if(nullptr == pVar)
        {
            pVar = NF_SHARE_PTR<AFIData>(NF_NEW AFXData());
        }
        //一个个放入数值和类型
        switch (var.GetType(i))
        {
        case DT_BOOLEAN:
            pVar->SetBool(var.Bool(i));
            break;
            //TODO:
        default:
            return -1;
            break;
        }
    }

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = bCover ? Cover : Add;
    xEventData.nRow = nFindRow;
    xEventData.nCol = 0;
    xEventData.strRecordName = mstrRecordName;

    AFXData tData;
    OnEventHandler(mSelf, xEventData, tData, tData); //FIXME:RECORD

    return nFindRow;
}

bool AFCRecord::SetBool(const int nRow, const int nCol, const bool value)
{
    if (!ValidPos(nRow, nCol))
    {
        return false;
    }

    if (DT_BOOLEAN != GetColType(nCol))
    {
        return false;
    }

    if (!IsUsed(nRow))
    {
        return false;
    }

    AFXData var;
    var.SetBool(value);

    NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if (!ValidCheck(DT_BOOLEAN, var, pVar))
    {
        return false;
    }

    AFXData oldValue;
    oldValue.SetBool(pVar->GetBool());

    pVar->SetBool(value);

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool AFCRecord::SetBool(const int nRow, const std::string& strColTag, const bool value)
{
    int nCol = GetCol(strColTag);
    return SetBool(nRow, nCol, value);
}

bool AFCRecord::SetInt(const int nRow, const int nCol, const int32_t value)
{
    if(!ValidPos(nRow, nCol))
    {
        return false;
    }

    if(DT_INT != GetColType(nCol))
    {
        return false;
    }

    if(!IsUsed(nRow))
    {
        return false;
    }

    AFXData var;
    var.SetInt(value);

    NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(!ValidCheck(DT_INT, var, pVar))
    {
        return false;
    }

    AFXData oldValue;
    oldValue.SetInt(pVar->GetInt());

    pVar->SetInt(value);

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool AFCRecord::SetInt(const int nRow, const std::string& strColTag, const int32_t value)
{
    int nCol = GetCol(strColTag);
    return SetInt(nRow, nCol, value);
}

bool AFCRecord::SetInt64(const int nRow, const int nCol, const int64_t value)
{
    if (!ValidPos(nRow, nCol))
    {
        return false;
    }

    if (DT_INT64 != GetColType(nCol))
    {
        return false;
    }

    if (!IsUsed(nRow))
    {
        return false;
    }

    AFXData var;
    var.SetInt64(value);

    NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if (!ValidCheck(DT_INT64, var, pVar))
    {
        return false;
    }

    AFXData oldValue;
    oldValue.SetInt64(pVar->GetInt64());

    pVar->SetInt64(value);

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool AFCRecord::SetInt64(const int nRow, const std::string& strColTag, const int64_t value)
{
    int nCol = GetCol(strColTag);
    return SetInt64(nRow, nCol, value);
}

bool AFCRecord::SetFloat(const int nRow, const int nCol, const float value)
{
    if (!ValidPos(nRow, nCol))
    {
        return false;
    }

    if (DT_FLOAT != GetColType(nCol))
    {
        return false;
    }

    if (!IsUsed(nRow))
    {
        return false;
    }

    AFXData var;
    var.SetFloat(value);

    NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if (!ValidCheck(DT_FLOAT, var, pVar))
    {
        return false;
    }

    AFXData oldValue;
    oldValue.SetFloat(pVar->GetFloat());

    pVar->SetFloat(value);

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool AFCRecord::SetFloat(const int nRow, const std::string& strColTag, const float value)
{
    int nCol = GetCol(strColTag);
    return SetFloat(nRow, nCol, value);
}

bool AFCRecord::SetDouble(const int nRow, const int nCol, const double value)
{
    if(!ValidPos(nRow, nCol))
    {
        return false;
    }

    if(DT_DOUBLE != GetColType(nCol))
    {
        return false;
    }

    if(!IsUsed(nRow))
    {
        return false;
    }

    AFXData var;
    var.SetDouble(value);

    NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(!ValidCheck(DT_DOUBLE, var, pVar))
    {
        return false;
    }

    AFXData oldValue;
    oldValue.SetDouble(pVar->GetDouble());

    pVar->SetDouble(value);

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool AFCRecord::SetDouble(const int nRow, const std::string& strColTag, const double value)
{
    int nCol = GetCol(strColTag);
    return SetDouble(nRow, nCol, value);
}

bool AFCRecord::SetString(const int nRow, const int nCol, const std::string& value)
{
    if(!ValidPos(nRow, nCol))
    {
        return false;
    }

    if(DT_STRING != GetColType(nCol))
    {
        return false;
    }

    if(!IsUsed(nRow))
    {
        return false;
    }

    AFXData var;
    var.SetString(value.c_str());

    NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(!ValidCheck(DT_STRING, var, pVar))
    {
        return false;
    }

    AFXData oldValue;
    oldValue.SetString(pVar->GetString());

    pVar->SetString(value.c_str());

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool AFCRecord::SetString(const int nRow, const std::string& strColTag, const std::string& value)
{
    int nCol = GetCol(strColTag);
    return SetString(nRow, nCol, value);
}

bool AFCRecord::SetObject(const int nRow, const int nCol, const AFGUID& value)
{
    if(!ValidPos(nRow, nCol))
    {
        return false;
    }

    if(DT_OBJECT != GetColType(nCol))
    {
        return false;
    }

    if(!IsUsed(nRow))
    {
        return false;
    }

    AFXData var;
    var.SetObject(value);

    NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(!ValidCheck(DT_OBJECT, var, pVar))
    {
        return false;
    }

    AFXData oldValue;
    oldValue.SetObject(pVar->GetObject());

    pVar->SetObject(value);

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool AFCRecord::SetObject(const int nRow, const std::string& strColTag, const AFGUID& value)
{
    int nCol = GetCol(strColTag);
    return SetObject(nRow, nCol, value);
}

//bool AFCRecord::SetPoint(const int nRow, const int nCol, const Point3D& value)
//{
//    if(!ValidPos(nRow, nCol))
//    {
//        return false;
//    }
//
//    if(TDATA_POINT != GetColType(nCol))
//    {
//        return false;
//    }
//
//    if(!IsUsed(nRow))
//    {
//        return false;
//    }
//
//    AFIDataList::TData var;
//    var.SetPoint(value);
//
//    NF_SHARE_PTR<AFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
//    if(!ValidCheck(TDATA_POINT, var, pVar))
//    {
//        return false;
//    }
//
//    AFIDataList::TData oldValue;
//    oldValue.SetPoint(pVar->GetPoint());
//
//    pVar->variantData = value;
//
//    RECORD_EVENT_DATA xEventData;
//    xEventData.nOpType = Update;
//    xEventData.nRow = nRow;
//    xEventData.nCol = nCol;
//    xEventData.strRecordName = mstrRecordName;
//
//    OnEventHandler(mSelf, xEventData, oldValue, *pVar);
//
//    return true;
//}
//
//bool AFCRecord::SetPoint(const int nRow, const std::string& strColTag, const Point3D& value)
//{
//    int nCol = GetCol(strColTag);
//    return SetPoint(nRow, nCol, value);
//}

// 获得数据
bool AFCRecord::QueryRow(const int nRow, AFIDataList& varList)
{
    if(!ValidRow(nRow))
    {
        return false;
    }

    if(!IsUsed(nRow))
    {
        return false;
    }

    varList.Clear();
    for(int i = 0; i < GetCols(); ++i)
    {
        NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, i));
        if(nullptr != pVar)
        {
            varList.Append(*pVar);
        }
        else
        {
            switch(GetColType(i))
            {
            case DT_BOOLEAN:
                varList.AddBool(NULL_BOOLEAN);
                break;
            case DT_INT:
                varList.AddInt(NULL_INT);
                break;
            case DT_INT64:
                varList.AddInt(NULL_INT64);
                break;
            case DT_FLOAT:
                varList.AddFloat(NULL_FLOAT);
                break;
            case DT_DOUBLE:
                varList.AddDouble(NULL_DOUBLE);
                break;
            case DT_STRING:
                varList.AddString(NULL_STR.c_str());
                break;
            case DT_OBJECT:
                varList.AddObject(NULL_GUID);
                break;
            case DT_POINTER:
                varList.AddPointer(NULL);
                break;
            default:
                return false;
                break;
            }
        }
    }

    if(varList.GetCount() != GetCols())
    {
        return false;
    }

    return true;
}

bool AFCRecord::GetBool(const int nRow, const int nCol) const
{
    if (!ValidPos(nRow, nCol))
    {
        return NULL_BOOLEAN;
    }

    if (!IsUsed(nRow))
    {
        return NULL_BOOLEAN;
    }

    const NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if (nullptr == pVar)
    {
        return NULL_BOOLEAN;
    }

    return pVar->GetBool();
}

bool AFCRecord::GetBool(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetBool(nRow, nCol);
}

int32_t AFCRecord::GetInt(const int nRow, const int nCol) const
{
    if(!ValidPos(nRow, nCol))
    {
        return NULL_INT;
    }

    if(!IsUsed(nRow))
    {
        return NULL_INT;
    }

    const NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(nullptr == pVar)
    {
        return NULL_INT;
    }

    return pVar->GetInt();
}

int32_t AFCRecord::GetInt(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetInt(nRow, nCol);
}

int64_t AFCRecord::GetInt64(const int nRow, const int nCol) const
{
    if (!ValidPos(nRow, nCol))
    {
        return NULL_INT64;
    }

    if (!IsUsed(nRow))
    {
        return NULL_INT64;
    }

    const NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if (nullptr == pVar)
    {
        return NULL_INT64;
    }

    return pVar->GetInt64();
}

int64_t AFCRecord::GetInt64(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetInt64(nRow, nCol);
}

float AFCRecord::GetFloat(const int nRow, const int nCol) const
{
    if (!ValidPos(nRow, nCol))
    {
        return NULL_FLOAT;
    }

    if (!IsUsed(nRow))
    {
        return NULL_FLOAT;
    }

    const NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if (nullptr == pVar)
    {
        return NULL_FLOAT;
    }

    return pVar->GetFloat();
}

float AFCRecord::GetFloat(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetFloat(nRow, nCol);
}

double AFCRecord::GetDouble(const int nRow, const int nCol) const
{
    if(!ValidPos(nRow, nCol))
    {
        return NULL_DOUBLE;
    }

    if(!IsUsed(nRow))
    {
        return NULL_DOUBLE;
    }

    const NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(nullptr == pVar)
    {
        return NULL_DOUBLE;
    }

    return pVar->GetDouble();
}

double AFCRecord::GetDouble(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetDouble(nRow, nCol);
}

const std::string& AFCRecord::GetString(const int nRow, const int nCol) const
{
    if(!ValidPos(nRow, nCol))
    {
        return NULL_STR;
    }

    if(!IsUsed(nRow))
    {
        return NULL_STR;
    }

    const NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(nullptr == pVar)
    {
        return NULL_STR;
    }

    return pVar->GetString();
}

const std::string& AFCRecord::GetString(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetString(nRow, nCol);
}

const AFGUID& AFCRecord::GetObject(const int nRow, const int nCol) const
{
    if(!ValidPos(nRow, nCol))
    {
        return NULL_GUID;
    }

    if(!IsUsed(nRow))
    {
        return NULL_GUID;
    }

    const  NF_SHARE_PTR<AFIData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(nullptr == pVar)
    {
        return NULL_GUID;
    }

    return pVar->GetObject();
}

const AFGUID& AFCRecord::GetObject(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetObject(nRow, nCol);
}

int AFCRecord::FindRowByColValue(const int nCol, const AFIDataList& var, AFIDataList& varResult)
{
    if(!ValidCol(nCol))
    {
        return 0;
    }

    int eType = var.GetType(0);
    if(eType != mVarRecordType->GetType(nCol))
    {
        return 0;
    }

    switch(eType)
    {
    case DT_BOOLEAN:
        return FindBool(nCol, var.Bool(nCol), varResult);
        break;
    case DT_INT:
        return FindInt(nCol, var.Int(nCol), varResult);
        break;
    case DT_INT64:
        return FindInt64(nCol, var.Int64(nCol), varResult);
        break;
    case DT_FLOAT:
        return FindFloat(nCol, var.Float(nCol), varResult);
        break;
    case DT_DOUBLE:
        return FindDouble(nCol, var.Double(nCol), varResult);
        break;
    case DT_STRING:
        return FindString(nCol, var.String(nCol), varResult);
        break;
    case DT_OBJECT:
        return FindObject(nCol, var.Object(nCol), varResult);
        break;
    default:
        break;
    }

    return 0;
}

int AFCRecord::FindRowByColValue(const std::string& strColTag, const AFIDataList& var, AFIDataList& varResult)
{
    int nCol = GetCol(strColTag);
    return FindRowByColValue(nCol, var, varResult);
}

int AFCRecord::FindBool(const int nCol, const bool value, AFIDataList& varResult)
{
    if (!ValidCol(nCol))
    {
        return 0;
    }

    if (AF_DATA_TYPE::DT_BOOLEAN != mVarRecordType->GetType(nCol))
    {
        return 0;
    }

    for (int i = 0; i < mnMaxRow; ++i)
    {
        if (!IsUsed(i))
        {
            continue;
        }

        if (GetBool(i, nCol) == value)
        {
            varResult << i;
        }
    }

    return varResult.GetCount();
}

int AFCRecord::FindBool(const std::string& strColTag, const bool value, AFIDataList& varResult)
{
    if (strColTag.empty())
    {
        return 0;
    }

    int nCol = GetCol(strColTag);
    return FindBool(nCol, value, varResult);
}

int AFCRecord::FindInt(const int nCol, const int32_t value, AFIDataList& varResult)
{
    if(!ValidCol(nCol))
    {
        return 0;
    }

    if(AF_DATA_TYPE::DT_INT != mVarRecordType->GetType(nCol))
    {
        return 0;
    }

    for (int i = 0; i < mnMaxRow; ++i)
    {
        if (!IsUsed(i))
        {
            continue;
        }

        if (GetInt(i, nCol) == value)
        {
            varResult << i;
        }
    }

    return varResult.GetCount();
}

int AFCRecord::FindInt(const std::string& strColTag, const int32_t value, AFIDataList& varResult)
{
    if(strColTag.empty())
    {
        return 0;
    }

    int nCol = GetCol(strColTag);
    return FindInt(nCol, value, varResult);
}

int AFCRecord::FindInt64(const int nCol, const int64_t value, AFIDataList& varResult)
{
    if (!ValidCol(nCol))
    {
        return 0;
    }

    if (AF_DATA_TYPE::DT_INT64 != mVarRecordType->GetType(nCol))
    {
        return 0;
    }

    for (int i = 0; i < mnMaxRow; ++i)
    {
        if (!IsUsed(i))
        {
            continue;
        }

        if (GetInt64(i, nCol) == value)
        {
            varResult << i;
        }
    }

    return varResult.GetCount();
}

int AFCRecord::FindInt64(const std::string& strColTag, const int64_t value, AFIDataList& varResult)
{
    if (strColTag.empty())
    {
        return 0;
    }

    int nCol = GetCol(strColTag);
    return FindInt64(nCol, value, varResult);
}

int AFCRecord::FindFloat(const int nCol, const float value, AFIDataList& varResult)
{
    if (!ValidCol(nCol))
    {
        return 0;
    }

    if (AF_DATA_TYPE::DT_FLOAT != mVarRecordType->GetType(nCol))
    {
        return 0;
    }

    for (int i = 0; i < mnMaxRow; ++i)
    {
        if (!IsUsed(i))
        {
            continue;
        }

        if (GetFloat(i, nCol) == value)
        {
            varResult << i;
        }
    }

    return varResult.GetCount();
}

int AFCRecord::FindFloat(const std::string& strColTag, const float value, AFIDataList& varResult)
{
    if (strColTag.empty())
    {
        return 0;
    }

    int nCol = GetCol(strColTag);
    return FindFloat(nCol, value, varResult);
}

int AFCRecord::FindDouble(const int nCol, const double value, AFIDataList& varResult)
{
    if(!ValidCol(nCol))
    {
        return 0;
    }

    if(AF_DATA_TYPE::DT_DOUBLE != mVarRecordType->GetType(nCol))
    {
        return 0;
    }

    for(int i = 0; i < mnMaxRow; ++i)
    {
        if(!IsUsed(i))
        {
            continue;
        }

        if(GetDouble(i, nCol) == value)
        {
            varResult << i;
        }
    }

    return varResult.GetCount();
}

int AFCRecord::FindDouble(const std::string& strColTag, const double value, AFIDataList& varResult)
{
    if(strColTag.empty())
    {
        return 0;
    }

    int nCol = GetCol(strColTag);
    return FindDouble(nCol, value, varResult);
}

int AFCRecord::FindString(const int nCol, const std::string& value, AFIDataList& varResult)
{
    if(!ValidCol(nCol))
    {
        return 0;
    }

    if(AF_DATA_TYPE::DT_STRING != mVarRecordType->GetType(nCol))
    {
        return 0;
    }

    for(int64_t i = 0; i < mnMaxRow; ++i)
    {
        if(!IsUsed(i))
        {
            continue;
        }

        const std::string& strData = GetString(i, nCol);
        if(0 == strcmp(strData.c_str(), value.c_str()))
        {
            varResult << i;
        }
    }

    return varResult.GetCount();
}

int AFCRecord::FindString(const std::string& strColTag, const std::string& value, AFIDataList& varResult)
{
    if(strColTag.empty())
    {
        return 0;
    }

    int nCol = GetCol(strColTag);
    return FindString(nCol, value, varResult);
}

int AFCRecord::FindObject(const int nCol, const AFGUID& value, AFIDataList& varResult)
{
    if(!ValidCol(nCol))
    {
        return 0;
    }

    if(AF_DATA_TYPE::DT_OBJECT != mVarRecordType->GetType(nCol))
    {
        return 0;
    }

    for(int64_t i = 0; i < mnMaxRow; ++i)
    {
        if(!IsUsed(i))
        {
            continue;
        }

        if(GetObject(i, nCol) == value)
        {
            varResult << i;
        }
    }

    return varResult.GetCount();
}

int AFCRecord::FindObject(const std::string& strColTag, const AFGUID& value, AFIDataList& varResult)
{
    if(strColTag.empty())
    {
        return 0;
    }

    int nCol = GetCol(strColTag);
    return FindObject(nCol, value, varResult);
}

bool AFCRecord::Remove(const int nRow)
{
    if(ValidRow(nRow))
    {
        if(IsUsed(nRow))
        {
            RECORD_EVENT_DATA xEventData;
            xEventData.nOpType = Del;
            xEventData.nRow = nRow;
            xEventData.nCol = 0;
            xEventData.strRecordName = mstrRecordName;

            OnEventHandler(mSelf, xEventData, AFXData(), AFXData());

            mVecUsedState[nRow] = 0;
            return true;
        }
    }

    return false;
}

bool AFCRecord::Clear()
{
    for(int i = GetRows() - 1; i >= 0; i--)
    {
        Remove(i);
    }

    return true;
}

void AFCRecord::AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb)
{
    mtRecordCallback.push_back(cb);
}

const bool AFCRecord::GetSave()
{
    return mbSave;
}

const bool AFCRecord::GetCache()
{
    return mbCache;
}

const bool AFCRecord::GetPublic()
{
    return mbPublic;
}

const bool AFCRecord::GetPrivate()
{
    return mbPrivate;
}

int AFCRecord::GetPos(int nRow, int nCol) const
{
    return nRow * mVarRecordType->GetCount() + nCol;
}

const std::string& AFCRecord::GetName() const
{
    return mstrRecordName;
}

void AFCRecord::SetSave(const bool bSave)
{
    mbSave = bSave;
}

void AFCRecord::SetCache(const bool bCache)
{
    mbCache = bCache;
}

void AFCRecord::SetPublic(const bool bPublic)
{
    mbPublic = bPublic;
}

void AFCRecord::SetPrivate(const bool bPrivate)
{
    mbPrivate = bPrivate;
}

void AFCRecord::SetName(const std::string& strName)
{
    mstrRecordName = strName;
}

const NF_SHARE_PTR<AFIDataList> AFCRecord::GetInitData() const
{
    NF_SHARE_PTR<AFIDataList> pIniData = NF_SHARE_PTR<AFIDataList>(NF_NEW AFXDataList());
    pIniData->Append(*mVarRecordType, 0, mVarRecordType->GetCount());

    return pIniData;
}

void AFCRecord::OnEventHandler(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar)
{
    for(auto iter : mtRecordCallback)
    {
        //AFIDataList参数:所属对象名string，操作类型int，Row,Col, OLD属性值，NEW属性值
        (*iter)(self, xEventData, oldVar, newVar);
    }
}

bool AFCRecord::IsUsed(const int nRow) const
{
    if(ValidRow(nRow))
    {
        return (mVecUsedState[nRow] > 0);
    }

    return false;
}

bool AFCRecord::SwapRowInfo(const int nOriginRow, const int nTargetRow)
{
    if(!IsUsed(nOriginRow))
    {
        return false;
    }

    if(ValidRow(nOriginRow)
            && ValidRow(nTargetRow))
    {
        for(int i = 0; i < GetCols(); ++i)
        {
            NF_SHARE_PTR<AFIData> pOrigin = mtRecordVec.at(GetPos(nOriginRow, i));
            mtRecordVec[GetPos(nOriginRow, i)] = mtRecordVec.at(GetPos(nTargetRow, i));
            mtRecordVec[GetPos(nTargetRow, i)] = pOrigin;
        }

        int nOriginUse = mVecUsedState[nOriginRow];
        mVecUsedState[nOriginRow] = mVecUsedState[nTargetRow];
        mVecUsedState[nTargetRow] = nOriginUse;

        RECORD_EVENT_DATA xEventData;
        xEventData.nOpType = Swap;
        xEventData.nRow = nOriginRow;
        xEventData.nCol = nTargetRow;
        xEventData.strRecordName = mstrRecordName;

        AFXData xData;
        OnEventHandler(mSelf, xEventData, xData, xData);

        return true;
    }

    return false;
}

const NF_SHARE_PTR<AFIDataList> AFCRecord::GetTag() const
{
    NF_SHARE_PTR<AFIDataList> pIniData = NF_SHARE_PTR<AFIDataList>(NF_NEW AFXDataList());
    pIniData->Append(*mVarRecordTag, 0, mVarRecordTag->GetCount());
    return pIniData;
}

const AFIRecord::TRECORDVEC& AFCRecord::GetRecordVec() const
{
    return mtRecordVec;
}

bool AFCRecord::SetUsed(const int nRow, const int bUse)
{
    if(ValidRow(nRow))
    {
        mVecUsedState[nRow] = bUse;
        return true;
    }

    return false;
}

bool AFCRecord::ValidPos(int nRow, int nCol) const
{
    if(ValidCol(nCol)
            && ValidRow(nRow))
    {
        return true;
    }

    return false;
}

bool AFCRecord::ValidRow(int nRow) const
{
    if(nRow >= GetRows() || nRow < 0)
    {
        return false;
    }

    return true;
}

bool AFCRecord::ValidCol(int nCol) const
{
    if(nCol >= GetCols() || nCol < 0)
    {
        return false;
    }

    return true;
}

bool AFCRecord::ValidCheck(AF_DATA_TYPE eType, const AFIData& var, NF_SHARE_PTR<AFIData>& pVar)
{
    if(var.GetType() != eType)
    {
        return false;
    }

    if(nullptr == pVar)
    {
        pVar = NF_SHARE_PTR<AFIData>(NF_NEW AFXData());
        switch(eType)
        {
        case DT_UNKNOWN:
            break;
        case DT_BOOLEAN:
            pVar->SetBool(NULL_BOOLEAN);
            break;
        case DT_INT:
            pVar->SetInt(NULL_INT);
            break;
        case DT_INT64:
            pVar->SetInt64(NULL_INT64);
            break;
        case DT_FLOAT:
            pVar->SetFloat(NULL_FLOAT);
            break;
        case DT_DOUBLE:
            pVar->SetDouble(NULL_DOUBLE);
            break;
        case DT_STRING:
            pVar->SetString(NULL_STR.c_str());
            break;
        case DT_OBJECT:
            pVar->SetObject(NULL_GUID);
            break;
        default:
            break;
        }
    }

    if(pVar->GetType() != eType)
    {
        return false;
    }

    //TODO:如果值没有变化，则直接不用继续设置值了
    //if(pVar->variantData == var.variantData)
    //{
    //    return false;
    //}

    return true;
}

int AFCRecord::GetCol(const std::string& strTag) const
{
    auto it = mmTag.find(strTag);
    if(it != mmTag.end())
    {
        return it->second;
    }

    return -1;
}