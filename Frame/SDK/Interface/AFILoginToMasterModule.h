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
// * @file  	AFILoginToMasterModule.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFILoginToMasterModule                                                  *
*****************************************************************************/
#ifndef AFI_LOGINNET_CLIENTMODULE_H
#define AFI_LOGINNET_CLIENTMODULE_H

#include <iostream>
#include "AFIModule.h"
#include "AFINetClientModule.hpp"
#include "SDK/Proto/AFMsgPreGame.pb.h"

class AFILoginToMasterModule
    : public AFIModule
{
public:
    virtual AFINetClientModule* GetClusterModule() = 0;
    virtual AFMapEx<int, AFMsg::ServerInfoReport>& GetWorldMap() = 0;
};

#endif


