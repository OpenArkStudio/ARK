// -------------------------------------------------------------------------
//    @FileName         :    NFCPropertyManager.cpp
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFCPropertyManager
//
// -------------------------------------------------------------------------

#include "NFCProperty.h"
#include "NFCPropertyManager.h"

NFCPropertyManager::~NFCPropertyManager()
{
    ClearAll();
}

bool NFCPropertyManager::RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
    NF_SHARE_PTR<NFIProperty> pProperty = this->GetElement(strProperty);
    if(nullptr != pProperty)
    {
        pProperty->RegisterCallback(cb);
        return true;
    }

    return false;
}

NF_SHARE_PTR<NFIProperty> NFCPropertyManager::AddProperty(const NFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty)
{
    const std::string& strProperty = pProperty->GetKey();
    NF_SHARE_PTR<NFIProperty> pOldProperty = this->GetElement(strProperty);
    if(nullptr == pOldProperty)
    {
        NF_SHARE_PTR<NFIProperty> pNewProperty(NF_NEW NFCProperty(self, strProperty, pProperty->GetType()));

        pNewProperty->SetPublic(pProperty->GetPublic());
        pNewProperty->SetPrivate(pProperty->GetPrivate());
        pNewProperty->SetSave(pProperty->GetSave());
        pNewProperty->SetCache(pProperty->GetCache());
        pNewProperty->SetRelationValue(pProperty->GetRelationValue());

        AddElement(strProperty, pNewProperty);
    }

    return pOldProperty;
}

NF_SHARE_PTR<NFIProperty> NFCPropertyManager::AddProperty(const NFGUID& self, const std::string& strPropertyName, const TDATA_TYPE varType)
{
    NF_SHARE_PTR<NFIProperty> pProperty = this->GetElement(strPropertyName);
    if(nullptr == pProperty)
    {
        pProperty = NF_SHARE_PTR<NFIProperty>(NF_NEW NFCProperty(self, strPropertyName, varType));

        AddElement(strPropertyName, pProperty);
    }

    return pProperty;
}

bool NFCPropertyManager::SetProperty(const std::string& strPropertyName, const NFIDataList::TData& TData)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        pProperty->SetValue(TData);
        return true;
    }

    return false;
}

const NFGUID& NFCPropertyManager::Self()
{
    return mSelf;
}

bool NFCPropertyManager::SetPropertyInt(const std::string& strPropertyName, const NFINT64 value)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->SetInt(value);
    }

    return false;
}

bool NFCPropertyManager::SetPropertyDouble(const std::string& strPropertyName, const double value)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->SetDouble(value);
    }

    return false;
}

bool NFCPropertyManager::SetPropertyString(const std::string& strPropertyName, const std::string& value)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->SetString(value);
    }

    return false;
}

bool NFCPropertyManager::SetPropertyObject(const std::string& strPropertyName, const NFGUID& value)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->SetObject(value);
    }

    return false;
}

bool NFCPropertyManager::SetPropertyPoint(const std::string& strPropertyName, const Point3D& value)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->SetPoint(value);
    }

    return false;
}

NFINT64 NFCPropertyManager::GetPropertyInt(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetInt();
    }

    return 0;
}

double NFCPropertyManager::GetPropertyDouble(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetDouble();
    }

    return 0.0;
}

const std::string& NFCPropertyManager::GetPropertyString(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetString();
    }

    return NULL_STR;
}

const NFGUID& NFCPropertyManager::GetPropertyObject(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetObject();
    }

    return NULL_GUID;
}

const Point3D& NFCPropertyManager::GetPropertyPoint(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetPoint();
    }

    return NULL_POINT;
}
