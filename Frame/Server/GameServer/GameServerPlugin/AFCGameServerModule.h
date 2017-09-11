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
// * @file  	AFCGameServerModule.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFCGameServerModule                                                  *
*****************************************************************************/
#ifndef AFC_GAMESERVER_MODULE_H
#define AFC_GAMESERVER_MODULE_H

#include "SDK/Base/AFMap.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIGameServerModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIGUIDModule.h"

class AFCGameServerModule
    : public AFIGameServerModule
{
public:
    AFCGameServerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~AFCGameServerModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();
    virtual bool BeforeShut();


protected:


protected:
    AFIGUIDModule* m_pUUIDModule;
    AFIClassModule* m_pClassModule;
    AFIKernelModule* m_pKernelModule;
private:
};

#endif


