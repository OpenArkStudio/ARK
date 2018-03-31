/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "HelloWorld2.h"
#include "SDK/Core/AFCEntity.h"
#include "SDK/Core/AFIComponent.h"

bool HelloWorld2::Init()
{
    std::cout << "Hello, world2, Init" << std::endl;

    return true;
}

int HelloWorld2::OnPropertyCallBackEvent(const AFGUID& self, const std::string& strProperty, const AFIData& oldVar, const AFIData& newVar)
{
    //属性回调事件，只要属性值内容有变化，就会被回调
    std::cout << "OnPropertyCallBackEvent Property: " << strProperty << " OldValue: " << oldVar.GetInt() << " NewValue: " << newVar.GetInt() << std::endl;

    return 0;
}

bool HelloWorld2::PostInit()
{
    AFCDataList xData;
    xData.AddInt(111);

    {
        std::cout << "Hello, world2, PostInit" << std::endl;

        //created a object for this test
        AFIEntity* pEntity = new AFCEntity(AFGUID(0, 1), pPluginManager);

        //add a property name is "Hello" of this object
        pEntity->GetNodeManager()->AddNode("Hello", AFCData(DT_INT, 1), 0);
        //add a property name is "World" of this object
        pEntity->GetNodeManager()->AddNode("World", AFCData(DT_INT, 1), 0);

        //set the "world" property value as 1111
        pEntity->SetNodeInt("World", 1111);
        int n1 = pEntity->GetNodeInt("World");
        //get the "world" property value and printf it
        const int nDataNode1 = pEntity->GetNodeInt("World");
        std::cout << "DataNode - World:" << nDataNode1 << std::endl;

        //add a call back functin for "world" property
        pEntity->AddNodeCallBack("World", this, &HelloWorld2::OnPropertyCallBackEvent);

        ////set the "world" property value as 2222[than the function "HelloWorld2::OnPropertyCallBackEvent" will be called]
        pEntity->SetNodeInt("World", 2222);
        int n2 = pEntity->GetNodeInt("World");

        //get the "world" property value and printf it
        const int nProperty2 = pEntity->GetNodeInt("World");
        std::cout << "Property World:" << nProperty2 << std::endl;

    }

    return true;
}

void HelloWorld2::Update()
{

}

bool HelloWorld2::PreShut()
{
    std::cout << "Hello, world2, PreShut" << std::endl;
    return true;
}

bool HelloWorld2::Shut()
{
    std::cout << "Hello, world2, Shut" << std::endl;
    return true;
}