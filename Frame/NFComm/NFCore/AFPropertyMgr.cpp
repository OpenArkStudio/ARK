// -------------------------------------------------------------------------
//    @FileName         :    AFCPropertyMgr.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    AFCPropertyMgr
//
// -------------------------------------------------------------------------

#include "AFPropertyMgr.h"

AFPropertyMgr::AFPropertyMgr()
{

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

//TODO:变化回调函数
//bool AFPropertyMgr::RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb)
//{
//    NF_SHARE_PTR<NFIProperty> pProperty = this->GetElement(strProperty);
//    if(nullptr != pProperty)
//    {
//        pProperty->RegisterCallback(cb);
//        return true;
//    }
//
//    return false;
//}

bool AFPropertyMgr::FindIndex(const char* name, size_t& index)
{
    if (!mxIndices.GetData(name, index))
    {
        return false;
    }

    return true;
}

bool AFPropertyMgr::AddProperty(const char* name, const AFDataList::TData& value)
{
    AFProperty* pProperty = new AFProperty();
    pProperty->name = name;
    pProperty->value = value;
    mxIndices.Add(name, mxPropertys.size());
    mxPropertys.push_back(pProperty);

    return true;
}

bool AFPropertyMgr::SetProperty(const char* name, const AFDataList::TData& value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    mxPropertys[index]->value = value;
    return true;
}

bool AFPropertyMgr::SetPropertyInt(const char* name, const NFINT64 value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    mxPropertys[index]->value.SetInt(value);
    return true;
}

bool AFPropertyMgr::SetPropertyDouble(const char* name, const double value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    mxPropertys[index]->value.SetDouble(value);
    return true;
}

bool AFPropertyMgr::SetPropertyString(const char* name, const std::string& value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    mxPropertys[index]->value.SetString(value);
    return true;
}

bool AFPropertyMgr::SetPropertyObject(const char* name, const NFGUID& value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    mxPropertys[index]->value.SetObject(value);
    return true;
}

bool AFPropertyMgr::SetPropertyPoint(const char* name, const Point3D& value)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return false;
    }

    mxPropertys[index]->value.SetPoint(value);
    return true;
}

NFINT64 AFPropertyMgr::GetPropertyInt(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_INT;
    }

    return mxPropertys[index]->value.GetInt();
}

double AFPropertyMgr::GetPropertyDouble(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_DOUBLE;
    }

    return mxPropertys[index]->value.GetDouble();
}

const std::string& AFPropertyMgr::GetPropertyString(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_STR;
    }

    return mxPropertys[index]->value.GetString();
}

const NFGUID& AFPropertyMgr::GetPropertyObject(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_GUID;
    }

    return mxPropertys[index]->value.GetObject();
}

const Point3D& AFPropertyMgr::GetPropertyPoint(const char* name)
{
    size_t index;
    if (!FindIndex(name, index))
    {
        return NULL_POINT;
    }

    return mxPropertys[index]->value.GetPoint();
}