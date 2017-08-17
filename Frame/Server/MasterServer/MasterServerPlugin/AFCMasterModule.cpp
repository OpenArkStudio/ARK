// -------------------------------------------------------------------------
//    @FileName         :    AFCMasterModle.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCMasterModle
//    @Desc             :
// -------------------------------------------------------------------------

#include "AFCMasterModule.h"
#include "AFMasterPlugin.h"

bool AFCMasterModule::Init()
{

    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();

    return true;
}

bool AFCMasterModule::Shut()
{
    return true;
}

bool AFCMasterModule::Execute()
{


    return true;
}
