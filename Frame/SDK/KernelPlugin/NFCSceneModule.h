// -------------------------------------------------------------------------
//    @FileName			:    NFCSceneModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    NFCSceneModule
//
// -------------------------------------------------------------------------

#ifndef NFC_SCENE_MODULE_H
#define NFC_SCENE_MODULE_H

#include <iostream>
#include "SDK/Core/NFIObject.h"
#include "SDK/Core/AFGUID.h"
#include "SDK/Interface/NFISceneModule.h"

class NFCSceneModule
    : public NFISceneModule
{
public:
    NFCSceneModule(NFIPluginManager* p)
    {
        //pPluginManager = p;
    }


    virtual ~NFCSceneModule()
    {
    }

    virtual bool Init();
    virtual bool AfterInit();
    virtual bool BeforeShut();
    virtual bool Shut();
    virtual bool Execute();
};

#endif