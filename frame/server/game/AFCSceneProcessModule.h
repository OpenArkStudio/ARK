/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#pragma once

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "base/AFList.hpp"
#include "interface/AFIKernelModule.h"
#include "interface/AFIConfigModule.h"
#include "interface/AFIMetaClassModule.h"
#include "interface/AFIMapModule.h"
#include "interface/AFILogModule.h"
#include "interface/AFIGUIDModule.h"
#include "interface/AFIPluginManager.h"
#include "interface/AFISceneProcessModule.h"
#include "interface/AFIPropertyModule.h"
#include "interface/AFIGameNetModule.h"

namespace ark
{

    class AFCSceneProcessModule : public AFISceneProcessModule
    {
    public:
        explicit AFCSceneProcessModule() = default;

        bool Init() override;
        bool PostInit() override;

        virtual E_SCENE_TYPE GetMapInstanceType(const int nSceneID);
        virtual bool IsInstanceMap(const int nSceneID);
        virtual bool ApplyMapInstance(const int nSceneID, int& nGroupID);
        virtual bool ExitMapInstance(const int nSceneID, const int& nGroupID);

    protected:
        int CreateMapInstance(const int& nSceneID);
        bool CreateMapEntities(const int nSceneID, const int nGroupID);
        bool LoadMapResource(const int nSceneID);

        int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const ArkEntityEvent eClassEvent, const AFIDataList& var);
        int OnEnterSceneEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);
        int OnLeaveSceneEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

        void OnClientSwapMapProcess(const AFNetMsg* msg);

    private:
        AFIConfigModule* m_pConfigModule;
        AFIMetaClassModule* m_pClassModule;
        AFIKernelModule* m_pKernelModule;
        AFIMapModule* m_pMapModule;
        AFILogModule* m_pLogModule;
        AFIGUIDModule* m_pGUIDModule;
        AFIGameNetModule* m_pGameNetModule;
        //////////////////////////////////////////////////////////////////////////
        struct MapSeedResource
        {
            std::string strSeedID;
            std::string strConfigID;
            float fSeedX;
            float fSeedY;
            float fSeedZ;
        };

        //SceneID,(SeedID,SeedData)
        AFMapEx<int, AFMapEx<std::string, MapSeedResource>> map_res_;
    };

}