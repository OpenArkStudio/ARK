// -------------------------------------------------------------------------
//    @FileName         :    AFISceneProcessModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-04-14
//    @Module           :    AFISceneProcessModule
//
// -------------------------------------------------------------------------

#ifndef AFI_SCENEPROCESS_MODULE_H
#define AFI_SCENEPROCESS_MODULE_H

#include <iostream>
#include "AFIModule.h"

enum E_SCENE_TYPE
{
    SCENE_TYPE_NORMAL,//城镇
    SCENE_TYPE_CLONE_SCENE,//副本

    SCENE_TYPE_ERROR,
};

class AFISceneProcessModule
    : public AFIModule
{

public:
    virtual bool IsCloneScene(const int nSceneID) = 0;
    virtual E_SCENE_TYPE GetCloneSceneType(const int nContainerID) = 0;
    virtual bool ApplyCloneGroup(const int nSceneID, int& nGroupID) = 0;
    virtual bool ExitCloneGroup(const int nSceneID, const int& nGroupID) = 0;
};

#endif