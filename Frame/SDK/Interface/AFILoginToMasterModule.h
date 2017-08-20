// -------------------------------------------------------------------------
//    @FileName         £º    AFILoginNet_ClientModule.h
//    @Author           £º    Ark Game Tech
//    @Date             £º    2012-12-15
//    @Module           £º    AFILoginNet_ClientModule
//
// -------------------------------------------------------------------------

#ifndef AFI_LOGINNET_CLIENTMODULE_H
#define AFI_LOGINNET_CLIENTMODULE_H

#include <iostream>
#include "AFIModule.h"
#include "AFINetClientModule.hpp"
#include "SDK/Proto/AFMsgPreGame.pb.h"

class AFILoginToMasterModule
    : public AFIModule
{
public:
    virtual AFINetClientModule* GetClusterModule() = 0;
    virtual AFMapEx<int, AFMsg::ServerInfoReport>& GetWorldMap() = 0;
};

#endif
