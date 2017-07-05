// -------------------------------------------------------------------------
//    @FileName         £º    AFIGameServerToWorldModule.h
//    @Author           £º    Ark Game Tech
//    @Date             £º    2012-12-15
//    @Module           £º    AFIGameServerToWorldModule
//
// -------------------------------------------------------------------------

#ifndef AFI_GAMESERVERTOWORLDMODULE_H
#define AFI_GAMESERVERTOWORLDMODULE_H

#include <iostream>
#include "AFIModule.h"
#include "AFINetClientModule.hpp"

class AFIGameServerToWorldModule
    : public AFIModule
{

public:
	virtual void SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen) = 0;
    virtual AFINetClientModule* GetClusterClientModule() = 0;
};

#endif