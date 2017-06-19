// -------------------------------------------------------------------------
//    @FileName         :    NFCProperty.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    NFCProperty
//
// -------------------------------------------------------------------------

#include "NFCProperty.h"
#include <complex>

NFCProperty::NFCProperty()
{
    mbPublic = false;
    mbPrivate = false;
    mbSave = false;
    mbCache = false;

    mSelf = NULL_GUID;
    eType = DT_UNKNOWN;

    msPropertyName = "";
}

NFCProperty::NFCProperty(const AFGUID& self, const std::string& strPropertyName, const int varType)
{
    mbPublic = false;
    mbPrivate = false;
    mbSave = false;
    mbCache = false;

    mstrRelationValue = "";

    mSelf = self;

    msPropertyName = strPropertyName;
    eType = (AF_DATA_TYPE)varType;
}

NFCProperty::~NFCProperty()
{
    for(auto& iter = mtPropertyCallback.begin(); iter != mtPropertyCallback.end(); ++iter)
    {
        iter->reset();
    }

    mtPropertyCallback.clear();
    mxData.reset();
}

void NFCProperty::SetValue(const AFIData& xData)
{
    //TODO:
    //if(eType != xData.GetType())
    //{
    //    return;
    //}

    ////TODO:如果为空就不用设置了
    ////if(xData.IsNullValue())
    ////{
    ////    return;
    ////}

    //if(nullptr == mxData)
    //{
    //    mxData = NF_SHARE_PTR<AFIData>(NF_NEW AFXData(xData));
    //}

    //AFXData oldValue;
    //oldValue = *mxData;

    //*mxData = xData;

    //AFXData newValue;
    //newValue = *mxData;

    //OnEventHandler(oldValue, newValue);
}

void NFCProperty::SetValue(const NFIProperty* pProperty)
{
    SetValue(pProperty->GetValue());
}

const AFIData& NFCProperty::GetValue() const
{
    if(nullptr != mxData)
    {
        return *mxData;
    }

    return AFXData();
}

const std::string&  NFCProperty::GetKey() const
{
    return msPropertyName;
}

const bool NFCProperty::GetSave() const
{
    return mbSave;
}

const bool NFCProperty::GetPublic() const
{
    return mbPublic;
}

const bool NFCProperty::GetPrivate() const
{
    return mbPrivate;
}

const bool NFCProperty::GetCache() const
{
    return mbCache;
}

const std::string& NFCProperty::GetRelationValue() const
{
    return mstrRelationValue;
}

void NFCProperty::SetSave(bool bSave)
{
    mbSave = bSave;
}

void NFCProperty::SetPublic(bool bPublic)
{
    mbPublic = bPublic;
}

void NFCProperty::SetPrivate(bool bPrivate)
{
    mbPrivate = bPrivate;
}

void NFCProperty::SetCache(bool bCache)
{
    mbCache = bCache;
}

void NFCProperty::SetRelationValue(const std::string& strRelationValue)
{
    mstrRelationValue = strRelationValue;
}

NFINT64 NFCProperty::GetInt() const
{
    if(nullptr == mxData)
    {
        return 0;
    }

    return mxData->GetInt();
}

double NFCProperty::GetDouble() const
{
    if(nullptr == mxData)
    {
        return 0.0;
    }

    return mxData->GetDouble();
}

const std::string& NFCProperty::GetString() const
{
    if(nullptr == mxData)
    {
        return NULL_STR;
    }

    return mxData->GetString();
}

const AFGUID& NFCProperty::GetObject() const
{
    if(nullptr == mxData)
    {
        return NULL_GUID;
    }

    return mxData->GetObject();
}

//const Point3D& NFCProperty::GetPoint() const
//{
//    if(nullptr == mxData)
//    {
//        return NULL_POINT;
//    }
//
//    return mxData->GetPoint();
//}

void NFCProperty::RegisterCallback(const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
    mtPropertyCallback.push_back(cb);
}

