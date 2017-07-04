// -------------------------------------------------------------------------
//    @FileName         :    NFIPropertyManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    NFIPropertyManager
//
// -------------------------------------------------------------------------

#pragma once

#include "NFMap.h"
#include "NFIProperty.h"
#include "AFIData.h"

using namespace ArkFrame;

class NFIPropertyManager
    : public NFMapEx<std::string, NFIProperty>
{
public:
    virtual ~NFIPropertyManager() {}

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const AFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty) = 0;
    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const AFGUID& self, const std::string& strPropertyName, const int varType) = 0;

    virtual const AFGUID& Self() = 0;

    virtual bool SetProperty(const std::string& strPropertyName, const AFIData& TData) = 0;

    virtual bool SetPropertyBool(const std::string& strPropertyName, const bool value) = 0;
    virtual bool SetPropertyInt(const std::string& strPropertyName, const int32_t value) = 0;
    virtual bool SetPropertyInt64(const std::string& strPropertyName, const int64_t value) = 0;
    virtual bool SetPropertyFloat(const std::string& strPropertyName, const float value) = 0;
    virtual bool SetPropertyDouble(const std::string& strPropertyName, const double value) = 0;
    virtual bool SetPropertyString(const std::string& strPropertyName, const std::string& value) = 0;
    virtual bool SetPropertyObject(const std::string& strPropertyName, const AFGUID& value) = 0;

    virtual bool GetPropertyBool(const std::string& strPropertyName) = 0;
    virtual int32_t GetPropertyInt(const std::string& strPropertyName) = 0;
    virtual int64_t GetPropertyInt64(const std::string& strPropertyName) = 0;
    virtual float GetPropertyFloat(const std::string& strPropertyName) = 0;
    virtual double GetPropertyDouble(const std::string& strPropertyName) = 0;
    virtual const std::string& GetPropertyString(const std::string& strPropertyName) = 0;
    virtual const AFGUID& GetPropertyObject(const std::string& strPropertyName) = 0;
};