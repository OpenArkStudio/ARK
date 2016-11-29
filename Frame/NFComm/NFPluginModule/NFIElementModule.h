// -------------------------------------------------------------------------
//    @FileName         :    NFIElementModule.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-15
//    @Module           :    NFIElementModule
//
// -------------------------------------------------------------------------

#ifndef NFI_ELEMENTINFO_MODULE_H
#define NFI_ELEMENTINFO_MODULE_H

#include <iostream>
#include "NFIModule.h"
#include "NFComm/NFCore/NFIDataList.h"
#include "NFComm/NFCore/NFIPropertyManager.h"
#include "NFComm/NFCore/NFIRecordManager.h"
#include "NFComm/NFCore/NFIComponentManager.h"

class NFIElementModule
    : public NFIModule
{
public:

    /**
     * @fn  virtual bool Load() = 0;
     *
     * @brief   Loads all Instance.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Load() = 0;

    /**
     * @fn  virtual bool Save() = 0;
     *
     * @brief   Saves all Instance.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Save() = 0;

    /**
     * @fn  virtual bool Clear() = 0;
     *
     * @brief   Clears this object to its blank/initial state.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Clear() = 0;

    /**
     * @fn  virtual bool ExistElement(const std::string& strConfigName) = 0;
     *
     * @brief   Exist element.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strConfigName   Name of the configuration.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool ExistElement(const std::string& strConfigName) = 0;

    /**
     * @fn  virtual bool ExistElement(const std::string& strClassName, const std::string& strConfigName) = 0;
     *
     * @brief   Exist element.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strClassName    Name of the class.
     * @param   strConfigName   Name of the configuration.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool ExistElement(const std::string& strClassName, const std::string& strConfigName) = 0;

    /**
     * @fn  virtual std::shared_ptr<NFIPropertyManager> GetPropertyManager(const std::string& strConfigName) = 0;
     *
     * @brief   Gets property manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strConfigName   Name of the configuration.
     *
     * @return  The property manager.
     */

    virtual std::shared_ptr<NFIPropertyManager> GetPropertyManager(const std::string& strConfigName) = 0;

    /**
     * @fn  virtual std::shared_ptr<NFIRecordManager> GetRecordManager(const std::string& strConfigName) = 0;
     *
     * @brief   Gets record manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strConfigName   Name of the configuration.
     *
     * @return  The record manager.
     */

    virtual std::shared_ptr<NFIRecordManager> GetRecordManager(const std::string& strConfigName) = 0;

    /**
     * @fn  virtual std::shared_ptr<NFIComponentManager> GetComponentManager(const std::string& strConfigName) = 0;
     *
     * @brief   Gets component manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strConfigName   Name of the configuration.
     *
     * @return  The component manager.
     */

    virtual std::shared_ptr<NFIComponentManager> GetComponentManager(const std::string& strConfigName) = 0;

    /**
     * @fn  virtual NFINT64 GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strConfigName   Name of the configuration.
     * @param   strPropertyName Name of the property.
     *
     * @return  The property int.
     */

    virtual NFINT64 GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual double GetPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property float.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strConfigName   Name of the configuration.
     * @param   strPropertyName Name of the property.
     *
     * @return  The property float.
     */

    virtual double GetPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual const std::string& GetPropertyString(const std::string& strConfigName, const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strConfigName   Name of the configuration.
     * @param   strPropertyName Name of the property.
     *
     * @return  The property string.
     */

    virtual const std::string& GetPropertyString(const std::string& strConfigName, const std::string& strPropertyName) = 0;

protected:
private:
};
#endif