// -------------------------------------------------------------------------
//    @FileName			:    AFIPropertyConfigModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-09-30
//    @Module           :    AFIPropertyConfigModule
//
// -------------------------------------------------------------------------

#ifndef AFI_PROPERTY_COAFIG_MODULE_H
#define AFI_PROPERTY_COAFIG_MODULE_H

#include <iostream>
#include "AFIModule.h"

class AFIPropertyConfigModule
    : public AFIModule
{
public:

    virtual int CalculateBaseValue(const int nJob,  const int nLevel, const std::string& strProperty) = 0;
    virtual bool LegalLevel(const int nJob, const int nLevel) = 0;
};


#endif
