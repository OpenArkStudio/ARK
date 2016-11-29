// -------------------------------------------------------------------------
//    @FileName         :    NFIProperty.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFIProperty
//
// -------------------------------------------------------------------------

#ifndef NFI_PROPERTY_H
#define NFI_PROPERTY_H

#include "NFDefine.h"
#include "NFIDataList.h"
#include "NFList.h"

/**
 * @class   NFIProperty
 *
 * @brief   A nfi property.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFIProperty
{
public:

    /**
     * @fn  virtual NFIProperty::~NFIProperty()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual ~NFIProperty() {}

    /**
     * @fn  virtual void NFIProperty::SetValue(const NFIDataList::TData& TData) = 0;
     *
     * @brief   Sets a value.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   TData   The data.
     */

    virtual void SetValue(const NFIDataList::TData& TData) = 0;

    /**
     * @fn  virtual void NFIProperty::SetValue(const NFIProperty* pProperty) = 0;
     *
     * @brief   Sets a value.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   pProperty   The property.
     */

    virtual void SetValue(const NFIProperty* pProperty) = 0;

    /**
     * @fn  virtual bool NFIProperty::SetInt(const NFINT64 value) = 0;
     *
     * @brief   Sets an int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetInt(const NFINT64 value) = 0;

    /**
     * @fn  virtual bool NFIProperty::SetDouble(const double value) = 0;
     *
     * @brief   Sets a double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetDouble(const double value) = 0;

    /**
     * @fn  virtual bool NFIProperty::SetString(const std::string& value) = 0;
     *
     * @brief   Sets a string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetString(const std::string& value) = 0;

    /**
     * @fn  virtual bool NFIProperty::SetObject(const NFGUID& value) = 0;
     *
     * @brief   Sets an object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetObject(const NFGUID& value) = 0;

    /**
     * @fn  virtual bool NFIProperty::SetPoint(const Point3D& value) = 0;
     *
     * @brief   Sets a point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPoint(const Point3D& value) = 0;

    /**
     * @fn  virtual NFINT64 NFIProperty::GetInt() const = 0;
     *
     * @brief   Gets the int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The int.
     */

    virtual NFINT64 GetInt() const = 0;

    /**
     * @fn  virtual double NFIProperty::GetDouble() const = 0;
     *
     * @brief   Gets the double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The double.
     */

    virtual double GetDouble() const = 0;

    /**
     * @fn  virtual const std::string& NFIProperty::GetString() const = 0;
     *
     * @brief   Gets the string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The string.
     */

    virtual const std::string& GetString() const = 0;

    /**
     * @fn  virtual const NFGUID& NFIProperty::GetObject() const = 0;
     *
     * @brief   Gets the object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The object.
     */

    virtual const NFGUID& GetObject() const = 0;

    /**
     * @fn  virtual const Point3D& NFIProperty::GetPoint() const = 0;
     *
     * @brief   Gets the point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The point.
     */

    virtual const Point3D& GetPoint() const = 0;

    /**
     * @fn  virtual const TDATA_TYPE NFIProperty::GetType() const = 0;
     *
     * @brief   Gets the type.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The type.
     */

    virtual const TDATA_TYPE GetType() const = 0;

    /**
     * @fn  virtual const bool NFIProperty::GeUsed() const = 0;
     *
     * @brief   Ge used.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A const bool.
     */

    virtual const bool GeUsed() const = 0;

    /**
     * @fn  virtual const std::string& NFIProperty::GetKey() const = 0;
     *
     * @brief   Gets the key.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The key.
     */

    virtual const std::string& GetKey() const = 0;

    /**
     * @fn  virtual const bool NFIProperty::GetSave() const = 0;
     *
     * @brief   Gets the save.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The save.
     */

    virtual const bool GetSave() const = 0;

    /**
     * @fn  virtual const bool NFIProperty::GetPublic() const = 0;
     *
     * @brief   Gets the public.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The public.
     */

    virtual const bool GetPublic() const = 0;

    /**
     * @fn  virtual const bool NFIProperty::GetPrivate() const = 0;
     *
     * @brief   Gets the private.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The private.
     */

    virtual const bool GetPrivate() const = 0;

    /**
     * @fn  virtual const bool NFIProperty::GetCache() const = 0;
     *
     * @brief   Gets the cache.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The cache.
     */

    virtual const bool GetCache() const = 0;

    /**
     * @fn  virtual const std::string& NFIProperty::GetRelationValue() const = 0;
     *
     * @brief   Gets relation value.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The relation value.
     */

    virtual const std::string& GetRelationValue() const = 0;

    /**
     * @fn  virtual void NFIProperty::SetSave(bool bSave) = 0;
     *
     * @brief   Sets a save.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   bSave   True to save.
     */

    virtual void SetSave(bool bSave) = 0;

    /**
     * @fn  virtual void NFIProperty::SetPublic(bool bPublic) = 0;
     *
     * @brief   Sets a public.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   bPublic True to public.
     */

    virtual void SetPublic(bool bPublic) = 0;

    /**
     * @fn  virtual void NFIProperty::SetPrivate(bool bPrivate) = 0;
     *
     * @brief   Sets a private.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   bPrivate    True to private.
     */

    virtual void SetPrivate(bool bPrivate) = 0;

    /**
     * @fn  virtual void NFIProperty::SetCache(bool bCache) = 0;
     *
     * @brief   Sets a cache.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   bCache  True to cache.
     */

    virtual void SetCache(bool bCache) = 0;

    /**
     * @fn  virtual void NFIProperty::SetRelationValue(const std::string& strRelation) = 0;
     *
     * @brief   Sets relation value.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRelation The relation.
     */

    virtual void SetRelationValue(const std::string& strRelation) = 0;

    /**
     * @fn  virtual const NFIDataList::TData& NFIProperty::GetValue() const = 0;
     *
     * @brief   Gets the value.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The value.
     */

    virtual const NFIDataList::TData& GetValue() const = 0;

    /**
     * @fn  virtual const NF_SHARE_PTR<NFList<std::string>> NFIProperty::GetEmbeddedList() const = 0;
     *
     * @brief   Gets embedded list.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The embedded list.
     */

    virtual const NF_SHARE_PTR<NFList<std::string>> GetEmbeddedList() const = 0;

    /**
     * @fn  virtual const NF_SHARE_PTR<NFMapEx<std::string, std::string>> NFIProperty::GetEmbeddedMap() const = 0;
     *
     * @brief   Gets embedded map.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The embedded map.
     */

    virtual const NF_SHARE_PTR<NFMapEx<std::string, std::string>> GetEmbeddedMap() const = 0;

    /**
     * @fn  virtual bool NFIProperty::Changed() const = 0;
     *
     * @brief   Changed this object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Changed() const = 0;

    /**
     * @fn  virtual std::string NFIProperty::ToString() = 0;
     *
     * @brief   Convert this object into a string representation.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A std::string that represents this object.
     */

    virtual std::string ToString() = 0;

    /**
     * @fn  virtual bool NFIProperty::FromString(const std::string& strData) = 0;
     *
     * @brief   Initializes this object from the given from string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strData The data.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool FromString(const std::string& strData) = 0;

    /**
     * @fn  virtual bool NFIProperty::DeSerialization() = 0;
     *
     * @brief   Determines if we can de serialization.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool DeSerialization() = 0;

    /**
     * @fn  virtual void NFIProperty::RegisterCallback(const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
     *
     * @brief   Registers the callback described by cb.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   cb  The cb.
     */

    virtual void RegisterCallback(const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
};

#endif
