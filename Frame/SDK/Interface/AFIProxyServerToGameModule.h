// -------------------------------------------------------------------------
//    @FileName         £º    AFIProxyServerToGameModule.h
//    @Author           £º    Ark Game Tech
//    @Date             £º    2012-12-15
//    @Module           £º    AFIProxyServerToGameModule
//
// -------------------------------------------------------------------------

#ifndef AFI_PROXYNET_TO_GAME_H
#define AFI_PROXYNET_TO_GAME_H

#include <iostream>
#include "AFIModule.h"
#include "AFINetClientModule.hpp"

class AFIProxyServerToGameModule
    : public  AFIModule
{
public:
	virtual AFINetClientModule* GetClusterModule() = 0;
};

#endif