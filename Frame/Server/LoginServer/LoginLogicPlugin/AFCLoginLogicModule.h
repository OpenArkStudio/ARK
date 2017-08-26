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
// * @file  	AFCLoginLogicModule.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFCLoginLogicModule                                                  *
*****************************************************************************/
#ifndef AFC_LOGINLOGIC_MODULE_H
#define AFC_LOGINLOGIC_MODULE_H

#include "SDK/Base/AFMap.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFILoginLogicModule.h"
#include "SDK/Interface/AFILoginNet_ServerModule.h"

class AFCLoginLogicModule
    : public AFILoginLogicModule
{
public:
    AFCLoginLogicModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

    virtual int OnLoginProcess(const AFGUID& object, const std::string& strAccount, const std::string& strPwd);

protected:

protected:

    AFILoginNet_ServerModule* m_pLoginNet_ServerModule;
private:
};

#endif

