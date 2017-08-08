// -------------------------------------------------------------------------
//    @FileName         :    HelloWorld3.h
//    @Author           :    Ark Game Tech
//    @Date             :    2014-05-14 08:51
//    @Module           :   HelloWorld3
//
// -------------------------------------------------------------------------

#ifndef AFC_HELLO_WORLD3_H
#define AFC_HELLO_WORLD3_H

#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIPluginManager.h"

class HelloWorld3Module
    : public AFIModule
{
public:
    HelloWorld3Module(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool AfterInit();

    virtual bool Execute();

    virtual bool BeforeShut();
    virtual bool Shut();

protected:
    int OnEvent(const AFGUID& self, const int event, const AFIDataList& arg);
    int OnClassCallBackEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT event, const AFIDataList& arg);
    int OnPropertyCallBackEvent(const AFGUID& self, const std::string& strProperty, const AFIData& oldVarList, const AFIData& newVarList);
    int OnPropertyStrCallBackEvent(const AFGUID& self, const std::string& strProperty, const AFIData& oldVarList, const AFIData& newVarList);

    int OnHeartBeat(const AFGUID& self, const std::string& strHeartBeat, const float fTime, const int nCount);

protected:
    unsigned long mLastTime;
protected:
    AFIKernelModule* m_pKernelModule;
    AFIElementModule* m_pElementModule;
};

#endif
