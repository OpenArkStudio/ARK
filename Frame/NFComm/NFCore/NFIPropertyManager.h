// -------------------------------------------------------------------------
//    @FileName         :    NFIPropertyManager.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFIPropertyManager
//
// -------------------------------------------------------------------------

#ifndef NFI_PROPERTYMANAGER_H
#define NFI_PROPERTYMANAGER_H

#include "NFMap.h"
#include "NFIProperty.h"
#include <map>

/**
 * @class   NFIPropertyManager
 *
 * @brief   Manager for nfi properties.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFIPropertyManager
    : public NFMapEx<std::string, NFIProperty>
{
public:
    virtual ~NFIPropertyManager() {}

    /**
     * @fn  virtual bool NFIPropertyManager::RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
     *
     * @brief   Registers the callback.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strProperty The property.
     * @param   cb          The cb.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIProperty> NFIPropertyManager::AddProperty(const NFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty) = 0;
     *
     * @brief   Adds a property to 'pProperty'.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   self        The class instance that this method operates on.
     * @param   pProperty   The property.
     *
     * @return  A NF_SHARE_PTR&lt;NFIProperty&gt;
     */

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty) = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIProperty> NFIPropertyManager::AddProperty(const NFGUID& self, const std::string& strPropertyName, const TDATA_TYPE varType) = 0;
     *
     * @brief   Adds a property.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   self            The class instance that this method operates on.
     * @param   strPropertyName Name of the property.
     * @param   varType         Type of the variable.
     *
     * @return  A NF_SHARE_PTR&lt;NFIProperty&gt;
     */

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, const std::string& strPropertyName, const TDATA_TYPE varType) = 0;

    /**
     * @fn  virtual const NFGUID& NFIPropertyManager::Self() = 0;
     *
     * @brief   Gets the self.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A reference to a const NFGUID.
     */

    virtual const NFGUID& Self() = 0;

    /**
     * @fn  virtual bool NFIPropertyManager::SetProperty(const std::string& strPropertyName, const NFIDataList::TData& TData) = 0;
     *
     * @brief   ///////////////////////////////////////////////////////////////////////.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   TData           The data.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetProperty(const std::string& strPropertyName, const NFIDataList::TData& TData) = 0;

    /**
     * @fn  virtual bool NFIPropertyManager::SetPropertyInt(const std::string& strPropertyName, const NFINT64 value) = 0;
     *
     * @brief   Sets property int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyInt(const std::string& strPropertyName, const NFINT64 value) = 0;

    /**
     * @fn  virtual bool NFIPropertyManager::SetPropertyDouble(const std::string& strPropertyName, const double value) = 0;
     *
     * @brief   Sets property double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyDouble(const std::string& strPropertyName, const double value) = 0;

    /**
     * @fn  virtual bool NFIPropertyManager::SetPropertyString(const std::string& strPropertyName, const std::string& value) = 0;
     *
     * @brief   Sets property string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyString(const std::string& strPropertyName, const std::string& value) = 0;

    /**
     * @fn  virtual bool NFIPropertyManager::SetPropertyObject(const std::string& strPropertyName, const NFGUID& value) = 0;
     *
     * @brief   Sets property object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyObject(const std::string& strPropertyName, const NFGUID& value) = 0;

    /**
     * @fn  virtual bool NFIPropertyManager::SetPropertyPoint(const std::string& strPropertyName, const Point3D& value) = 0;
     *
     * @brief   Sets property point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyPoint(const std::string& strPropertyName, const Point3D& value) = 0;

    /**
     * @fn  virtual NFINT64 NFIPropertyManager::GetPropertyInt(const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property int.
     */

    virtual NFINT64 GetPropertyInt(const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual double NFIPropertyManager::GetPropertyDouble(const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property double.
     */

    virtual double GetPropertyDouble(const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual const std::string& NFIPropertyManager::GetPropertyString(const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property string.
     */

    virtual const std::string& GetPropertyString(const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual const NFGUID& NFIPropertyManager::GetPropertyObject(const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property object.
     */

    virtual const NFGUID& GetPropertyObject(const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual const Point3D& NFIPropertyManager::GetPropertyPoint(const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property point.
     */

    virtual const Point3D& GetPropertyPoint(const std::string& strPropertyName) = 0;

};

#endif
