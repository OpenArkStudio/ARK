// -------------------------------------------------------------------------
//    @FileName         :    NFCProperty.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    NFCProperty
//
// -------------------------------------------------------------------------

#include "NFCProperty.h"
#include <complex>

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
    if(eType != xData.GetType())
    {
        return;
    }

    //新值旧值都是默认值，则不用设置了
    if(xData.IsNullValue() && mxData->IsNullValue())
    {
        return;
    }

    if(nullptr == mxData)
    {
        mxData = NF_SHARE_PTR<AFIData>(NF_NEW AFXData(xData));
    }

    AFXData oldValue;
    oldValue = *mxData;

    mxData->Assign(xData);

    AFXData newValue;
    newValue = *mxData;

    OnEventHandler(oldValue, newValue);
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

bool NFCProperty::GetBool() const
{
    if (nullptr == mxData)
    {
        return NULL_BOOLEAN;
    }

    return mxData->GetBool();
}

int32_t NFCProperty::GetInt() const
{
    if(nullptr == mxData)
    {
        return NULL_INT;
    }

    return mxData->GetInt();
}

int64_t NFCProperty::GetInt64() const
{
    if (nullptr == mxData)
    {
        return NULL_INT64;
    }

    return mxData->GetInt64();
}

float NFCProperty::GetFloat() const
{
    if (nullptr == mxData)
    {
        return NULL_FLOAT;
    }

    return mxData->GetFloat();
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

bool NFCProperty::SetBool(const bool value)
{
    if (eType != DT_BOOLEAN)
    {
        return false;
    }

    if (nullptr == mxData)
    {
        //本身是空就是因为没数据，还来个没数据的就不存了
        if (NULL_BOOLEAN == value)
        {
            return false;
        }

        mxData = NF_SHARE_PTR<AFIData>(NF_NEW AFXData());
        mxData->SetBool(NULL_BOOLEAN);
    }

    if (value == mxData->GetBool())
    {
        return false;
    }

    AFXData oldValue;
    oldValue = *mxData;

    mxData->SetBool(value);

    OnEventHandler(oldValue, *mxData);

    return true;
}

bool NFCProperty::SetInt(const int32_t value)
{
    if(eType != DT_INT)
    {
        return false;
    }

    if(nullptr == mxData)
    {
        //本身是空就是因为没数据，还来个没数据的就不存了
        if(NULL_INT == value)
        {
            return false;
        }

        mxData = NF_SHARE_PTR<AFIData>(NF_NEW AFXData());
        mxData->SetInt(NULL_INT);
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

bool NFCProperty::SetInt64(const int32_t value)
{
    if (eType != DT_INT64)
    {
        return false;
    }

    if (nullptr == mxData)
    {
        //本身是空就是因为没数据，还来个没数据的就不存了
        if (NULL_INT64 == value)
        {
            return false;
        }

        mxData = NF_SHARE_PTR<AFIData>(NF_NEW AFXData());
        mxData->SetInt64(NULL_INT64);
    }

    if (value == mxData->GetInt64())
    {
        return false;
    }

    AFXData oldValue;
    oldValue = *mxData;

    mxData->SetInt64(value);

    OnEventHandler(oldValue, *mxData);

    return true;
}

bool NFCProperty::SetFloat(const float value)
{
    if (eType != DT_FLOAT)
    {
        return false;
    }

    if (nullptr == mxData)
    {
        //本身是空就是因为没数据，还来个没数据的就不存了
        if (NULL_FLOAT == value)
        {
            return false;
        }

        mxData = NF_SHARE_PTR<AFIData>(NF_NEW AFXData());
        mxData->SetFloat(NULL_FLOAT);
    }

    if (value == mxData->GetFloat())
    {
        return false;
    }

    AFXData oldValue;
    oldValue = *mxData;

    mxData->SetFloat(value);

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
    return !(GetValue().IsNullValue());
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
        strData = AF_LEXICAL_CAST<std::string>(GetBool());
        break;
    case DT_INT:
        strData = AF_LEXICAL_CAST<std::string> (GetInt());
        break;
    case DT_INT64:
        strData = AF_LEXICAL_CAST<std::string>(GetInt64());
        break;
    case DT_FLOAT:
        strData = AF_LEXICAL_CAST<std::string>(GetFloat());
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
            bool value = NULL_BOOLEAN;
            bRet = NF_StrTo(strData, value);
            SetBool(value);
        }
        break;
    case DT_INT:
        {
            int32_t value = NULL_INT;
            bRet = NF_StrTo(strData, value);
            SetInt(value);
        }
        break;
    case DT_INT64:
        {
            int64_t value = NULL_INT64;
            bRet = NF_StrTo(strData, value);
            SetInt64(value);
        }
        break;
    case DT_FLOAT:
        {
            float value = NULL_FLOAT;
            bRet = NF_StrTo(strData, value);
            SetFloat(value);
        }
        break;
    case DT_DOUBLE:
        {
            double value = 0;
            bRet = NF_StrTo(strData, value);
            SetDouble(value);
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