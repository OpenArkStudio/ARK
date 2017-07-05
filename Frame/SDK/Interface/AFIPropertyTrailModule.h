// -------------------------------------------------------------------------
//    @FileName			:    AFIPropertyTrailModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-09-30
//    @Module           :    AFIPropertyTrailModule
//
// -------------------------------------------------------------------------

#ifndef AFI_PROPERTY_TRAIL_MODULE_H
#define AFI_PROPERTY_TRAIL_MODULE_H

#include <iostream>
#include "AFIModule.h"

class AFIPropertyTrailModule
    : public AFIModule
{
public:
    virtual void StartTrail(const AFGUID self) = 0;
    virtual void EndTrail(const AFGUID self) = 0;

};


#endif
