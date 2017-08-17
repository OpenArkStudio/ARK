// -------------------------------------------------------------------------
//    @FileName         :    AFCProxyLogicModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    AFCProxyLogicModule
//
// -------------------------------------------------------------------------

#include "AFProxyLogicPlugin.h"
#include "AFCProxyLogicModule.h"

bool AFCProxyLogicModule::Init()
{
    return true;
}

bool AFCProxyLogicModule::Shut()
{
    return true;
}

bool AFCProxyLogicModule::Execute()
{
    return true;
}


bool AFCProxyLogicModule::AfterInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();

    return true;
}
