/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include "AFCPropertyMgr.h"

AFCPropertyMgr::AFCPropertyMgr(const AFGUID& self)
{
    mxSelf = self;
}

AFCPropertyMgr::~AFCPropertyMgr()
{
    Clear();
}

void AFCPropertyMgr::Clear()
{
    for(size_t i = 0; i < mxPropertys.size(); ++i)
    {
        delete mxPropertys[i];
    }

    for(size_t i = 0; i < mxPropertyCBs.size(); ++i)
    {
        delete mxPropertyCBs[i];
    }

    mxPropertys.clear();
    mxIndices.Clear();
    mxPropertyCBs.clear();
    mxCallBackIndices.Clear();
}

const AFGUID& AFCPropertyMgr::Self() const
{
    return mxSelf;
}

bool AFCPropertyMgr::RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
    size_t index(0);
    if(!FindIndex(strProperty.c_str(), index))
    {
        return false;
    }

    //mxPropertyCBs.emplace(std::make_pair(strProperty.c_str(), cb));
    size_t indexCallback(0);
    if(mxCallBackIndices.GetData(strProperty.c_str(), indexCallback))
    {
        mxPropertyCBs[indexCallback]->mxCallBackList.push_back(cb);
    }
    else
    {
        AFPropertyCallBack* pPropertyCB = new AFPropertyCallBack();
        pPropertyCB->mxCallBackList.push_back(cb);
        mxCallBackIndices.Add(strProperty.c_str(), mxPropertyCBs.size());
        mxPropertyCBs.push_back(pPropertyCB);
    }
    return true;
}

size_t AFCPropertyMgr::GetPropertyCount()
{
    return mxPropertys.size();
}

AFProperty* AFCPropertyMgr::GetPropertyByIndex(size_t index)
{
    if(index < 0 || index > mxPropertys.size())
    {
        return NULL;
    }

    return mxPropertys[index];
}

AFProperty* AFCPropertyMgr::GetProperty(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    return mxPropertys[index];
}

bool AFCPropertyMgr::FindIndex(const char* name, size_t& index)
{
    if(!mxIndices.GetData(name, index))
    {
        return false;
    }

    return true;
}

bool AFCPropertyMgr::OnPropertyCallback(const char* name, const AFIData& oldData, const AFIData& newData)
{
    //if(mxPropertyCBs.empty())
    //{
    //    return false;
    //}

    //auto& range = mxPropertyCBs.equal_range(name);
    //for(auto& iter = range.first; iter != range.second; ++iter)
    //{
    //    (*(iter->second))(mxSelf, name, oldData, newData);
    //}

    size_t indexCallBack = 0;
    if(!mxCallBackIndices.GetData(name, indexCallBack))
    {
        return false;
    }

    for(int i = 0; i < mxPropertyCBs[indexCallBack]->mxCallBackList.size(); i++)
    {
        (*(mxPropertyCBs[indexCallBack]->mxCallBackList[i]))(mxSelf, name, oldData, newData);
    }

    return true;
}

bool AFCPropertyMgr::AddProperty(const char* name, const AFIData& value, const int8_t feature)
{
    AFProperty* pProperty = new AFProperty();
    pProperty->name = name;
    pProperty->prop_value = value;
    pProperty->feature = feature;
    mxIndices.Add(name, mxPropertys.size());
    mxPropertys.push_back(pProperty);
    return true;
}

bool AFCPropertyMgr::SetProperty(const char* name, const AFIData& value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    AFCData oldValue = mxPropertys[index]->prop_value;
    mxPropertys[index]->prop_value = value;

    //TODO:call cb

    return true;
}

bool AFCPropertyMgr::SetPropertyBool(const char* name, const bool value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    bool oldValue = mxPropertys[index]->prop_value.GetBool();
    oldData.SetBool(oldValue);

    mxPropertys[index]->prop_value.SetBool(value);

    if(oldValue != value)
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFCPropertyMgr::SetPropertyInt(const char* name, const int32_t value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    int32_t oldValue = mxPropertys[index]->prop_value.GetInt();
    oldData.SetInt(oldValue);

    mxPropertys[index]->prop_value.SetInt(value);

    if(oldValue != value)
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFCPropertyMgr::SetPropertyInt64(const char* name, const int64_t value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    int64_t oldValue = mxPropertys[index]->prop_value.GetInt64();
    oldData.SetInt64(oldValue);

    mxPropertys[index]->prop_value.SetInt64(value);

    if(oldValue != value)
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFCPropertyMgr::SetPropertyFloat(const char* name, const float value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    float oldValue = mxPropertys[index]->prop_value.GetFloat();
    oldData.SetFloat(oldValue);

    mxPropertys[index]->prop_value.SetFloat(value);

    if(!IsZeroFloat(oldValue - value))
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFCPropertyMgr::SetPropertyDouble(const char* name, const double value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    double oldValue = mxPropertys[index]->prop_value.GetDouble();
    oldData.SetDouble(oldValue);

    mxPropertys[index]->prop_value.SetDouble(value);

    if(!IsZeroDouble(oldValue - value))
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFCPropertyMgr::SetPropertyString(const char* name, const std::string& value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    std::string oldValue = mxPropertys[index]->prop_value.GetString();
    oldData.SetString(oldValue.c_str());

    mxPropertys[index]->prop_value.SetString(value.c_str());

    if(oldValue != value)
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFCPropertyMgr::SetPropertyObject(const char* name, const AFGUID& value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    AFGUID oldValue = mxPropertys[index]->prop_value.GetObject();
    oldData.SetObject(oldValue);

    mxPropertys[index]->prop_value.SetObject(value);

    if(oldValue != value)
    {
        //property callbacks
        OnPropertyCallback(name, oldData, mxPropertys[index]->prop_value);
    }

    return true;
}

bool AFCPropertyMgr::GetPropertyBool(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_BOOLEAN;
    }

    return mxPropertys[index]->prop_value.GetBool();
}

int32_t AFCPropertyMgr::GetPropertyInt(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_INT;
    }

    return mxPropertys[index]->prop_value.GetInt();
}

int64_t AFCPropertyMgr::GetPropertyInt64(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_INT64;
    }

    return mxPropertys[index]->prop_value.GetInt64();
}

float AFCPropertyMgr::GetPropertyFloat(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_FLOAT;
    }

    return mxPropertys[index]->prop_value.GetFloat();
}

double AFCPropertyMgr::GetPropertyDouble(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_DOUBLE;
    }

    return mxPropertys[index]->prop_value.GetDouble();
}

const char* AFCPropertyMgr::GetPropertyString(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_STR.c_str();
    }

    return mxPropertys[index]->prop_value.GetString();
}

const AFGUID& AFCPropertyMgr::GetPropertyObject(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_GUID;
    }

    return mxPropertys[index]->prop_value.GetObject();
}