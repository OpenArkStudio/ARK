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

#pragma once

#include "Contrib/Interface/AFIBusConfigModule.h"
#include "Contrib/Interface/AFIProcConfigModule.h"

class AFCBusConfigModule final : public AFIBusConfigModule
{
public:
    explicit AFCBusConfigModule(AFIPluginManager* p);
    virtual ~AFCBusConfigModule() = default;

    virtual bool Init();

    virtual bool GetBusRelation(const ARK_PROCESS_TYPE& target_type, const int& inst_id, ARK_CONNECTION_TYPE& connect_type, AFHostConfig& host_config);
    virtual bool GetBusServer(const ARK_PROCESS_TYPE& type, const uint8_t inst_id, AFHostConfig& host_config);

    virtual const ARK_PROCESS_TYPE GetSelfProcType();
    virtual const AFBusAddr GetSelfBusID();

protected:
    bool LoadBusRelation();

private:
    std::map<ARK_PROCESS_TYPE, std::map<ARK_PROCESS_TYPE, ARK_CONNECTION_TYPE>> mxBusRelations;

    AFIProcConfigModule* m_pProcConfigModule;
};