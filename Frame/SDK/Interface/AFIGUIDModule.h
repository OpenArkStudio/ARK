// -------------------------------------------------------------------------
//    @FileName         :    AFIGUIDModule.h
//    @Author           :    Nick Yang
//    @Date             :    2014-4-9
//    @Module           :    AFIGUIDModule
//
// -------------------------------------------------------------------------

#pragma once

#include "NFIModule.h"

class AFIGUIDModule
    : public NFIModule
{
public:
    //You need to call this function first then you can call CreateGUID, please follow this order
    virtual void SetWorkerAndDatacenter(uint16_t worker_id, uint16_t data_center_id) = 0;
    virtual uint64_t CreateGUID() = 0;
};