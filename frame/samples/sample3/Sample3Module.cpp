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

#include "base/AFDataDefine.hpp"
#include "base/AFDateTime.hpp"
#include "Sample3Module.h"

namespace ark
{

    bool Sample3Module::Init()
    {
        std::cout << typeid(Sample3Module).name() << ", Init" << std::endl;
        return true;
    }

    int Sample3Module::OnEvent(const AFGUID& self, const int event, const AFIDataList& arg)
    {
        std::cout << "OnEvent EventID: " << event << " self: " << self << " argList: " << arg.String(0) << " " << " " << arg.Int(1) << std::endl;

        m_pKernelModule->SetNodeString(self, "Hello", arg.String(0));
        m_pKernelModule->SetNodeInt(self, "World", arg.Int(1));

        return 0;
    }

    int Sample3Module::OnClassCallBackEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT event, const AFIDataList& arg)
    {
        std::cout << "OnClassCallBackEvent ClassName: " << strClassName << " ID: " << self << " Event: " << event << std::endl;

        if (event == ARK_ENTITY_EVENT::ENTITY_EVT_DATA_FINISHED)
        {
            m_pKernelModule->AddEventCallBack(self, 11111111, this, &Sample3Module::OnEvent);
        }

        return 0;
    }

    int Sample3Module::OnIntDataNodeCB(const AFGUID& self, const std::string& strNodeName, const AFIData& oldVar, const AFIData& newVar)
    {
        std::cout << "OnIntDataNodeCB DataNode: " << strNodeName << " OldValue: " << oldVar.GetInt() << " NewValue: " << newVar.GetInt() << std::endl;

        return 0;
    }

    int Sample3Module::OnStrDataNodeCB(const AFGUID& self, const std::string& strNodeName, const AFIData& oldVar, const AFIData& newVar)
    {
        //属性回调事件，只要属性值内容有变化，就会被回调
        std::cout << "OnStrDataNodeCB DataNode: " << strNodeName << " OldValue: " << oldVar.GetString() << " NewValue: " << newVar.GetString() << std::endl;

        return 0;
    }

    bool Sample3Module::PostInit()
    {
        std::cout << typeid(Sample3Module).name() << ", PostInit" << std::endl;

        m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
        m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();
        m_pClassModule = pPluginManager->FindModule<AFIMetaClassModule>();
        m_pMapModule = pPluginManager->FindModule<AFIMapModule>();

        ARK_LOG_INFO("Init finished...");

        //Create scene, all entity need in scene
        m_pMapModule->CreateMap(1);

        //Add Class callback
        m_pKernelModule->AddClassCallBack(ark::Player::ThisName(), this, &Sample3Module::OnClassCallBackEvent);
        //Create Entity
        //Entity GUID is (0, 10)
        //SceneID = 1
        //GroupID = 0
        //Class Name = ark::Player::ThisName()
        ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->CreateEntity(1, 1, 0, ark::Player::ThisName(), "", AFCDataList());
        if (pEntity == nullptr)
        {
            return false;
        }

        //to do
        AFCData xData;
        m_pClassModule->AddNodeCallBack(ark::Player::ThisName(), ark::Player::Gold(), this, &Sample3Module::OnIntDataNodeCB);
        m_pClassModule->AddNodeCallBack(ark::Player::ThisName(), ark::Player::Name(), this, &Sample3Module::OnStrDataNodeCB);
        m_pKernelModule->AddClassCallBack(ark::Player::ThisName(), this, &Sample3Module::OnClassCallBackEvent);

        pEntity->SetNodeInt(ark::Player::Gold(), 1000);
        pEntity->SetNodeString(ark::Player::Name(), "test");

        m_pKernelModule->DoEvent(pEntity->Self(), 11111111, AFCDataList() << "another_test" << int(200));
        return true;
    }

    bool Sample3Module::PreShut()
    {
        std::cout << typeid(Sample3Module).name() << ", PreShut" << std::endl;
        m_pKernelModule->DestroyAll();

        return true;
    }

    bool Sample3Module::Shut()
    {
        std::cout << typeid(Sample3Module).name() << ", Shut" << std::endl;
        return true;
    }

}