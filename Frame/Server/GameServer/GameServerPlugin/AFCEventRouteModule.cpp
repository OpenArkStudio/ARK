// -------------------------------------------------------------------------
//    @FileName      :    AFCEventRouteModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2015-01-02
//    @Module           :    AFCEventRouteModule
//
// -------------------------------------------------------------------------

#include "AFCEventRouteModule.h"

bool AFCEventRouteModule::Init()
{


    return true;
}


bool AFCEventRouteModule::Shut()
{
    return true;
}

bool AFCEventRouteModule::Execute(const float fLasFrametime, const float fStartedTime)
{
    //Œª÷√ƒÿ
    return true;
}

bool AFCEventRouteModule::AfterInit()
{
    m_pEventProcessModule = dynamic_cast<AFIEventProcessModule*>(pPluginManager->FindModule("AFCEventProcessModule"));
    m_pKernelModule = dynamic_cast<AFIKernelModule*>(pPluginManager->FindModule("AFCKernelModule"));
    m_pLogModule = dynamic_cast<AFILogModule*>(pPluginManager->FindModule("AFCLogModule"));

    assert(NULL != m_pEventProcessModule);
    assert(NULL != m_pKernelModule);
    assert(NULL != m_pLogModule);


    return true;
}

bool AFCEventRouteModule::StartRouteEvent(const AFIDENTID& self, const AFIDataList& xDataList)
{
    return false;
}

bool AFCEventRouteModule::ReceiveRouteEvent(const AFIDENTID& self, const AFIDataList& xDataList)
{
    return false;
}

