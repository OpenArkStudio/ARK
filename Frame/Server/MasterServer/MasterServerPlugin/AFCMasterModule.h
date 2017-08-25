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
// * @FileName  AFCMasterModule.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFCMasterModule                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFCMasterModule                                                  *
*****************************************************************************/
#ifndef AFC_MASTER_MODULE_H
#define AFC_MASTER_MODULE_H

#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIMasterModule.h"
#include "SDK/Interface/AFIMasterNet_ServerModule.h"

class AFCMasterModule
    : public AFIMasterModule
{

public:
    AFCMasterModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
protected:


private:

    AFIKernelModule* m_pKernelModule;
};

#endif
