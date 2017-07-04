// -------------------------------------------------------------------------
//    @FileName         :    NFCProperty.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    NFCProperty
//
// -------------------------------------------------------------------------

#pragma once

#include "NFDefine.h"
#include "NFMap.h"
#include "NFList.h"
#include "AFCDataList.h"
#include "NFIProperty.h"
#include "AFIData.h"
#include <vector>

using namespace ArkFrame;

class NFCProperty : public NFIProperty
{
public:
    NFCProperty() = delete;

    NFCProperty(const AFGUID& self, const std::string& strPropertyName, const int varType);

    virtual ~NFCProperty();

    virtual void SetValue(const AFIData& TData);
    virtual void SetValue(const NFIProperty* pProperty);

    virtual bool SetBool(const bool value);
    virtual bool SetInt(const int32_t value);
    virtual bool SetInt64(const int32_t value);
    virtual bool SetFloat(const float value);
    virtual bool SetDouble(const double value);
    virtual bool SetString(const std::string& value);
    virtual bool SetObject(const AFGUID& value);

    virtual bool GetBool() const;
    virtual int32_t GetInt() const;
    virtual int64_t GetInt64() const;
    virtual float GetFloat() const;
    virtual double GetDouble() const;
    virtual const std::string& GetString() const;
    virtual const AFGUID& GetObject() const;

    virtual const int GetType() const;
    virtual const bool GeUsed() const;
    virtual const std::string& GetKey() const;

    virtual const bool GetSave() const;
    virtual const bool GetPublic() const;
    virtual const bool GetPrivate() const;
    virtual const bool GetCache() const;
    virtual const std::string& GetRelationValue() const;

    virtual void SetSave(bool bSave);
    virtual void SetPublic(bool bPublic);
    virtual void SetPrivate(bool bPrivate);
    virtual void SetCache(bool bCache);
    virtual void SetRelationValue(const std::string& strRelation);

    virtual const AFIData& GetValue() const;

    virtual const NF_SHARE_PTR<NFList<std::string>> GetEmbeddedList() const;
    virtual const NF_SHARE_PTR<NFMapEx<std::string, std::string>> GetEmbeddedMap() const;

    virtual bool Changed() const;
    virtual std::string ToString();
    virtual bool FromString(const std::string& strData);

    virtual bool DeSerialization();
    virtual void RegisterCallback(const PROPERTY_EVENT_FUNCTOR_PTR& cb);

private:
    int OnEventHandler(const AFIData& oldVar, const AFIData& newVar);

private:

    /**
     * @typedef std::vector<PROPERTY_EVENT_FUNCTOR_PTR> TPROPERTYCALLBACKEX
     *
     * @brief   Defines an alias representing the tpropertycallbackex.
     */

    typedef std::vector<PROPERTY_EVENT_FUNCTOR_PTR> TPROPERTYCALLBACKEX;
    /** @brief   The mt property callback. */
    TPROPERTYCALLBACKEX mtPropertyCallback;

    /** @brief   The self. */
    AFGUID mSelf;
    /** @brief   可以想办法与基本类型共用. */
    std::string msPropertyName;
    /** @brief   The mstr relation value. */
    std::string mstrRelationValue;
    /** @brief   只有在不存在指针的时候才用这个判断类型--为节约内存. */
    AF_DATA_TYPE eType;

    /** @brief   Information describing the mx. */
    NF_SHARE_PTR<AFIData> mxData;
    /** @brief   The mx embedded map. */
    NF_SHARE_PTR<NFMapEx<std::string, std::string>> mxEmbeddedMap;
    /** @brief   List of mx embedded. */
    NF_SHARE_PTR<NFList<std::string>> mxEmbeddedList;

    /** @brief   True to megabytes public. */
    bool mbPublic;
    /** @brief   True to megabytes private. */
    bool mbPrivate;
    /** @brief   True to megabytes save. */
    bool mbSave;
    /** @brief   True to megabytes cache. */
    bool mbCache;
};
