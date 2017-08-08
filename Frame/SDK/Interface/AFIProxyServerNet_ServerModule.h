// -------------------------------------------------------------------------
//    @FileName         £º    AFIProxyServerNet_ServerModule.h
//    @Author           £º    Ark Game Tech
//    @Date             £º    2012-12-15
//    @Module           £º    AFIProxyServerNet_ServerModule
//
// -------------------------------------------------------------------------

#ifndef AFI_PROXYNET_SERVERMODULE_H
#define AFI_PROXYNET_SERVERMODULE_H

#include <iostream>
#include "AFIModule.h"
#include "AFINetServerModule.h"

class AFIProxyServerNet_ServerModule
    :  public AFIModule
{

public:
    virtual int Transpond(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID) = 0;
    virtual int EnterGameSuccessEvent(const AFGUID xClientID, const AFGUID xPlayerID) = 0;
};

#endif