// -------------------------------------------------------------------------
//    @FileName         :    NFCPropertyManager.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFCPropertyManager
//
// -------------------------------------------------------------------------

#ifndef NFC_PROPERTYMANAGER_H
#define NFC_PROPERTYMANAGER_H

#include "NFDefine.h"
#include "NFMap.h"
#include "NFIDataList.h"
#include "NFIPropertyManager.h"

/**
 * @class   NFCPropertyManager
 *
 * @brief   Manager for nfc properties.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class NFCPropertyManager : public NFIPropertyManager
{
public:

    /**
     * @fn  NFCPropertyManager::NFCPropertyManager(const NFGUID& self)
     *
     * @brief   Constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   self    The class instance that this method operates on.
     */

    NFCPropertyManager(const NFGUID& self)
    {
        mSelf = self;
    };

    /**
     * @fn  virtual NFCPropertyManager::~NFCPropertyManager();
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    virtual ~NFCPropertyManager();

    /**
     * @fn  virtual bool NFCPropertyManager::RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb);
     *
     * @brief   Registers the callback.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strProperty The property.
     * @param   cb          The cb.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb);

    /**
     * @fn  virtual NF_SHARE_PTR<NFIProperty> NFCPropertyManager::AddProperty(const NFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty);
     *
     * @brief   Adds a property to 'pProperty'.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   self        The class instance that this method operates on.
     * @param   pProperty   The property.
     *
     * @return  A NF_SHARE_PTR&lt;NFIProperty&gt;
     */

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty);

    /**
     * @fn  virtual NF_SHARE_PTR<NFIProperty> NFCPropertyManager::AddProperty(const NFGUID& self, const std::string& strPropertyName, const TDATA_TYPE varType);
     *
     * @brief   Adds a property.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   self            The class instance that this method operates on.
     * @param   strPropertyName Name of the property.
     * @param   varType         Type of the variable.
     *
     * @return  A NF_SHARE_PTR&lt;NFIProperty&gt;
     */

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, const std::string& strPropertyName, const TDATA_TYPE varType);

    /**
     * @fn  virtual const NFGUID& NFCPropertyManager::Self();
     *
     * @brief   Gets the self.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  A reference to a const NFGUID.
     */

    virtual const NFGUID& Self();

    /**
     * @fn  virtual bool NFCPropertyManager::SetProperty(const std::string& strPropertyName, const NFIDataList::TData& TData);
     *
     * @brief   Sets a property.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strPropertyName Name of the property.
     * @param   TData           The data.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetProperty(const std::string& strPropertyName, const NFIDataList::TData& TData);

    /**
     * @fn  virtual bool NFCPropertyManager::SetPropertyInt(const std::string& strPropertyName, const NFINT64 value);
     *
     * @brief   Sets property int.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyInt(const std::string& strPropertyName, const NFINT64 value);

    /**
     * @fn  virtual bool NFCPropertyManager::SetPropertyDouble(const std::string& strPropertyName, const double value);
     *
     * @brief   Sets property double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyDouble(const std::string& strPropertyName, const double value);

    /**
     * @fn  virtual bool NFCPropertyManager::SetPropertyString(const std::string& strPropertyName, const std::string& value);
     *
     * @brief   Sets property string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyString(const std::string& strPropertyName, const std::string& value);

    /**
     * @fn  virtual bool NFCPropertyManager::SetPropertyObject(const std::string& strPropertyName, const NFGUID& value);
     *
     * @brief   Sets property object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyObject(const std::string& strPropertyName, const NFGUID& value);

    /**
     * @fn  virtual bool NFCPropertyManager::SetPropertyPoint(const std::string& strPropertyName, const Point3D& value);
     *
     * @brief   Sets property point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyPoint(const std::string& strPropertyName, const Point3D& value);

    /**
     * @fn  virtual NFINT64 NFCPropertyManager::GetPropertyInt(const std::string& strPropertyName);
     *
     * @brief   Gets property int.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property int.
     */

    virtual NFINT64 GetPropertyInt(const std::string& strPropertyName);

    /**
     * @fn  virtual double NFCPropertyManager::GetPropertyDouble(const std::string& strPropertyName);
     *
     * @brief   Gets property double.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property double.
     */

    virtual double GetPropertyDouble(const std::string& strPropertyName);

    /**
     * @fn  virtual const std::string& NFCPropertyManager::GetPropertyString(const std::string& strPropertyName);
     *
     * @brief   Gets property string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property string.
     */

    virtual const std::string& GetPropertyString(const std::string& strPropertyName);

    /**
     * @fn  virtual const NFGUID& NFCPropertyManager::GetPropertyObject(const std::string& strPropertyName);
     *
     * @brief   Gets property object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property object.
     */

    virtual const NFGUID& GetPropertyObject(const std::string& strPropertyName);

    /**
     * @fn  virtual const Point3D& NFCPropertyManager::GetPropertyPoint(const std::string& strPropertyName);
     *
     * @brief   Gets property point.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property point.
     */

    virtual const Point3D& GetPropertyPoint(const std::string& strPropertyName);

private:
    /** @brief   The self. */
    NFGUID mSelf;
    /** @brief   The mx property index map. */
    std::map<std::string, int> mxPropertyIndexMap;
};


#endif
