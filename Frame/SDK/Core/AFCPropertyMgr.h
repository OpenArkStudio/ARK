// -------------------------------------------------------------------------
//    @FileName         :    AFCPropertyMgr.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    AFCPropertyMgr
//
// -------------------------------------------------------------------------

#pragma once

#include <unordered_map>
#include "AFIPropertyMgr.h"
#include "Noncopyable.hpp"
#include "BitValue.hpp"

struct AFProperty
{
    enum PROP_FEATURE
    {
        PF_PUBLIC   = 0, //send to others
        PF_PRIVATE  = 1, //send to self
        PF_REAL_TIME= 2, //send real-time when changed
        PF_SAVE     = 3, //if need save to database
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
};

class AFCPropertyMgr : public AFIPropertyMgr, noncopyable
{
public:
    AFCPropertyMgr() = delete;

    AFCPropertyMgr(const AFGUID& self);
    virtual ~AFCPropertyMgr();
    virtual void Clear();

    virtual const AFGUID& Self() const;

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb);
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
    ArraryPod<AFProperty*, 1, CoreAlloc> mxPropertys;
    StringPod<char, size_t, StringTraits<char>, CoreAlloc> mxIndices;

    using PROERTY_CALLBACKS = std::unordered_multimap<PropertyName, PROPERTY_EVENT_FUNCTOR_PTR>;
    PROERTY_CALLBACKS mxPropertyCBs;

    AFGUID mxSelf;
};