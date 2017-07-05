// -------------------------------------------------------------------------
//    @FileName			:    AFCLevelModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-09-28
//    @Module           :    AFCLevelModule
//
// -------------------------------------------------------------------------

#ifndef AFC_LEVEL_MODULE_H
#define AFC_LEVEL_MODULE_H

#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIGameLogicModule.h"
#include "SDK/Interface/AFILevelModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIPropertyConfigModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Proto/NFProtocolDefine.hpp"
#include "SDK/Interface/AFIElementModule.h"

class AFCLevelModule
    : public AFILevelModule
{
public:
    AFCLevelModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~AFCLevelModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

    virtual int AddExp(const AFGUID& self, const int nExp);

private:
    AFIPropertyConfigModule* m_pPropertyConfigModule;
    AFIKernelModule* m_pKernelModule;
    AFILogModule* m_pLogModule;
    AFIElementModule* m_pElementModule;
};

#endif
