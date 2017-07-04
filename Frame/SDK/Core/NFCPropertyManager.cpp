// -------------------------------------------------------------------------
//    @FileName         :    NFCPropertyManager.cpp
//    @Author           :    Ark Game Tech
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

NF_SHARE_PTR<NFIProperty> NFCPropertyManager::AddProperty(const AFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty)
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

NF_SHARE_PTR<NFIProperty> NFCPropertyManager::AddProperty(const AFGUID& self, const std::string& strPropertyName, const int varType)
{
    NF_SHARE_PTR<NFIProperty> pProperty = this->GetElement(strPropertyName);
    if(nullptr == pProperty)
    {
        pProperty = NF_SHARE_PTR<NFIProperty>(NF_NEW NFCProperty(self, strPropertyName, varType));

        AddElement(strPropertyName, pProperty);
    }

    return pProperty;
}

bool NFCPropertyManager::SetProperty(const std::string& strPropertyName, const AFIData& TData)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        pProperty->SetValue(TData);
        return true;
    }

    return false;
}

const AFGUID& NFCPropertyManager::Self()
{
    return mSelf;
}

bool NFCPropertyManager::SetPropertyBool(const std::string& strPropertyName, const bool value)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->SetBool(value);
    }

    return false;
}

bool NFCPropertyManager::SetPropertyInt(const std::string& strPropertyName, const int32_t value)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->SetInt(value);
    }

    return false;
}

bool NFCPropertyManager::SetPropertyInt64(const std::string& strPropertyName, const int64_t value)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->SetInt64(value);
    }

    return false;
}

bool NFCPropertyManager::SetPropertyFloat(const std::string& strPropertyName, const float value)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->SetFloat(value);
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

bool NFCPropertyManager::SetPropertyObject(const std::string& strPropertyName, const AFGUID& value)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->SetObject(value);
    }

    return false;
}

bool NFCPropertyManager::GetPropertyBool(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->GetBool();
    }

    return NULL_BOOLEAN;
}

int32_t NFCPropertyManager::GetPropertyInt(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetInt();
    }

    return NULL_INT;
}

int64_t NFCPropertyManager::GetPropertyInt64(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->GetInt64();
    }

    return NULL_INT64;
}

float NFCPropertyManager::GetPropertyFloat(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->GetFloat();
    }

    return NULL_FLOAT;
}

double NFCPropertyManager::GetPropertyDouble(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetDouble();
    }

    return NULL_DOUBLE;
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

const AFGUID& NFCPropertyManager::GetPropertyObject(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetObject();
    }

    return NULL_GUID;
}