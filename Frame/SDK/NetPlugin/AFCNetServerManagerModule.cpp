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

#include "SDK/Interface/AFIPluginManager.h"
#include "AFCNetServerManagerModule.h"
#include "AFCNetServerService.h"

bool AFCNetServerManagerModule::Init()
{
    m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();

    return true;
}

int AFCNetServerManagerModule::CreateServer()
{
    const AFServerConfig* server_config = m_pBusModule->GetAppServerInfo();
    if (server_config == nullptr)
    {
        ARK_LOG_ERROR("Cannot get process server info, bus id = {}", AFBusAddr(m_pBusModule->GetSelfBusID()).ToString());
        ARK_ASSERT_NO_EFFECT(0);
        return -1;
    }

    AFINetServerService* pServer = ARK_NEW AFCNetServerService();
    _net_servers.AddElement(m_pBusModule->GetSelfBusID(), pServer);

    int nRet = pServer->Start(m_pBusModule->GetSelfBusID(), server_config->private_url, server_config->thread_num, server_config->max_connection);
    if (nRet < 0)
    {
        ARK_LOG_ERROR("Cannot start server net, url = {}", server_config->private_url);
        ARK_ASSERT_NO_EFFECT(0);
        return -2;
    }

    return 0;
}

AFINetServerService* AFCNetServerManagerModule::GetSelfNetServer()
{
    return _net_servers.GetElement(m_pBusModule->GetSelfBusID());
}

bool AFCNetServerManagerModule::Update()
{
    for (bool bRet = _net_servers.Begin(); bRet; bRet = _net_servers.Increase())
    {
        const auto& pServer = _net_servers.GetCurrentData();
        if (pServer != nullptr)
        {
            pServer->Update();
        }
    }

    return true;
}

bool AFCNetServerManagerModule::Shut()
{
    for (bool bRet = _net_servers.Begin(); bRet; bRet = _net_servers.Increase())
    {
        const auto* pServer = _net_servers.GetCurrentData();
        if (pServer != nullptr)
        {
            ARK_DELETE(pServer);
        }
    }

    return true;
}