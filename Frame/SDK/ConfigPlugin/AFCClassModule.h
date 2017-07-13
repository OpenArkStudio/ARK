// -------------------------------------------------------------------------
//    @FileName         :    AFCClassModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCClassModule
//
// -------------------------------------------------------------------------

#ifndef AFC_LOGICCLASS_MODULE_H
#define AFC_LOGICCLASS_MODULE_H

#include <string>
#include <map>
#include <iostream>
#include "AFCElementModule.h"
#include "RapidXML/rapidxml.hpp"
#include "SDK/Core/AFMap.h"
#include "SDK/Core/AFList.h"
#include "SDK/Core/AFCDataList.h"
#include "SDK/Core/AFCRecord.h"
#include "SDK/Core/AFList.h"
#include "SDK/Core/AFCPropertyMgr.h"
#include "SDK/Core/AFCRecordManager.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIPluginManager.h"

class AFCClass : public AFIClass
{
public:

    AFCClass(const std::string& strClassName)
    {
        m_pParentClass = NULL;
        mstrClassName = strClassName;

        m_pPropertyManager = NF_SHARE_PTR<AFIPropertyMgr>(NF_NEW AFCPropertyMgr(NULL_GUID));
        m_pRecordManager = NF_SHARE_PTR<AFIRecordManager>(NF_NEW AFCRecordManager(NULL_GUID));
    }

    virtual ~AFCClass()
    {
        ClearAll();
    }

    virtual NF_SHARE_PTR<AFIPropertyMgr> GetPropertyManager()
    {
        return m_pPropertyManager;
    }

    virtual NF_SHARE_PTR<AFIRecordManager> GetRecordManager()
    {
        return m_pRecordManager;
    }

    virtual bool AddClassCallBack(const CLASS_EVENT_FUNCTOR_PTR& cb)
    {
        return mxClassEventInfo.Add(cb);
    }

    virtual bool DoEvent(const AFGUID& objectID, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& valueList)
    {
        CLASS_EVENT_FUNCTOR_PTR cb;
        bool bRet = mxClassEventInfo.First(cb);
        while(bRet)
        {
            (*cb)(objectID, mstrClassName, eClassEvent,  valueList);

            bRet = mxClassEventInfo.Next(cb);
        }

        return true;
    }

    void SetParent(NF_SHARE_PTR<AFIClass> pClass)
    {
        m_pParentClass = pClass;
    }

    NF_SHARE_PTR<AFIClass> GetParent()
    {
        return m_pParentClass;
    }

    void SetTypeName(const char* strType)
    {
        mstrType = strType;
    }

    const std::string& GetTypeName()
    {
        return mstrType;
    }

    const std::string& GetClassName()
    {
        return mstrClassName;
    }

    const bool AddConfigName(std::string& strConfigName)
    {
        mxConfigList.Add(strConfigName);

        return true;
    }

    NFList<std::string>& GetConfigNameList()
    {
        return mxConfigList;
    }

    void SetInstancePath(const std::string& strPath)
    {
        mstrClassInstancePath = strPath;
    }

    const std::string& GetInstancePath()
    {
        return mstrClassInstancePath;
    }

private:
    NF_SHARE_PTR<AFIPropertyMgr> m_pPropertyManager;
    NF_SHARE_PTR<AFIRecordManager> m_pRecordManager;

    NF_SHARE_PTR<AFIClass> m_pParentClass;
    std::string mstrType;
    std::string mstrClassName;
    std::string mstrClassInstancePath;

    NFList<std::string> mxConfigList;

    NFList<CLASS_EVENT_FUNCTOR_PTR> mxClassEventInfo;
};

class AFCClassModule
    : public AFIClassModule
{
public:
    AFCClassModule(AFIPluginManager* p);
    virtual ~AFCClassModule();

    virtual bool Init();
    virtual bool Shut();

    virtual bool Load();
    virtual bool Save();
    virtual bool Clear();

    virtual bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb);
    virtual bool DoEvent(const AFGUID& objectID, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& valueList);

    virtual NF_SHARE_PTR<AFIPropertyMgr> GetClassPropertyManager(const std::string& strClassName);
    virtual NF_SHARE_PTR<AFIRecordManager> GetClassRecordManager(const std::string& strClassName);
    //virtual NF_SHARE_PTR<AFIComponentManager> GetClassComponentManager(const std::string& strClassName);

    virtual bool AddClass(const std::string& strClassName, const std::string& strParentName);

protected:
    virtual int ComputerType(const char* pstrTypeName, AFIData& var);
    virtual bool AddPropertys(rapidxml::xml_node<>* pPropertyRootNode, NF_SHARE_PTR<AFIClass> pClass);
    virtual bool AddRecords(rapidxml::xml_node<>* pRecordRootNode, NF_SHARE_PTR<AFIClass> pClass);
    virtual bool AddComponents(rapidxml::xml_node<>* pRecordRootNode, NF_SHARE_PTR<AFIClass> pClass);
    virtual bool AddClassInclude(const char* pstrClassFilePath, NF_SHARE_PTR<AFIClass> pClass);
    virtual bool AddClass(const char* pstrClassFilePath, NF_SHARE_PTR<AFIClass> pClass);

    //加载此节点的类
    virtual bool Load(rapidxml::xml_node<>* attrNode, NF_SHARE_PTR<AFIClass> pParentClass);

protected:
    AFIElementModule* m_pElementModule;

    std::string msConfigFileName;
};

#endif
