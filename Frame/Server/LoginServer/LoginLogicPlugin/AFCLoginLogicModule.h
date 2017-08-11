// -------------------------------------------------------------------------
//    @FileName			:    AFCLoginLogicModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCLoginLogicModule
//
// -------------------------------------------------------------------------

#ifndef AFC_LOGINLOGIC_MODULE_H
#define AFC_LOGINLOGIC_MODULE_H

#include "SDK/Base/AFMap.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFILoginLogicModule.h"
#include "SDK/Interface/AFILoginNet_ServerModule.h"

class AFCLoginLogicModule
    : public AFILoginLogicModule
{
public:
    AFCLoginLogicModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

    virtual int OnLoginProcess(const AFGUID& object, const std::string& strAccount, const std::string& strPwd);

protected:

protected:

    AFILoginNet_ServerModule* m_pLoginNet_ServerModule;
private:
};

#endif