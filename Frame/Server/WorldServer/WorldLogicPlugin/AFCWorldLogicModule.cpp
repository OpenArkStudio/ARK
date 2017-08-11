// -------------------------------------------------------------------------
//    @FileName         :    AFCWorldLogicModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCWorldLogicModule
//    @Desc             :
// -------------------------------------------------------------------------

#include "AFWorldLogicPlugin.h"
#include "AFCWorldLogicModule.h"
#include "SDK/Base/AFPlatform.hpp"

bool AFCWorldLogicModule::Init()
{
    return true;
}

bool AFCWorldLogicModule::Shut()
{
    return true;
}

bool AFCWorldLogicModule::Execute()
{
    return true;
}

bool AFCWorldLogicModule::AfterInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();

    return true;
}
