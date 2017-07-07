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

struct AFProperty
{
    PropertyName name; //属性名，最大16个字符
    AFXData prop_value;    //属性值

    //TODO:暂时还没用上
    bool bPublic;
    bool bPrivate;
    bool bSave;
    bool bRealtime;
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
    virtual bool AddProperty(const char* name, const AFIData& value, bool bPublic, bool bPrivate, bool bSave, bool bRealTime);

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