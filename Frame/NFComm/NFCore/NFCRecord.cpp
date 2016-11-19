// -------------------------------------------------------------------------
//    @FileName         :    NFCRecord.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFCRecord
//
// -------------------------------------------------------------------------

#include <exception>
#include "NFIDataList.h"
#include "NFCRecord.h"

NFCRecord::NFCRecord()
{
    mSelf = NULL_GUID;

    mbSave = false;
    mbPublic = false;
    mbPrivate = false;
    mbCache = false;

    mstrRecordName = "";
    mnMaxRow = 0;

}

NFCRecord::NFCRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<NFIDataList>& valueList, const NF_SHARE_PTR<NFIDataList>& tagList, const int nMaxRow)
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
        mtRecordVec.push_back(NF_SHARE_PTR<NFIDataList::TData>());
    }

    //optimize would be better, it should be applied memory space only once
    for(int i = 0; i < mVarRecordTag->GetCount(); ++i)
    {
        if(!mVarRecordTag->String(i).empty())
        {
            mmTag[mVarRecordTag->String(i)] = i;
        }
    }
}

NFCRecord::~NFCRecord()
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

int NFCRecord::GetCols() const
{
    return mVarRecordType->GetCount();
}

int NFCRecord::GetRows() const
{
    return mnMaxRow;
}

TDATA_TYPE NFCRecord::GetColType(const int nCol) const
{
    return mVarRecordType->Type(nCol);
}

const std::string& NFCRecord::GetColTag(const int nCol) const
{
    return mVarRecordTag->String(nCol);
}

// 添加数据
int NFCRecord::AddRow(const int nRow)
{
    return AddRow(nRow, *mVarRecordType);
}

int NFCRecord::AddRow(const int nRow, const NFIDataList& var)
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
        if(var.Type(i) != GetColType(i))
        {
            return -1;
        }
    }

    SetUsed(nFindRow, 1);

    for(int i = 0; i < GetCols(); ++i)
    {
        NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nFindRow, i));
        if(nullptr == pVar)
        {
            pVar = NF_SHARE_PTR<NFIDataList::TData>(NF_NEW NFIDataList::TData(var.Type(i)));
        }

        pVar->variantData = var.GetStack(i)->variantData;
    }

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = bCover ? Cover : Add;
    xEventData.nRow = nFindRow;
    xEventData.nCol = 0;
    xEventData.strRecordName = mstrRecordName;

    NFIDataList::TData tData;
    OnEventHandler(mSelf, xEventData, tData, tData); //FIXME:RECORD

    return nFindRow;
}

