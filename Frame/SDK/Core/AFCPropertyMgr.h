/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @FileName  AFCPropertyMgr.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFCPropertyMgr                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFCPropertyMgr                                                  *
*****************************************************************************/
#pragma once

#include <unordered_map>
#include "AFIPropertyMgr.h"
#include "SDK/Base/AFNoncopyable.hpp"
#include "SDK/Base/AFBitValue.hpp"


class AFCPropertyMgr : public AFIPropertyMgr, AFNoncopyable
{
public:
    AFCPropertyMgr() = delete;

    AFCPropertyMgr(const AFGUID& self);
    virtual ~AFCPropertyMgr();
    virtual void Clear();

    virtual const AFGUID& Self() const;

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb);

    virtual size_t GetPropertyCount();
    virtual AFProperty* GetPropertyByIndex(size_t index);
    virtual AFProperty* GetProperty(const char* name);
    virtual bool AddProperty(const char* name, const AFIData& value, const int8_t feature);
    virtual bool SetProperty(const char* name, const AFIData& value);

    virtual bool SetPropertyBool(const char* name, const bool value);
    virtual bool SetPropertyInt(const char* name, const int32_t value);
    virtual bool SetPropertyInt64(const char* name, const int64_t value);
    virtual bool SetPropertyFloat(const char* name, const float value);
    virtual bool SetPropertyDouble(const char* name, const double value);
    virtual bool SetPropertyString(const char* name, const std::string& value);
    virtual bool SetPropertyObject(const char* name, const AFGUID& value);

    virtual bool GetPropertyBool(const char* name);
    virtual int32_t GetPropertyInt(const char* name);
    virtual int64_t GetPropertyInt64(const char* name);
    virtual float GetPropertyFloat(const char* name);
    virtual double GetPropertyDouble(const char* name);
    virtual const char* GetPropertyString(const char* name);
    virtual const AFGUID& GetPropertyObject(const char* name);

protected:
    bool FindIndex(const char* name, size_t& index);

    bool OnPropertyCallback(const char* name, const AFIData& oldData, const AFIData& newData);

private:
    struct  AFPropertyCallBack
    {
        std::vector<PROPERTY_EVENT_FUNCTOR_PTR> mxCallBackList;
    };

private:
    ArraryPod<AFProperty*, 1, CoreAlloc> mxPropertys;
    StringPod<char, size_t, StringTraits<char>, CoreAlloc> mxIndices;

    ArraryPod<AFPropertyCallBack*, 1, CoreAlloc> mxPropertyCBs;
    StringPod<char, size_t, StringTraits<char>, CoreAlloc> mxCallBackIndices;

    //using PROERTY_CALLBACKS = std::unordered_multimap<PropertyName, PROPERTY_EVENT_FUNCTOR_PTR>;
    //PROERTY_CALLBACKS mxPropertyCBs;

    AFGUID mxSelf;
};
