// -------------------------------------------------------------------------
//    @FileName         :    AFIElementModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFIElementModule
//
// -------------------------------------------------------------------------

#ifndef AFI_ELEMENTINFO_MODULE_H
#define AFI_ELEMENTINFO_MODULE_H

#include <iostream>
#include "AFIModule.h"
#include "SDK/Core/AFCDataList.h"
#include "SDK/Core/AFIPropertyManager.h"
#include "SDK/Core/AFIRecordManager.h"
//#include "SDK/Core/AFIComponentManager.h"

class AFIElementModule
    : public AFIModule
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
     * @fn  virtual std::shared_ptr<AFIPropertyManager> GetPropertyManager(const std::string& strConfigName) = 0;
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

    virtual std::shared_ptr<AFIPropertyManager> GetPropertyManager(const std::string& strConfigName) = 0;

    /**
     * @fn  virtual std::shared_ptr<AFIRecordManager> GetRecordManager(const std::string& strConfigName) = 0;
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

    virtual std::shared_ptr<AFIRecordManager> GetRecordManager(const std::string& strConfigName) = 0;

    /**
     * @fn  virtual std::shared_ptr<AFIComponentManager> GetComponentManager(const std::string& strConfigName) = 0;
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

    //virtual std::shared_ptr<AFIComponentManager> GetComponentManager(const std::string& strConfigName) = 0;

    /**
     * @fn  virtual AFINT64 GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName) = 0;
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

    virtual AFINT64 GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName) = 0;

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
};
#endif