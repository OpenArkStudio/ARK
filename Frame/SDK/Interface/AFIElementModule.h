// -------------------------------------------------------------------------
//    @FileName         :    AFIElementModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFIElementModule
//
// -------------------------------------------------------------------------

#pragma once

#include <iostream>
#include "AFIModule.h"
#include "SDK/Core/AFCDataList.h"
#include "SDK/Core/AFIPropertyMgr.h"
#include "SDK/Core/AFIRecordManager.h"

class AFIElementModule
    : public AFIModule
{
public:
    virtual bool Load() = 0;
    virtual bool Save() = 0;
    virtual bool Clear() = 0;
    virtual bool ExistElement(const std::string& strConfigName) = 0;
    virtual bool ExistElement(const std::string& strClassName, const std::string& strConfigName) = 0;

    virtual std::shared_ptr<AFIPropertyMgr> GetPropertyManager(const std::string& strConfigName) = 0;

    virtual std::shared_ptr<AFIRecordManager> GetRecordManager(const std::string& strConfigName) = 0;

    virtual bool GetPropertyBool(const std::string& strConfigName, const std::string& strPropertyName) = 0;
    virtual int32_t GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName) = 0;
    virtual int64_t GetPropertyInt64(const std::string& strConfigName, const std::string& strPropertyName) = 0;
    virtual float GetPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName) = 0;
    virtual double GetPropertyDouble(const std::string& strConfigName, const std::string& strPropertyName) = 0;
    virtual const std::string& GetPropertyString(const std::string& strConfigName, const std::string& strPropertyName) = 0;
};