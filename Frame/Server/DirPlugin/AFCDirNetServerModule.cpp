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

#include "AFCDirNetServerModule.h"
#include "SDK/Interface/AFIPluginManager.h"

namespace ark
{

    bool AFCDirNetServerModule::Init()
    {
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pNetServerManagerModule = pPluginManager->FindModule<AFINetServerManagerModule>();

        return true;
    }

    bool AFCDirNetServerModule::PostInit()
    {
        int ret = StartServer();
        if (ret != 0)
        {
            exit(0);
            return false;
        }

        return true;
    }

    int AFCDirNetServerModule::StartServer()
    {
        int ret = m_pNetServerManagerModule->CreateServer();
        if (ret != 0)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            ARK_ASSERT_NO_EFFECT(0);
            return ret;
        }

        m_pNetServer = m_pNetServerManagerModule->GetSelfNetServer();
        if (m_pNetServer == nullptr)
        {
            ret = -3;
            ARK_LOG_ERROR("Cannot find server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            return ret;
        }

        //m_pNetServer->AddRecvCallback(AFMsg::EGMI_PTWG_PROXY_REFRESH, this, &AFCWorldNetServerModule::OnRefreshProxyServerInfoProcess);

        m_pNetServer->AddEventCallBack(this, &AFCDirNetServerModule::OnSocketEvent);

        return 0;
    }

    void AFCDirNetServerModule::OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id)
    {
        switch (event)
        {
        case CONNECTED:
            OnClientConnected(conn_id);
            break;
        case DISCONNECTED:
            OnClientDisconnect(conn_id);
            break;
        default:
            break;
        }
    }

    void AFCDirNetServerModule::OnClientConnected(const AFGUID& conn_id)
    {
        ARK_LOG_INFO("Connected success, id = {}", conn_id.ToString());
    }

    void AFCDirNetServerModule::OnClientDisconnect(const AFGUID& conn_id)
    {
        ARK_LOG_ERROR("Connection closed, id = {}", conn_id.ToString());
    }

    AFINetServerService* AFCDirNetServerModule::GetNetServer()
    {
        return m_pNetServer;
    }

}