bool NFCRecord::SetInt(const int nRow, const int nCol, const NFINT64 value)
{
    if(!ValidPos(nRow, nCol))
    {
        return false;
    }

    if(TDATA_INT != GetColType(nCol))
    {
        return false;
    }

    if(!IsUsed(nRow))
    {
        return false;
    }

    NFIDataList::TData var;
    var.SetInt(value);

    NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(!ValidCheck(TDATA_INT, var, pVar))
    {
        return false;
    }

    NFCDataList::TData oldValue;
    oldValue.SetInt(pVar->GetInt());

    pVar->variantData = value;

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool NFCRecord::SetInt(const int nRow, const std::string& strColTag, const NFINT64 value)
{
    int nCol = GetCol(strColTag);
    return SetInt(nRow, nCol, value);
}

bool NFCRecord::SetDouble(const int nRow, const int nCol, const double value)
{
    if(!ValidPos(nRow, nCol))
    {
        return false;
    }

    if(TDATA_DOUBLE != GetColType(nCol))
    {
        return false;
    }

    if(!IsUsed(nRow))
    {
        return false;
    }

    NFIDataList::TData var;
    var.SetDouble(value);

    NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(!ValidCheck(TDATA_DOUBLE, var, pVar))
    {
        return false;
    }

    NFCDataList::TData oldValue;
    oldValue.SetDouble(pVar->GetDouble());

    pVar->variantData = value;

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool NFCRecord::SetDouble(const int nRow, const std::string& strColTag, const double value)
{
    int nCol = GetCol(strColTag);
    return SetDouble(nRow, nCol, value);
}

bool NFCRecord::SetString(const int nRow, const int nCol, const std::string& value)
{
    if(!ValidPos(nRow, nCol))
    {
        return false;
    }

    if(TDATA_STRING != GetColType(nCol))
    {
        return false;
    }

    if(!IsUsed(nRow))
    {
        return false;
    }

    NFIDataList::TData var;
    var.SetString(value);

    NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(!ValidCheck(TDATA_STRING, var, pVar))
    {
        return false;
    }

    NFCDataList::TData oldValue;
    oldValue.SetString(pVar->GetString());

    pVar->variantData = (std::string)value;

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool NFCRecord::SetString(const int nRow, const std::string& strColTag, const std::string& value)
{
    int nCol = GetCol(strColTag);
    return SetString(nRow, nCol, value);
}

bool NFCRecord::SetObject(const int nRow, const int nCol, const NFGUID& value)
{
    if(!ValidPos(nRow, nCol))
    {
        return false;
    }

    if(TDATA_OBJECT != GetColType(nCol))
    {
        return false;
    }

    if(!IsUsed(nRow))
    {
        return false;
    }

    NFIDataList::TData var;
    var.SetObject(value);

    NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(!ValidCheck(TDATA_OBJECT, var, pVar))
    {
        return false;
    }

    NFCDataList::TData oldValue;
    oldValue.SetObject(pVar->GetObject());

    pVar->variantData = value;

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool NFCRecord::SetObject(const int nRow, const std::string& strColTag, const NFGUID& value)
{
    int nCol = GetCol(strColTag);
    return SetObject(nRow, nCol, value);
}

bool NFCRecord::SetPoint(const int nRow, const int nCol, const Point3D& value)
{
    if(!ValidPos(nRow, nCol))
    {
        return false;
    }

    if(TDATA_POINT != GetColType(nCol))
    {
        return false;
    }

    if(!IsUsed(nRow))
    {
        return false;
    }

    NFIDataList::TData var;
    var.SetPoint(value);

    NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(!ValidCheck(TDATA_POINT, var, pVar))
    {
        return false;
    }

    NFCDataList::TData oldValue;
    oldValue.SetPoint(pVar->GetPoint());

    pVar->variantData = value;

    RECORD_EVENT_DATA xEventData;
    xEventData.nOpType = Update;
    xEventData.nRow = nRow;
    xEventData.nCol = nCol;
    xEventData.strRecordName = mstrRecordName;

    OnEventHandler(mSelf, xEventData, oldValue, *pVar);

    return true;
}

bool NFCRecord::SetPoint(const int nRow, const std::string& strColTag, const Point3D& value)
{
    int nCol = GetCol(strColTag);
    return SetPoint(nRow, nCol, value);
}

// 获得数据
bool NFCRecord::QueryRow(const int nRow, NFIDataList& varList)
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
        NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, i));
        if(nullptr != pVar)
        {
            varList.Append(*pVar);
        }
        else
        {
            switch(GetColType(i))
            {
            case TDATA_INT:
                varList.Add(NFINT64(0));
                break;
            case TDATA_DOUBLE:
                varList.Add(0.0f);
                break;
            case TDATA_STRING:
                varList.Add(NULL_STR.c_str());
                break;
            case TDATA_OBJECT:
                varList.Add(NULL_GUID);
                break;
            case TDATA_POINT:
                varList.Add(NULL_POINT);
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

NFINT64 NFCRecord::GetInt(const int nRow, const int nCol) const
{
    if(!ValidPos(nRow, nCol))
    {
        return NULL_INT;
    }

    if(!IsUsed(nRow))
    {
        return NULL_INT;
    }

    const NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(nullptr == pVar)
    {
        return NULL_INT;
    }

    return pVar->GetInt();
}

NFINT64 NFCRecord::GetInt(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetInt(nRow, nCol);
}

double NFCRecord::GetDouble(const int nRow, const int nCol) const
{
    if(!ValidPos(nRow, nCol))
    {
        return NULL_DOUBLE;
    }

    if(!IsUsed(nRow))
    {
        return NULL_DOUBLE;
    }

    const NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(nullptr == pVar)
    {
        return NULL_DOUBLE;
    }

    return pVar->GetDouble();
}

double NFCRecord::GetDouble(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetDouble(nRow, nCol);
}

const std::string& NFCRecord::GetString(const int nRow, const int nCol) const
{
    if(!ValidPos(nRow, nCol))
    {
        return NULL_STR;
    }

    if(!IsUsed(nRow))
    {
        return NULL_STR;
    }

    const NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(nullptr == pVar)
    {
        return NULL_STR;
    }

    return pVar->GetString();
}

const std::string& NFCRecord::GetString(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetString(nRow, nCol);
}

const NFGUID& NFCRecord::GetObject(const int nRow, const int nCol) const
{
    if(!ValidPos(nRow, nCol))
    {
        return NULL_GUID;
    }

    if(!IsUsed(nRow))
    {
        return NULL_GUID;
    }

    const  NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(nullptr == pVar)
    {
        return NULL_GUID;
    }

    return pVar->GetObject();
}

const NFGUID& NFCRecord::GetObject(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetObject(nRow, nCol);
}

const Point3D& NFCRecord::GetPoint(const int nRow, const int nCol) const
{
    if(!ValidPos(nRow, nCol))
    {
        return NULL_POINT;
    }

    if(!IsUsed(nRow))
    {
        return NULL_POINT;
    }

    const  NF_SHARE_PTR<NFIDataList::TData>& pVar = mtRecordVec.at(GetPos(nRow, nCol));
    if(nullptr == pVar)
    {
        return NULL_POINT;
    }

    return pVar->GetPoint();
}

const Point3D& NFCRecord::GetPoint(const int nRow, const std::string& strColTag) const
{
    int nCol = GetCol(strColTag);
    return GetPoint(nRow, nCol);
}

int NFCRecord::FindRowByColValue(const int nCol, const NFIDataList& var, NFIDataList& varResult)
{
    if(!ValidCol(nCol))
    {
        return -1;
    }

    TDATA_TYPE eType = var.Type(0);
    if(eType != mVarRecordType->Type(nCol))
    {
        return -1;
    }

    switch(eType)
    {
    case TDATA_INT:
        return FindInt(nCol, var.Int(nCol), varResult);
        break;
    case TDATA_DOUBLE:
        return FindDouble(nCol, var.Double(nCol), varResult);
        break;
    case TDATA_STRING:
        return FindString(nCol, var.String(nCol).c_str(), varResult);
        break;
    case TDATA_OBJECT:
        return FindObject(nCol, var.Object(nCol), varResult);
        break;
    case TDATA_POINT:
        return FindPoint(nCol, var.Point(nCol), varResult);
        break;
    default:
        break;
    }

    return -1;
}

int NFCRecord::FindRowByColValue(const std::string& strColTag, const NFIDataList& var, NFIDataList& varResult)
{
    int nCol = GetCol(strColTag);
    return FindRowByColValue(nCol, var, varResult);
}

int NFCRecord::FindInt(const int nCol, const NFINT64 value, NFIDataList& varResult)
{
    if(!ValidCol(nCol))
    {
        return -1;
    }

    if(TDATA_INT != mVarRecordType->Type(nCol))
    {
        return -1;
    }

    {
        for(int i = 0; i < mnMaxRow; ++i)
        {
            if(!IsUsed(i))
            {
                continue;
            }

            if(GetInt(i, nCol) == value)
            {
                varResult << i;
            }
        }

        return varResult.GetCount();
    }

    return -1;
}

int NFCRecord::FindInt(const std::string& strColTag, const NFINT64 value, NFIDataList& varResult)
{
    if(strColTag.empty())
    {
        return -1;
    }

    int nCol = GetCol(strColTag);
    return FindInt(nCol, value, varResult);
}

int NFCRecord::FindDouble(const int nCol, const double value, NFIDataList& varResult)
{
    if(!ValidCol(nCol))
    {
        return -1;
    }

    if(TDATA_DOUBLE != mVarRecordType->Type(nCol))
    {
        return -1;
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

int NFCRecord::FindDouble(const std::string& strColTag, const double value, NFIDataList& varResult)
{
    if(strColTag.empty())
    {
        return -1;
    }

    int nCol = GetCol(strColTag);
    return FindDouble(nCol, value, varResult);
}

int NFCRecord::FindString(const int nCol, const std::string& value, NFIDataList& varResult)
{
    if(!ValidCol(nCol))
    {
        return -1;
    }

    if(TDATA_STRING != mVarRecordType->Type(nCol))
    {
        return -1;
    }

    {
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

    return -1;
}

int NFCRecord::FindString(const std::string& strColTag, const std::string& value, NFIDataList& varResult)
{
    if(strColTag.empty())
    {
        return -1;
    }

    int nCol = GetCol(strColTag);
    return FindString(nCol, value, varResult);
}

int NFCRecord::FindObject(const int nCol, const NFGUID& value, NFIDataList& varResult)
{
    if(!ValidCol(nCol))
    {
        return -1;
    }

    if(TDATA_OBJECT != mVarRecordType->Type(nCol))
    {
        return -1;
    }

    {
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

    return -1;
}

int NFCRecord::FindObject(const std::string& strColTag, const NFGUID& value, NFIDataList& varResult)
{
    if(strColTag.empty())
    {
        return -1;
    }

    int nCol = GetCol(strColTag);
    return FindObject(nCol, value, varResult);
}

int NFCRecord::FindPoint(const int nCol, const Point3D& value, NFIDataList& varResult)
{
    if(!ValidCol(nCol))
    {
        return -1;
    }

    if(TDATA_POINT != mVarRecordType->Type(nCol))
    {
        return -1;
    }

    {
        for(int64_t i = 0; i < mnMaxRow; ++i)
        {
            if(!IsUsed(i))
            {
                continue;
            }

            if(GetPoint(i, nCol) == value)
            {
                varResult << i;
            }
        }

        return varResult.GetCount();
    }

    return -1;
}

int NFCRecord::FindPoint(const std::string& strColTag, const Point3D& value, NFIDataList& varResult)
{
    if(strColTag.empty())
    {
        return -1;
    }

    int nCol = GetCol(strColTag);
    return FindPoint(nCol, value, varResult);
}

bool NFCRecord::Remove(const int nRow)
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

            OnEventHandler(mSelf, xEventData, NFCDataList::TData(), NFCDataList::TData());

            mVecUsedState[nRow] = 0;
            return true;
        }
    }

    return false;
}

bool NFCRecord::Clear()
{
    for(int i = GetRows() - 1; i >= 0; i--)
    {
        Remove(i);
    }

    return true;
}

void NFCRecord::AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb)
{
    mtRecordCallback.push_back(cb);
}

const bool NFCRecord::GetSave()
{
    return mbSave;
}

const bool NFCRecord::GetCache()
{
    return mbCache;
}

const bool NFCRecord::GetPublic()
{
    return mbPublic;
}

const bool NFCRecord::GetPrivate()
{
    return mbPrivate;
}

int NFCRecord::GetPos(int nRow, int nCol) const
{
    return nRow * mVarRecordType->GetCount() + nCol;
}

const std::string& NFCRecord::GetName() const
{
    return mstrRecordName;
}

void NFCRecord::SetSave(const bool bSave)
{
    mbSave = bSave;
}

void NFCRecord::SetCache(const bool bCache)
{
    mbCache = bCache;
}

void NFCRecord::SetPublic(const bool bPublic)
{
    mbPublic = bPublic;
}

void NFCRecord::SetPrivate(const bool bPrivate)
{
    mbPrivate = bPrivate;
}

void NFCRecord::SetName(const std::string& strName)
{
    mstrRecordName = strName;
}

const NF_SHARE_PTR<NFIDataList> NFCRecord::GetInitData() const
{
    NF_SHARE_PTR<NFIDataList> pIniData = NF_SHARE_PTR<NFIDataList>(NF_NEW NFCDataList());
    pIniData->Append(*mVarRecordType);

    return pIniData;
}

void NFCRecord::OnEventHandler(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFIDataList::TData& oldVar, const NFIDataList::TData& newVar)
{
    for(auto iter : mtRecordCallback)
    {
        //NFIDataList参数:所属对象名string，操作类型int，Row,Col, OLD属性值，NEW属性值
        (*iter)(self, xEventData, oldVar, newVar);
    }
}

bool NFCRecord::IsUsed(const int nRow) const
{
    if(ValidRow(nRow))
    {
        return (mVecUsedState[nRow] > 0);
    }

    return false;
}

bool NFCRecord::SwapRowInfo(const int nOriginRow, const int nTargetRow)
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
            NF_SHARE_PTR<NFIDataList::TData> pOrigin = mtRecordVec.at(GetPos(nOriginRow, i));
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

        NFCDataList::TData xData;
        OnEventHandler(mSelf, xEventData, xData, xData);

        return true;
    }

    return false;
}

const NF_SHARE_PTR<NFIDataList> NFCRecord::GetTag() const
{
    NF_SHARE_PTR<NFIDataList> pIniData = NF_SHARE_PTR<NFIDataList>(NF_NEW NFCDataList());
    pIniData->Append(*mVarRecordTag);
    return pIniData;
}

const NFIRecord::TRECORDVEC& NFCRecord::GetRecordVec() const
{
    return mtRecordVec;
}

bool NFCRecord::SetUsed(const int nRow, const int bUse)
{
    if(ValidRow(nRow))
    {
        mVecUsedState[nRow] = bUse;
        return true;
    }

    return false;
}

bool NFCRecord::ValidPos(int nRow, int nCol) const
{
    if(ValidCol(nCol)
            && ValidRow(nRow))
    {
        return true;
    }

    return false;
}

bool NFCRecord::ValidRow(int nRow) const
{
    if(nRow >= GetRows() || nRow < 0)
    {
        return false;
    }

    return true;
}

bool NFCRecord::ValidCol(int nCol) const
{
    if(nCol >= GetCols() || nCol < 0)
    {
        return false;
    }

    return true;
}

bool NFCRecord::ValidCheck(TDATA_TYPE eType, const NFIDataList::TData& var, NF_SHARE_PTR<NFIDataList::TData>& pVar)
{
    if(var.GetType() != eType)
    {
        return false;
    }

    if(nullptr == pVar)
    {
        pVar = NF_SHARE_PTR<NFIDataList::TData>(NF_NEW NFIDataList::TData());
        pVar->SetType(eType);
        switch(eType)
        {
        case TDATA_UNKNOWN:
            break;
        case TDATA_INT:
            pVar->variantData = NULL_INT;
            break;
        case TDATA_DOUBLE:
            pVar->variantData = NULL_DOUBLE;
            break;
        case TDATA_STRING:
            pVar->variantData = NULL_STR;
            break;
        case TDATA_OBJECT:
            pVar->variantData = NULL_GUID;
            break;
        case TDATA_POINT:
            pVar->variantData = NULL_POINT;
            break;
        default:
            break;
        }
    }

    if(pVar->GetType() != eType)
    {
        return false;
    }

    if(pVar->variantData == var.variantData)
    {
        return false;
    }

    return true;
}

int NFCRecord::GetCol(const std::string& strTag) const
{
    auto it = mmTag.find(strTag);
    if(it != mmTag.end())
    {
        return it->second;
    }

    return -1;
}