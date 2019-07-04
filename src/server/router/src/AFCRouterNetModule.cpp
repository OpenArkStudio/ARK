/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2019 ArkNX authors.
*
* Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "interface/AFIPluginManager.h"
#include "router/include/AFCRouterNetModule.h"

namespace ark
{

    bool AFCRouterNetModule::Init()
    {
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pNetServiceManagerModule = pPluginManager->FindModule<AFINetServiceManagerModule>();

        return true;
    }

    bool AFCRouterNetModule::PostInit()
    {
        int ret = StartServer();
        if (ret != 0)
        {
            exit(0);
            return false;
        }

        return true;
    }

    int AFCRouterNetModule::StartServer()
    {
        int ret = m_pNetServiceManagerModule->CreateServer();
        if (ret != 0)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            ARK_ASSERT_NO_EFFECT(0);
            return ret;
        }

        m_pNetServer = m_pNetServiceManagerModule->GetSelfNetServer();
        if (m_pNetServer == nullptr)
        {
            ret = -3;
            ARK_LOG_ERROR("Cannot find server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            return ret;
        }

        //add msg process

        return 0;
    }

    bool AFCRouterNetModule::PreUpdate()
    {
        int ret = StartClient();
        return (ret == 0);
    }

    int AFCRouterNetModule::StartClient()
    {
        //创建所有与对端链接的client
        int ret = m_pNetServiceManagerModule->CreateClusterClients();
        if (ret != 0)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            ARK_ASSERT_NO_EFFECT(0);
            return ret;
        }

        AFINetClientService* pNetClientWorld = m_pNetServiceManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_MASTER);
        if (pNetClientWorld == nullptr)
        {
            return 0;
        }

        //add msg process

        return 0;
    }

    AFINetServerService* AFCRouterNetModule::GetNetServer()
    {
        return m_pNetServer;
    }

}