int NFCProperty::OnEventHandler(const AFIData& oldVar, const AFIData& newVar)
{
    if(mtPropertyCallback.size() <= 0)
    {
        return 0;
    }

    TPROPERTYCALLBACKEX::iterator it = mtPropertyCallback.begin();
    TPROPERTYCALLBACKEX::iterator end = mtPropertyCallback.end();
    for(it; it != end; ++it)
    {
        //AFDataList参数:属性名，OLD属性值，NEW属性值, ARG参数(pKernel,self)
        (**it)(mSelf, msPropertyName, oldVar, newVar);
    }

    return 0;
}

bool NFCProperty::SetInt(const NFINT64 value)
{
    if(eType != DT_INT)
    {
        return false;
    }

    if(nullptr == mxData)
    {
        //本身是空就是因为没数据，还来个没数据的就不存了
        if(0 == value)
        {
            return false;
        }

        mxData = NF_SHARE_PTR<AFIData>(NF_NEW AFXData());
        mxData->SetInt(0);
    }

    if(value == mxData->GetInt())
    {
        return false;
    }

    AFXData oldValue;
    oldValue = *mxData;

    mxData->SetInt(value);

    OnEventHandler(oldValue, *mxData);

    return true;
}

bool NFCProperty::SetDouble(const double value)
{
    if(eType != DT_DOUBLE)
    {
        return false;
    }

    if(nullptr == mxData)
    {
        //本身是空就是因为没数据，还来个没数据的就不存了
        if(IsZeroDouble(value))
        {
            return false;
        }

        mxData = NF_SHARE_PTR<AFIData>(NF_NEW AFXData());
        mxData->SetDouble(NULL_DOUBLE);
    }

    if(IsZeroDouble(value - mxData->GetDouble()))
    {
        return false;
    }

    AFXData oldValue;
    oldValue = *mxData;

    mxData->SetDouble(value);

    OnEventHandler(oldValue, *mxData);

    return true;
}

bool NFCProperty::SetString(const std::string& value)
{
    if(eType != DT_STRING)
    {
        return false;
    }

    if(nullptr == mxData)
    {
        //本身是空就是因为没数据，还来个没数据的就不存了
        if(value.empty())
        {
            return false;
        }

        mxData = NF_SHARE_PTR<AFIData>(NF_NEW AFXData());
        mxData->SetString(NULL_STR.c_str());
    }

    if(value == mxData->GetString())
    {
        return false;
    }

    AFXData oldValue;
    oldValue = *mxData;

    mxData->SetString(value.c_str());

    OnEventHandler(oldValue, *mxData);

    return true;
}

bool NFCProperty::SetObject(const AFGUID& value)
{
    if(eType != DT_OBJECT)
    {
        return false;
    }

    if(nullptr == mxData)
    {
        //本身是空就是因为没数据，还来个没数据的就不存了
        if(value.IsNull())
        {
            return false;
        }

        mxData = NF_SHARE_PTR<AFIData>(NF_NEW AFXData());
        mxData->SetObject(NULL_GUID);
    }

    if(value == mxData->GetObject())
    {
        return false;
    }

    AFXData oldValue;
    oldValue = *mxData;

    mxData->SetObject(value);

    OnEventHandler(oldValue, *mxData);

    return true;
}

bool NFCProperty::Changed() const
{
    //TODO:
    //return !(GetValue().IsNullValue());
    return true;
}

const int NFCProperty::GetType() const
{
    return eType;
}

const bool NFCProperty::GeUsed() const
{
    if(nullptr == mxData)
    {
        return true;
    }

    return false;
}

std::string NFCProperty::ToString()
{
    std::string strData;
    const int eType = GetType();
    switch(eType)
    {
    case DT_BOOLEAN:
        //strData = AF_LEXICAL_CAST<std::string>(GetBool());
        break;
    case DT_INT:
        strData = AF_LEXICAL_CAST<std::string> (GetInt());
        break;
    case DT_INT64:
        //TODO
        break;
    case DT_FLOAT:
        //TODO
        break;
    case DT_DOUBLE:
        strData = AF_LEXICAL_CAST<std::string> (GetDouble());
        break;
    case DT_STRING:
        strData = GetString();
        break;
    case DT_OBJECT:
        strData = GetObject().ToString();
        break;
    //case DT_POINT:
    //    strData = GetPoint().ToString();
    //    break;
    default:
        strData = NULL_STR;
        break;
    }

    return strData;
}

