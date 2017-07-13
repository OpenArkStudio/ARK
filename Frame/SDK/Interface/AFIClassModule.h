// -------------------------------------------------------------------------
//    @FileName         :    AFIClassModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFIClassModule
//
// -------------------------------------------------------------------------

#pragma once

#include <iostream>
#include "AFIModule.h"
#include "SDK/Core/AFIPropertyMgr.h"
#include "SDK/Core/AFIRecordManager.h"

class AFIClass : public NFList<std::string>//include files
{
public:
    virtual ~AFIClass() {}

    virtual NF_SHARE_PTR<AFIPropertyMgr> GetPropertyManager() = 0;
    virtual NF_SHARE_PTR<AFIRecordManager> GetRecordManager() = 0;

    virtual void SetParent(NF_SHARE_PTR<AFIClass> pClass) = 0;
    virtual NF_SHARE_PTR<AFIClass> GetParent() = 0;

    virtual void SetTypeName(const char* strType) = 0;
    virtual const std::string& GetTypeName() = 0;
    virtual const std::string& GetClassName() = 0;

    virtual const bool AddConfigName(std::string& strConfigName) = 0;
    virtual NFList<std::string>& GetConfigNameList() = 0;

    virtual void SetInstancePath(const std::string& strPath) = 0;
    virtual const std::string& GetInstancePath() = 0;

    virtual bool AddClassCallBack(const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool DoEvent(const AFGUID& objectID, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& valueList) = 0;
};

class AFIClassModule
    : public AFIModule,
      public AFMapEx<std::string, AFIClass>
{
public:
    virtual ~AFIClassModule() {}

    virtual bool Load() = 0;
    virtual bool Save() = 0;
    virtual bool Clear() = 0;

    template<typename BaseType>
    bool AddClassCallBack(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const CLASS_OBJECT_EVENT, const AFIDataList&))
    {
        CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        CLASS_EVENT_FUNCTOR_PTR functorPtr(new CLASS_EVENT_FUNCTOR(functor));
        return AddClassCallBack(strClassName, functorPtr);
    }

    virtual bool DoEvent(const AFGUID& objectID, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& valueList) = 0;

    virtual bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual NF_SHARE_PTR<AFIPropertyMgr> GetClassPropertyManager(const std::string& strClassName) = 0;
    virtual NF_SHARE_PTR<AFIRecordManager> GetClassRecordManager(const std::string& strClassName) = 0;
};