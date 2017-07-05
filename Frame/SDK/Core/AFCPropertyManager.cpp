// -------------------------------------------------------------------------
//    @FileName         :    AFCPropertyManager.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    AFCPropertyManager
//
// -------------------------------------------------------------------------

#include "AFCProperty.h"
#include "AFCPropertyManager.h"

AFCPropertyManager::~AFCPropertyManager()
{
    ClearAll();
}

bool AFCPropertyManager::RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
    NF_SHARE_PTR<AFIProperty> pProperty = this->GetElement(strProperty);
    if(nullptr != pProperty)
    {
        pProperty->RegisterCallback(cb);
        return true;
    }

    return false;
}

NF_SHARE_PTR<AFIProperty> AFCPropertyManager::AddProperty(const AFGUID& self, NF_SHARE_PTR<AFIProperty> pProperty)
{
    const std::string& strProperty = pProperty->GetKey();
    NF_SHARE_PTR<AFIProperty> pOldProperty = this->GetElement(strProperty);
    if(nullptr == pOldProperty)
    {
        NF_SHARE_PTR<AFIProperty> pNewProperty(NF_NEW AFCProperty(self, strProperty, pProperty->GetType()));

        pNewProperty->SetPublic(pProperty->GetPublic());
        pNewProperty->SetPrivate(pProperty->GetPrivate());
        pNewProperty->SetSave(pProperty->GetSave());
        pNewProperty->SetCache(pProperty->GetCache());
        pNewProperty->SetRelationValue(pProperty->GetRelationValue());

        AddElement(strProperty, pNewProperty);
    }

    return pOldProperty;
}

NF_SHARE_PTR<AFIProperty> AFCPropertyManager::AddProperty(const AFGUID& self, const std::string& strPropertyName, const int varType)
{
    NF_SHARE_PTR<AFIProperty> pProperty = this->GetElement(strPropertyName);
    if(nullptr == pProperty)
    {
        pProperty = NF_SHARE_PTR<AFIProperty>(NF_NEW AFCProperty(self, strPropertyName, varType));

        AddElement(strPropertyName, pProperty);
    }

    return pProperty;
}

bool AFCPropertyManager::SetProperty(const std::string& strPropertyName, const AFIData& TData)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        pProperty->SetValue(TData);
        return true;
    }

    return false;
}

const AFGUID& AFCPropertyManager::Self()
{
    return mSelf;
}

bool AFCPropertyManager::SetPropertyBool(const std::string& strPropertyName, const bool value)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->SetBool(value);
    }

    return false;
}

bool AFCPropertyManager::SetPropertyInt(const std::string& strPropertyName, const int32_t value)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->SetInt(value);
    }

    return false;
}

bool AFCPropertyManager::SetPropertyInt64(const std::string& strPropertyName, const int64_t value)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->SetInt64(value);
    }

    return false;
}

bool AFCPropertyManager::SetPropertyFloat(const std::string& strPropertyName, const float value)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->SetFloat(value);
    }

    return false;
}

bool AFCPropertyManager::SetPropertyDouble(const std::string& strPropertyName, const double value)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->SetDouble(value);
    }

    return false;
}

bool AFCPropertyManager::SetPropertyString(const std::string& strPropertyName, const std::string& value)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->SetString(value);
    }

    return false;
}

bool AFCPropertyManager::SetPropertyObject(const std::string& strPropertyName, const AFGUID& value)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->SetObject(value);
    }

    return false;
}

bool AFCPropertyManager::GetPropertyBool(const std::string& strPropertyName)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->GetBool();
    }

    return NULL_BOOLEAN;
}

int32_t AFCPropertyManager::GetPropertyInt(const std::string& strPropertyName)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetInt();
    }

    return NULL_INT;
}

int64_t AFCPropertyManager::GetPropertyInt64(const std::string& strPropertyName)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->GetInt64();
    }

    return NULL_INT64;
}

float AFCPropertyManager::GetPropertyFloat(const std::string& strPropertyName)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if (nullptr != pProperty)
    {
        return pProperty->GetFloat();
    }

    return NULL_FLOAT;
}

double AFCPropertyManager::GetPropertyDouble(const std::string& strPropertyName)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetDouble();
    }

    return NULL_DOUBLE;
}

const std::string& AFCPropertyManager::GetPropertyString(const std::string& strPropertyName)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetString();
    }

    return NULL_STR;
}

const AFGUID& AFCPropertyManager::GetPropertyObject(const std::string& strPropertyName)
{
    NF_SHARE_PTR<AFIProperty> pProperty = GetElement(strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetObject();
    }

    return NULL_GUID;
}