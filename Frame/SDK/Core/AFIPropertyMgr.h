#pragma once

#include "AFDefine.h"
#include "AFCDataList.h"
#include "AFString.hpp"
#include "AFArrayPod.hpp"
#include "AFStringPod.hpp"
#include "AFCData.h"
using namespace ArkFrame;

class AFIPropertyMgr
{
public:

    virtual ~AFIPropertyMgr() {}
    virtual void Clear() = 0;
    virtual const AFGUID& Self() const = 0;

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool AddProperty(const char* name, const AFIData& value, bool bPublic, bool bPrivate, bool bSave, bool bRealTime) = 0;

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