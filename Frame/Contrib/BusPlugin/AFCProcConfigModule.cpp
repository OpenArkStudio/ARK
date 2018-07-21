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

#include "AFCProcConfigModule.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"

AFCProcConfigModule::AFCProcConfigModule(AFIPluginManager* p)
{
    pPluginManager = p;
    LoadProcConfig();
}

bool AFCProcConfigModule::LoadProcConfig()
{
    //std::string proc_file_path = "../../cfg/proc.xml";

    //rapidxml::file<> xFileSource(proc_file_path.c_str());
    //rapidxml::xml_document<> xFileDoc;
    //xFileDoc.parse<0>(xFileSource.data());

    ////资源文件列表
    //rapidxml::xml_node<>* pRoot = xFileDoc.first_node();
    //if (pRoot == nullptr)
    //{
    //    ARK_ASSERT_NO_EFFECT(0);
    //    return false;
    //}


    return true;
}

//pluginloader.exe -d -x app_name=world app_bus_id=1.1.100.1 cfg=plugin.xml
//pluginloader.exe -d -x app_name=world app_bus_id=1.1.100.2 cfg=plugin.xml