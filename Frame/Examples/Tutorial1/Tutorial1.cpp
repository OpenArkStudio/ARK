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
// * @file  	Tutorial1.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     Tutorial1                                                  *
*****************************************************************************/
#include "Tutorial1.h"
#include "HelloWorld1.h"

#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, Tutorial1)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, Tutorial1)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int Tutorial1::GetPluginVersion()
{
    return 0;
}

const std::string Tutorial1::GetPluginName()
{
    return GET_CLASS_NAME(Tutorial1)
}

void Tutorial1::Install()
{
    REGISTER_MODULE(pPluginManager, HelloWorld1, HelloWorld1)
}

void Tutorial1::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, HelloWorld1, HelloWorld1)
}
