/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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
#include "Common/AFDataDefine.hpp"
#include "SDK/Core/AFList.hpp"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIConfigModule.h"
#include "SDK/Interface/AFIMetaClassModule.h"
#include "SDK/Interface/AFIMapModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "Server/Interface/AFISceneProcessModule.h"
#include "Server/Interface/AFIPropertyModule.h"
#include "Server/Interface/AFIGameNetModule.h"

namespace ark
{

    class AFCSceneProcessModule : public AFISceneProcessModule
    {
    public:
        explicit AFCSceneProcessModule() = default;

        bool Init() override;
        bool PostInit() override;

        virtual E_SCENE_TYPE GetCloneSceneType(const int nSceneID);
        virtual bool IsCloneScene(const int nSceneID);
        virtual bool ApplyCloneGroup(const int nSceneID, int& nGroupID);
        virtual bool ExitCloneGroup(const int nSceneID, const int& nGroupID);

    protected:
        int CreateCloneScene(const int& nSceneID);
        bool CreateSceneObject(const int nSceneID, const int nGroupID);
        bool LoadSceneResource(const int nSceneID);

        int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var);
        int OnEnterSceneEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);
        int OnLeaveSceneEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

        void OnClientSwapSceneProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    private:
        AFIConfigModule* m_pConfigModule;
        AFIMetaClassModule* m_pClassModule;
        AFIKernelModule* m_pKernelModule;
        AFIMapModule* m_pMapModule;
        AFILogModule* m_pLogModule;
        AFIGameNetModule* m_pGameNetModule;
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
    };

}