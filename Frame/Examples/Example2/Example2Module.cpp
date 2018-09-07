/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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

#include "Example2Module.h"
#include "SDK/Core/AFCEntity.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIKernelModule.h"

bool Example2Module::Init()
{
    std::cout << typeid(Example2Module).name() << ", Init" << std::endl;
    return true;
}

int Example2Module::OnDataNodeCB(const AFGUID& self, const std::string& name, const AFIData& oldVar, const AFIData& newVar)
{
    std::cout << "OnDataNodeCB DataNode: " << name << " Old value: " << oldVar.GetInt() << " New value: " << newVar.GetInt() << std::endl;
    return 0;
}

bool Example2Module::PostInit()
{
    AFIKernelModule* pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    AFIClassModule* pClassModule = pPluginManager->FindModule<AFIClassModule>();

    AFCDataList xData;
    xData.AddInt(111);

    std::cout << typeid(Example2Module).name() << ", PostInit" << std::endl;

    //Create an Entity for this test
    AFIEntity* pEntity = ARK_NEW AFCEntity(AFGUID(0, 1));

    //Add a DataNode name is "my_test1" of this Entity
    pEntity->GetNodeManager()->AddNode("my_test1", AFCData(DT_INT, 1), 0);
    //Add a DataNode name is "my_test2" of this Entity
    pEntity->GetNodeManager()->AddNode("my_test2", AFCData(DT_INT, 1), 0);

    //Set the "my_test2" DataNode value as 1234
    pEntity->SetNodeInt("my_test2", 1234);
    //Get the "my_test2" property value and printf it
    const int nDataNode2 = pEntity->GetNodeInt("my_test2");
    std::cout << "DataNode - my_test2:" << nDataNode2 << std::endl;

    //Add a callback function for DataNode "my_test2"
    pClassModule->AddNodeCallBack("Player", "ls", this, &Example2Module::OnDataNodeCB);

    //Set the DataNode "my_test2" value as 5678, then the function "Example2Module::OnDataNodeCB" will be called
    pEntity->SetNodeInt("my_test2", 5678);
    int nDataNode2_new = pEntity->GetNodeInt("my_test2");

    //delete memory
    ARK_DELETE(pEntity);

    return true;
}

bool Example2Module::PreShut()
{
    std::cout << typeid(Example2Module).name() << ", PreShut" << std::endl;

    return true;
}

bool Example2Module::Shut()
{
    std::cout << typeid(Example2Module).name() << ", Shut" << std::endl;
    return true;
}