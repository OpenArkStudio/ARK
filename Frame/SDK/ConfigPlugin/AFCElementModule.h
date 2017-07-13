// -------------------------------------------------------------------------
//    @FileName			:    AFCElementModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCElementModule
//
// -------------------------------------------------------------------------

#pragma once

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
#include "SDK/Core/AFCPropertyMgr.h"
#include "SDK/Core/AFCRecordManager.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIClassModule.h"

class AFCClass;

class ElementConfigInfo
{
public:
    ElementConfigInfo()
    {
        m_pPropertyManager = NF_SHARE_PTR<AFIPropertyMgr>(NF_NEW AFCPropertyMgr(NULL_GUID));
        m_pRecordManager = NF_SHARE_PTR<AFIRecordManager>(NF_NEW AFCRecordManager(NULL_GUID));
    }

    virtual ~ElementConfigInfo()
    {
    }

    NF_SHARE_PTR<AFIPropertyMgr> GetPropertyManager()
    {
        return m_pPropertyManager;
    }

    NF_SHARE_PTR<AFIRecordManager> GetRecordManager()
    {
        return m_pRecordManager;
    }

protected:
    NF_SHARE_PTR<AFIPropertyMgr> m_pPropertyManager;
    NF_SHARE_PTR<AFIRecordManager> m_pRecordManager;
};

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

    virtual NF_SHARE_PTR<AFIPropertyMgr> GetPropertyManager(const std::string& strConfigName);
    virtual NF_SHARE_PTR<AFIRecordManager> GetRecordManager(const std::string& strConfigName);

    virtual bool GetPropertyBool(const std::string& strConfigName, const std::string& strPropertyName);
    virtual int32_t GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName);
    virtual int64_t GetPropertyInt64(const std::string& strConfigName, const std::string& strPropertyName);
    virtual float GetPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName);
    virtual double GetPropertyDouble(const std::string& strConfigName, const std::string& strPropertyName);
    virtual const std::string& GetPropertyString(const std::string& strConfigName, const std::string& strPropertyName);

protected:
    virtual AFProperty* GetProperty(const std::string& strConfigName, const std::string& strPropertyName);

    virtual bool Load(rapidxml::xml_node<>* attrNode, NF_SHARE_PTR<AFIClass> pLogicClass);

    virtual bool LegalNumber(const char* str);
protected:
    AFIClassModule* m_pClassModule;
    bool mbLoaded;
};
