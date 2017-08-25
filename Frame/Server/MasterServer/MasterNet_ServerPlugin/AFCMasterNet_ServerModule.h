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
// * @FileName  AFCMasterNet_ServerModule.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFCMasterNet_ServerModule                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFCMasterNet_ServerModule                                                  *
*****************************************************************************/
#ifndef AFC_MASTERNET_SERVER_MODULE_H
#define AFC_MASTERNET_SERVER_MODULE_H

//  the cause of sock'libariy, thenfore "AFCNet.h" much be included first.

#include "SDK/Proto/AFMsgDefine.h"
#include "SDK/Interface/AFIMasterNet_ServerModule.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFINetServerModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"

class AFCMasterNet_ServerModule
    : public AFIMasterNet_ServerModule
{
public:
    AFCMasterNet_ServerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
        mnLastLogTime = pPluginManager->GetNowTime();
    }

    virtual bool Init();
    virtual bool Shut();

    virtual bool AfterInit();
    virtual bool Execute();

    virtual void LogReceive(const char* str) {}
    virtual void LogSend(const char* str) {}

protected:

    void OnSocketEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID);

    //连接丢失,删2层(连接对象，帐号对象)
    void OnClientDisconnect(const AFGUID& xClientID);
    //有连接
    void OnClientConnected(const AFGUID& xClientID);

protected:
    //世界服务器注册，刷新信息
    void OnWorldRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnWorldUnRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnRefreshWorldInfoProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //////////////////////////////////////////////////////////////////////////
    //登录服务器注册，刷新信息
    void OnLoginRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnLoginUnRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnRefreshLoginInfoProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //选择世界服务器消息
    void OnSelectWorldProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnSelectServerResultProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //////////////////////////////////////////////////////////////////////////

    void SynWorldToLogin();
    void LogGameServer();

    void OnHeartBeat(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void InvalidMessage(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

private:

    int64_t mnLastLogTime;

    //serverid,data
    AFMapEx<int, ServerData> mWorldMap;
    AFMapEx<int, ServerData> mLoginMap;


    AFIElementModule* m_pElementModule;
    AFIClassModule* m_pClassModule;
    AFIKernelModule* m_pKernelModule;
    AFILogModule* m_pLogModule;
    AFINetServerModule* m_pNetModule;
};

#endif
