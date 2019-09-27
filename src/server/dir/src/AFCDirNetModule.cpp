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

#include "dir/include/AFCDirNetModule.hpp"

namespace ark {

bool AFCDirNetModule::Init()
{
    m_pLogModule = FindModule<AFILogModule>();
    m_pBusModule = FindModule<AFIBusModule>();
    m_pNetServiceManagerModule = FindModule<AFINetServiceManagerModule>();

    return true;
}

bool AFCDirNetModule::PostInit()
{
    StartServer();
    return true;
}

int AFCDirNetModule::StartServer()
{
    auto ret = m_pNetServiceManagerModule->CreateServer();
    ret.Then([=](const std::pair<bool, std::string>& resp) {
        if (!resp.first)
        {
            ARK_LOG_ERROR(
                "Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), resp.second);
            ARK_ASSERT_NO_EFFECT(0);
            exit(0);
        }

        m_pNetServer = m_pNetServiceManagerModule->GetSelfNetServer();
        if (m_pNetServer == nullptr)
        {
            ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->GetSelfBusName());
            exit(0);
        }
    });

    return 0;
}

//bool AFCDirNetModule::PreUpdate()
//{
//    int ret = StartClient();
//    return (ret == 0);
//}

//int AFCDirNetModule::StartClient()
//{
//    // Create all clients that need to connect the target endpoint
//    int ret = m_pNetServiceManagerModule->CreateClusterClients();
//    if (ret != 0)
//    {
//        ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
//        ARK_ASSERT_NO_EFFECT(0);
//        return ret;
//    }
//
//    // Check the master connection
//    AFINetClientService* pNetClientMaster =
//        m_pNetServiceManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_MASTER);
//    if (pNetClientMaster == nullptr)
//    {
//        return -1;
//    }
//
//    return 0;
//}

std::shared_ptr<AFINetServerService> AFCDirNetModule::GetNetServer()
{
    return m_pNetServer;
}

} // namespace ark
