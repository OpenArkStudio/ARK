// -------------------------------------------------------------------------
//    @FileName         ：    AFIBuffModule.h
//    @Author           ：    Ark Game Tech
//    @Date             ：    2012-12-15
//    @Module           ：    AFIBuffModule
//
// -------------------------------------------------------------------------

#ifndef AFI_LEVEL_MODULE_H
#define AFI_LEVEL_MODULE_H

#include <iostream>
#include "AFIModule.h"


class AFILevelModule
    : public AFIModule
{
public:
    //nType:增加原因
    virtual int AddExp(const AFGUID& self, const int nExp) = 0;

};

#endif