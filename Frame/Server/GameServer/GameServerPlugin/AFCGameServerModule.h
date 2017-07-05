// -------------------------------------------------------------------------
//    @FileName			:    AFCGameServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCGameServerModule
//
// -------------------------------------------------------------------------

#ifndef AFC_GAMESERVER_MODULE_H
#define AFC_GAMESERVER_MODULE_H

#include "SDK/Core/AFMap.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIGameServerModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIUUIDModule.h"

class AFCGameServerModule
    : public AFIGameServerModule
{
public:
    AFCGameServerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~AFCGameServerModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();
    virtual bool BeforeShut();


protected:


protected:
    AFIUUIDModule* m_pUUIDModule;
    AFIClassModule* m_pClassModule;
    AFIKernelModule* m_pKernelModule;
private:
};

#endif
