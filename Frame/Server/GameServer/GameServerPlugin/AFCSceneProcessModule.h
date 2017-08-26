/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @file  	AFCSceneProcessModule.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFCSceneProcessModule                                                  *
*****************************************************************************/
#ifndef AFC_SCENEPROCESS_MODULE_H
#define AFC_SCENEPROCESS_MODULE_H

#include <string>
#include <map>
#include <iostream>
#include "SDK/Base/AFMap.h"
#include "SDK/Base/AFList.h"
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_iterators.hpp"
#include "RapidXML/rapidxml_print.hpp"
#include "RapidXML/rapidxml_utils.hpp"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIGameLogicModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIGameServerConfigModule.h"
#include "SDK/Interface/AFISceneProcessModule.h"
#include "SDK/Interface/AFIPropertyModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Proto/NFProtocolDefine.hpp"
#include "SDK/Interface/AFIGameServerNet_ServerModule.h"

class AFCSceneProcessModule
    : public AFISceneProcessModule
{
public:
    AFCSceneProcessModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~AFCSceneProcessModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

    virtual E_SCENE_TYPE GetCloneSceneType(const int nSceneID);
    virtual bool IsCloneScene(const int nSceneID);
    virtual bool ApplyCloneGroup(const int nSceneID, int& nGroupID);
    virtual bool ExitCloneGroup(const int nSceneID, const int& nGroupID);

protected:
    int CreateCloneScene(const int& nSceneID);

    bool CreateSceneObject(const int nSceneID, const int nGroupID);

    bool LoadSceneResource(const int nSceneID);

protected:

    int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& var);

    int OnEnterSceneEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);
    int OnLeaveSceneEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

protected:
    void OnClienSwapSceneProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

private:

    AFIElementModule* m_pElementModule;
    AFIClassModule* m_pClassModule;
    AFIKernelModule* m_pKernelModule;
    AFILogModule* m_pLogModule;
    AFIGameServerNet_ServerModule* m_pGameServerNet_ServerModule;
    //////////////////////////////////////////////////////////////////////////
    struct SceneSeedResource
    {
        std::string strSeedID;
        std::string strConfigID;
        float fSeedX;
        float fSeedY;
        float fSeedZ;
    };

    //SceneID,(SeedID,SeedData)
    AFMapEx<int, AFMapEx<std::string, SceneSeedResource>> mtSceneResourceConfig;

    //////////////////////////////////////////////////////////////////////////
};

#endif


