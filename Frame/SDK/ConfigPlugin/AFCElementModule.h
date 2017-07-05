// -------------------------------------------------------------------------
//    @FileName			:    AFCElementModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCElementModule
//
// -------------------------------------------------------------------------

#ifndef AFC_ELEMENT_MODULE_H
#define AFC_ELEMENT_MODULE_H

#include <map>
#include <string>
#include <iostream>
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_iterators.hpp"
#include "RapidXML/rapidxml_print.hpp"
#include "RapidXML/rapidxml_utils.hpp"
#include "SDK/Core/AFMap.h"
#include "SDK/Core/AFList.h"
#include "SDK/Core/AFCDataList.h"
#include "SDK/Core/AFCRecord.h"
#include "SDK/Core/AFCPropertyManager.h"
#include "SDK/Core/AFCRecordManager.h"
//#include "SDK/Core/AFCComponentManager.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIClassModule.h"

class AFCClass;

/**
 * @class   ElementConfigInfo
 *
 * @brief   Information about the element configuration.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class ElementConfigInfo
{
public:
    ElementConfigInfo()
    {
        m_pPropertyManager = NF_SHARE_PTR<AFIPropertyManager>(NF_NEW AFCPropertyManager(NULL_GUID));
        m_pRecordManager = NF_SHARE_PTR<AFIRecordManager>(NF_NEW AFCRecordManager(NULL_GUID));
        //m_pComponentManager = NF_SHARE_PTR<AFIComponentManager>(NF_NEW AFCComponentManager(NULL_GUID));
    }

    virtual ~ElementConfigInfo()
    {
    }

    NF_SHARE_PTR<AFIPropertyManager> GetPropertyManager()
    {
        return m_pPropertyManager;
    }

    NF_SHARE_PTR<AFIRecordManager> GetRecordManager()
    {
        return m_pRecordManager;
    }

    //NF_SHARE_PTR<AFIComponentManager> GetComponentManager()
    //{
    //    return m_pComponentManager;
    //}
protected:

    //std::string mstrConfigID;

    NF_SHARE_PTR<AFIPropertyManager> m_pPropertyManager;
    NF_SHARE_PTR<AFIRecordManager> m_pRecordManager;
    //NF_SHARE_PTR<AFIComponentManager> m_pComponentManager;
};

/**
 * @class   AFCElementModule
 *
 * @brief   A nfc element module.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class AFCElementModule
    : public AFIElementModule,
      AFMapEx<std::string, ElementConfigInfo>
{
public:
    AFCElementModule(AFIPluginManager* p);
    virtual ~AFCElementModule();

    virtual bool Init();
    virtual bool Shut();

    virtual bool AfterInit();
    virtual bool BeforeShut();
    virtual bool Execute();

    virtual bool Load();
    virtual bool Save();
    virtual bool Clear();

    virtual bool ExistElement(const std::string& strConfigName);
    virtual bool ExistElement(const std::string& strClassName, const std::string& strConfigName);

    virtual NF_SHARE_PTR<AFIPropertyManager> GetPropertyManager(const std::string& strConfigName);
    virtual NF_SHARE_PTR<AFIRecordManager> GetRecordManager(const std::string& strConfigName);
    //virtual NF_SHARE_PTR<AFIComponentManager> GetComponentManager(const std::string& strConfigName);

    virtual AFINT64 GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName);
    virtual double GetPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName);
    virtual const std::string& GetPropertyString(const std::string& strConfigName, const std::string& strPropertyName);

protected:
    virtual NF_SHARE_PTR<AFIProperty> GetProperty(const std::string& strConfigName, const std::string& strPropertyName);

    virtual bool Load(rapidxml::xml_node<>* attrNode, NF_SHARE_PTR<AFIClass> pLogicClass);

    virtual bool LegalNumber(const char* str);
protected:
    AFIClassModule* m_pClassModule;
    bool mbLoaded;
};

#endif
