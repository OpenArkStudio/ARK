// -------------------------------------------------------------------------
//    @FileName			:    AFCSceneModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCSceneModule
//
// -------------------------------------------------------------------------

#ifndef AFC_SCENE_MODULE_H
#define AFC_SCENE_MODULE_H

#include <iostream>
#include "SDK/Core/AFIObject.h"
#include "SDK/Core/AFGUID.h"
#include "SDK/Interface/AFISceneModule.h"

class AFCSceneModule
    : public AFISceneModule
{
public:
    AFCSceneModule(AFIPluginManager* p)
    {
        //pPluginManager = p;
    }


    virtual ~AFCSceneModule()
    {
    }

    virtual bool Init();
    virtual bool AfterInit();
    virtual bool BeforeShut();
    virtual bool Shut();
    virtual bool Execute();
};

#endif