bool NFCProperty::FromString(const std::string& strData)
{
    const int eType = GetType();
    bool bRet = false;
    switch(eType)
    {
    case DT_BOOLEAN:
        {
            //TODO
        }
        break;
    case DT_INT:
        {
            NFINT64  nValue = 0;
            bRet = NF_StrTo(strData, nValue);
            SetInt(nValue);
        }
        break;
    case DT_INT64:
        {
            //todo
        }
        break;
    case DT_FLOAT:
        {
            //todo
        }
        break;
    case DT_DOUBLE:
        {
            double  dValue = 0;
            bRet = NF_StrTo(strData, dValue);
            SetDouble(dValue);
        }
        break;

    case DT_STRING:
        {
            SetString(strData);
            bRet = true;
        }
        break;
    case DT_OBJECT:
        {
            AFGUID xID;

            bRet = xID.FromString(strData);
            SetObject(xID);
        }
        break;
    default:
        break;
    }

    return bRet;
}

bool NFCProperty::DeSerialization()
{
    //bool bRet = false;

    //const int eType = GetType();
    //if(eType == TDATA_STRING && nullptr != mxData && !mxData->IsNullValue())
    //{
    //    AFDataList xDataList;
    //    const std::string& strData = mxData->GetString();

    //    xDataList.Split(strData.c_str(), ";");
    //    if(xDataList.GetCount() <= 0)
    //    {
    //        return bRet;
    //    }

    //    AFDataList xTemDataList;
    //    xTemDataList.Split(xDataList.String(0).c_str(), ",");
    //    const int nSubDataLength = xTemDataList.GetCount();

    //    if(xDataList.GetCount() == 1 && nSubDataLength == 1)
    //    {
    //        //most of property value only one value
    //        return bRet;
    //    }

    //    if(nullptr == mxEmbeddedList)
    //    {
    //        mxEmbeddedList = NF_SHARE_PTR<NFList<std::string>>(NF_NEW NFList<std::string>());
    //    }
    //    else
    //    {
    //        mxEmbeddedList->ClearAll();
    //    }

    //    for(int i = 0; i < xDataList.GetCount(); ++i)
    //    {
    //        if(xDataList.String(i).empty())
    //        {
    //            NFASSERT(0, strData, __FILE__, __FUNCTION__);
    //        }

    //        mxEmbeddedList->Add(xDataList.String(i));
    //    }



    //    ////////////////////////////////////////

    //    if(nSubDataLength < 2 || nSubDataLength > 2)
    //    {
    //        return bRet;
    //    }

    //    if(nullptr == mxEmbeddedMap)
    //    {
    //        mxEmbeddedMap = NF_SHARE_PTR<NFMapEx<std::string, std::string>>(NF_NEW NFMapEx<std::string, std::string>());
    //    }
    //    else
    //    {
    //        mxEmbeddedMap->ClearAll();
    //    }

    //    for(int i = 0; i < xDataList.GetCount(); ++i)
    //    {
    //        AFDataList xTemDataList;
    //        const std::string& strTemData = xDataList.String(i);
    //        xTemDataList.Split(strTemData.c_str(), ",");
    //        {
    //            if(xTemDataList.GetCount() != nSubDataLength)
    //            {
    //                NFASSERT(0, strTemData, __FILE__, __FUNCTION__);
    //            }

    //            const std::string& strKey = xTemDataList.String(0);
    //            const std::string& strValue = xTemDataList.String(1);

    //            if(strKey.empty() || strValue.empty())
    //            {
    //                NFASSERT(0, strTemData, __FILE__, __FUNCTION__);
    //            }

    //            mxEmbeddedMap->AddElement(strKey, NF_SHARE_PTR<std::string>(NF_NEW std::string(strValue)));
    //        }
    //    }

    //    bRet = true;
    //}

    //return bRet;
    return true;
}

const NF_SHARE_PTR<NFList<std::string>> NFCProperty::GetEmbeddedList() const
{
    return this->mxEmbeddedList;
}

const NF_SHARE_PTR<NFMapEx<std::string, std::string>> NFCProperty::GetEmbeddedMap() const
{
    return this->mxEmbeddedMap;
}
