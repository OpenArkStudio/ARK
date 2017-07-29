#pragma once

#include "AFDefine.h"
#include "AFCDataList.h"
#include "AFString.hpp"
#include "AFArrayPod.hpp"
#include "AFStringPod.hpp"
#include "AFCData.h"
#include "BitValue.hpp"
using namespace ArkFrame;

struct AFProperty
{
    enum PROP_FEATURE
    {
        PF_PUBLIC = 0, //send to others
        PF_PRIVATE = 1, //send to self
        PF_REAL_TIME = 2, //send real-time when changed
        PF_SAVE = 3, //if need save to database
    };

    PropertyName name;  //属性名，最大16个字符
    AFXData prop_value; //属性值
    int8_t feature;     //特性

    bool IsPublic() const
    {
        return BitValue<int8_t>::HaveBitValue(feature, PF_PUBLIC);
    }

    bool IsPrivate() const
    {
        return BitValue<int8_t>::HaveBitValue(feature, PF_PRIVATE);
    }

    bool IsRealTime() const
    {
        return BitValue<int8_t>::HaveBitValue(feature, PF_REAL_TIME);
    }

    bool IsSave() const
    {
        return BitValue<int8_t>::HaveBitValue(feature, PF_SAVE);
    }

    bool Changed() const
    {
        return !(prop_value.IsNullValue());
    }

    const PropertyName& GetName()
    {
        return name;
    }

    const AFIData& GetValue()
    {
        return prop_value;
    }

    int GetType()
    {
        return prop_value.GetType();
    }

    const int8_t GetFeature()
    {
        return feature;
    }
};

class AFIPropertyMgr
{
public:

    virtual ~AFIPropertyMgr() {}
    virtual void Clear() = 0;
    virtual const AFGUID& Self() const = 0;

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;

    virtual size_t GetPropertyCount() = 0;
    virtual AFProperty* GetPropertyByIndex(size_t index) = 0;
    virtual AFProperty* GetProperty(const char* name) = 0;
    virtual bool AddProperty(const char* name, const AFIData& value, const int8_t feature) = 0;
    virtual bool SetProperty(const char* name, const AFIData& value) = 0;

    virtual bool SetPropertyBool(const char* name, const bool value) = 0;
    virtual bool SetPropertyInt(const char* name, const int32_t value) = 0;
    virtual bool SetPropertyInt64(const char* name, const int64_t value) = 0;
    virtual bool SetPropertyFloat(const char* name, const float value) = 0;
    virtual bool SetPropertyDouble(const char* name, const double value) = 0;
    virtual bool SetPropertyString(const char* name, const std::string& value) = 0;
    virtual bool SetPropertyObject(const char* name, const AFGUID& value) = 0;

    virtual bool GetPropertyBool(const char* name) = 0;
    virtual int32_t GetPropertyInt(const char* name) = 0;
    virtual int64_t GetPropertyInt64(const char* name) = 0;
    virtual float GetPropertyFloat(const char* name) = 0;
    virtual double GetPropertyDouble(const char* name) = 0;
    virtual const char* GetPropertyString(const char* name) = 0;
    virtual const AFGUID& GetPropertyObject(const char* name) = 0;
};