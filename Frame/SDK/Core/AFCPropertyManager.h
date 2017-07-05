// -------------------------------------------------------------------------
//    @FileName         :    AFCPropertyManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    AFCPropertyManager
//
// -------------------------------------------------------------------------

#pragma once

#include "AFDefine.h"
#include "AFMap.h"
#include "AFCDataList.h"
#include "AFIPropertyManager.h"
#include "AFCoreDef.hpp"
#include "AFArrayPod.hpp"
#include "AFStringPod.hpp"

class AFCPropertyManager : public AFIPropertyManager
{
public:
    
    AFCPropertyManager(const AFGUID& self)
    {
        mSelf = self;
    };

    virtual ~AFCPropertyManager();

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb);

    virtual NF_SHARE_PTR<AFIProperty> AddProperty(const AFGUID& self, NF_SHARE_PTR<AFIProperty> pProperty);
    virtual NF_SHARE_PTR<AFIProperty> AddProperty(const AFGUID& self, const std::string& strPropertyName, const int varType);
    virtual const AFGUID& Self();

    virtual bool SetProperty(const std::string& strPropertyName, const AFIData& TData);

    virtual bool SetPropertyBool(const std::string& strPropertyName, const bool value);
    virtual bool SetPropertyInt(const std::string& strPropertyName, const int32_t value);
    virtual bool SetPropertyInt64(const std::string& strPropertyName, const int64_t value);
    virtual bool SetPropertyFloat(const std::string& strPropertyName, const float value);
    virtual bool SetPropertyDouble(const std::string& strPropertyName, const double value);
    virtual bool SetPropertyString(const std::string& strPropertyName, const std::string& value);
    virtual bool SetPropertyObject(const std::string& strPropertyName, const AFGUID& value);

    virtual bool GetPropertyBool(const std::string& strPropertyName);
    virtual int32_t GetPropertyInt(const std::string& strPropertyName);
    virtual int64_t GetPropertyInt64(const std::string& strPropertyName);
    virtual float GetPropertyFloat(const std::string& strPropertyName);
    virtual double GetPropertyDouble(const std::string& strPropertyName);
    virtual const std::string& GetPropertyString(const std::string& strPropertyName);
    virtual const AFGUID& GetPropertyObject(const std::string& strPropertyName);

private:
    AFGUID mSelf;
};
