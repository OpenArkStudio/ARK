// -------------------------------------------------------------------------
//    @FileName         :    AFCPropertyMgr.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    AFCPropertyMgr
//
// -------------------------------------------------------------------------

#include "AFPropertyMgr.h"


AFPropertyMgr::AFPropertyMgr(const AFGUID& self)
{
    mxSelf = self;
}

AFPropertyMgr::~AFPropertyMgr()
{
    Clear();
}

void AFPropertyMgr::Clear()
{
    for (size_t i = 0; i < mxPropertys.size(); ++i)
    {
        delete mxPropertys[i];
    }

    mxPropertys.clear();
    mxIndices.Clear();
}

bool AFPropertyMgr::RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
    size_t index;
    if (!FindIndex(strProperty.c_str(), index))
    {
        return false;
    }

    mxPropertyCBs.emplace(std::make_pair(strProperty.c_str(), cb));
    return true;
}

bool AFPropertyMgr::FindIndex(const char* name, size_t& index)
{
    if (!mxIndices.GetData(name, index))
    {
        return false;
    }

    return true;
}

bool AFPropertyMgr::OnPropertyCallback(const char* name, const AFIData& oldData, const AFIData& newData)
{
    if (mxPropertyCBs.empty())
    {
        return false;
    }

    for (auto& iter : mxPropertyCBs)
    {
        (*(iter.second))(mxSelf, name, oldData, newData);
    }

    return true;
}

bool AFPropertyMgr::AddProperty(const char* name, const AFXData& value, bool bPublic, bool bPrivate, bool bSave, bool bRealTime)
{
    AFProperty* pProperty = new AFProperty();
    pProperty->name = name;
    pProperty->prop_value = value;
    pProperty->bPublic = bPublic;
    pProperty->bPrivate = bPrivate;
    pProperty->bSave = bSave;
    pProperty->bRealtime = bRealTime;
    mxIndices.Add(name, mxPropertys.size());
    mxPropertys.push_back(pProperty);

    return true;
}

bool AFPropertyMgr::SetProperty(const char* name, const AFXData& value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    mxPropertys[index]->prop_value = value;

    //TODO:call cb

    return true;
}

bool AFPropertyMgr::SetPropertyBool(const char* name, const bool value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFXData oldData;
    bool oldValue = mxPropertys[index]->prop_value.GetBool();
    oldData.SetBool(oldValue);

    mxPropertys[index]->prop_value.SetBool(value);

    if (oldValue != value)
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFPropertyMgr::SetPropertyInt(const char* name, const int32_t value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFXData oldData;
    int32_t oldValue = mxPropertys[index]->prop_value.GetInt();
    oldData.SetInt(oldValue);

    mxPropertys[index]->prop_value.SetInt(value);

    if (oldValue != value)
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFPropertyMgr::SetPropertyInt64(const char* name, const int64_t value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFXData oldData;
    int64_t oldValue = mxPropertys[index]->prop_value.GetInt64();
    oldData.SetInt64(oldValue);

    mxPropertys[index]->prop_value.SetInt64(value);

    if (oldValue != value)
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFPropertyMgr::SetPropertyFloat(const char* name, const float value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFXData oldData;
    float oldValue = mxPropertys[index]->prop_value.GetFloat();
    oldData.SetFloat(oldValue);

    mxPropertys[index]->prop_value.SetFloat(value);

    if (!IsZeroFloat(oldValue - value))
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFPropertyMgr::SetPropertyDouble(const char* name, const double value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFXData oldData;
    double oldValue = mxPropertys[index]->prop_value.GetDouble();
    oldData.SetDouble(oldValue);

    mxPropertys[index]->prop_value.SetDouble(value);

    if (!IsZeroDouble(oldValue - value))
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFPropertyMgr::SetPropertyString(const char* name, const std::string& value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFXData oldData;
    std::string oldValue = mxPropertys[index]->prop_value.GetString();
    oldData.SetString(oldValue.c_str());

    mxPropertys[index]->prop_value.SetString(value.c_str());

    if (oldValue == value)
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFPropertyMgr::SetPropertyObject(const char* name, const AFGUID& value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFXData oldData;
    AFGUID oldValue = mxPropertys[index]->prop_value.GetObject();
    oldData.SetObject(oldValue);

    mxPropertys[index]->prop_value.SetObject(value);

    if (oldValue == value)
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFPropertyMgr::GetPropertyBool(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_BOOLEAN;
    }

    return mxPropertys[index]->prop_value.GetBool();
}

int32_t AFPropertyMgr::GetPropertyInt(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_INT;
    }

    return mxPropertys[index]->prop_value.GetInt();
}

int64_t AFPropertyMgr::GetPropertyInt64(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_INT64;
    }

    return mxPropertys[index]->prop_value.GetInt64();
}

float AFPropertyMgr::GetPropertyFloat(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_FLOAT;
    }

    return mxPropertys[index]->prop_value.GetFloat();
}

double AFPropertyMgr::GetPropertyDouble(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_DOUBLE;
    }

    return mxPropertys[index]->prop_value.GetDouble();
}

const char* AFPropertyMgr::GetPropertyString(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_STR.c_str();
    }

    return mxPropertys[index]->prop_value.GetString();
}

const AFGUID& AFPropertyMgr::GetPropertyObject(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_GUID;
    }

    return mxPropertys[index]->prop_value.GetObject();
}