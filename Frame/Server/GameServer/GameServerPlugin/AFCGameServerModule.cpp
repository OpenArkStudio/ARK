// -------------------------------------------------------------------------
//    @FileName         :    NFCGameServerModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    NFCGameServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#include "NFGameServerPlugin.h"
#include "NFCGameServerModule.h"

bool NFCGameServerModule::Init()
{
    m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
    m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
    m_pUUIDModule = pPluginManager->FindModule<NFIUUIDModule>();

    m_pUUIDModule->SetIdentID(1);

    return true;
}

bool NFCGameServerModule::Shut()
{

    return true;
}

bool NFCGameServerModule::Execute()
{
    return true;
}

bool NFCGameServerModule::AfterInit()
{
    return true;
}

bool NFCGameServerModule::BeforeShut()
{
    return true;
}
