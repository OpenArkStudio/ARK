// -------------------------------------------------------------------------
//    @FileName			:    AFCLoginLogicModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCLoginLogicModule
//    @Desc             :
// -------------------------------------------------------------------------

#include "NFLoginLogicPlugin.h"
#include "AFCLoginLogicModule.h"


bool AFCLoginLogicModule::Init()
{

    return true;
}

bool AFCLoginLogicModule::Shut()
{
    return true;
}

int AFCLoginLogicModule::OnLoginProcess(const AFGUID& object, const std::string& strAccount, const std::string& strPwd)
{
	return 0;
}

bool AFCLoginLogicModule::Execute()
{


    return true;
}


bool AFCLoginLogicModule::AfterInit()
{
    m_pLoginNet_ServerModule = pPluginManager->FindModule<AFILoginNet_ServerModule>();

    return true;
}
