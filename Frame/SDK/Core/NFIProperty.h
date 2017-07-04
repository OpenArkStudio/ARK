// -------------------------------------------------------------------------
//    @FileName         :    NFIProperty.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    NFIProperty
//
// -------------------------------------------------------------------------

#pragma once

#include "NFDefine.h"
#include "AFCDataList.h"
#include "NFList.h"
#include "AFCData.h"

using namespace ArkFrame;

class NFIProperty
{
public:
    virtual ~NFIProperty() {}

    virtual void SetValue(const AFIData& TData) = 0;
    virtual void SetValue(const NFIProperty* pProperty) = 0;

    virtual bool SetBool(const bool value) = 0;
    virtual bool SetInt(const int32_t value) = 0;
    virtual bool SetInt64(const int32_t value) = 0;
    virtual bool SetFloat(const float value) = 0;
    virtual bool SetDouble(const double value) = 0;
    virtual bool SetString(const std::string& value) = 0;
    virtual bool SetObject(const AFGUID& value) = 0;

    virtual bool GetBool() const = 0;
    virtual int32_t GetInt() const = 0;
    virtual int64_t GetInt64() const = 0;
    virtual float GetFloat() const = 0;
    virtual double GetDouble() const = 0;
    virtual const std::string& GetString() const = 0;
    virtual const AFGUID& GetObject() const = 0;
    
    virtual const int GetType() const = 0;
    virtual const bool GeUsed() const = 0;
    virtual const std::string& GetKey() const = 0;
    
    virtual const bool GetSave() const = 0;
    virtual const bool GetPublic() const = 0;
    virtual const bool GetPrivate() const = 0;
    virtual const bool GetCache() const = 0;
    virtual const std::string& GetRelationValue() const = 0;

    virtual void SetSave(bool bSave) = 0;
    virtual void SetPublic(bool bPublic) = 0;
    virtual void SetPrivate(bool bPrivate) = 0;
    virtual void SetCache(bool bCache) = 0;
    virtual void SetRelationValue(const std::string& strRelation) = 0;

    virtual const AFIData& GetValue() const = 0;

    virtual const NF_SHARE_PTR<NFList<std::string>> GetEmbeddedList() const = 0;
    virtual const NF_SHARE_PTR<NFMapEx<std::string, std::string>> GetEmbeddedMap() const = 0;

    virtual bool Changed() const = 0;
    virtual std::string ToString() = 0;
    virtual bool FromString(const std::string& strData) = 0;
    virtual bool DeSerialization() = 0;
    
    virtual void RegisterCallback(const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
};