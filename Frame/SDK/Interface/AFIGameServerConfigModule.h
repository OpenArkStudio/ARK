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
// * @FileName  AFIGameServerConfigModule.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFIGameServerConfigModule                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFIGameServerConfigModule                                                  *
*****************************************************************************/
#ifndef AFI_GAMESERVER_COAFIG_MODULE_H
#define AFI_GAMESERVER_COAFIG_MODULE_H

#include <iostream>
#include "AFIModule.h"

class AFIGameServerConfigModule
    : public AFIModule
{
public:
    virtual bool AddSceneToActor(const int nSceneID, const int nGameServer) = 0;
    virtual int GetActorID(const int nSceneID) = 0;
};

#endif
