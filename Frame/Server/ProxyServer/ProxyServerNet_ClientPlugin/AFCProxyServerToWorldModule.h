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
// * @file  	AFCProxyServerToWorldModule.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFCProxyServerToWorldModule                                                  *
*****************************************************************************/
#ifndef AFC_PROXYSERVER_NETCLIENT_MODULE_H
#define AFC_PROXYSERVER_NETCLIENT_MODULE_H

#include <string>
#include "SDK/Proto/AFMsgDefine.h"
#include "SDK/Core/AFCHeartBeatManager.h"
#include "SDK/Interface/AFIProxyServerToWorldModule.h"
#include "SDK/Interface/AFIProxyServerNet_ServerModule.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIProxyLogicModule.h"
#include "SDK/Interface/AFINetServerModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFINetClientModule.hpp"
#include "SDK/Interface/AFIProxyServerToGameModule.h"

class AFCProxyServerToWorldModule : public AFIProxyServerToWorldModule
{
public:

    AFCProxyServerToWorldModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

    virtual void LogReceive(const char* str) {}
    virtual void LogSend(const char* str) {}

	virtual AFINetClientModule* GetClusterModule();
    virtual bool VerifyConnectData(const std::string& strAccount, const std::string& strKey);

protected:

    void OnSocketWSEvent(  const NetEventType eEvent, const AFGUID& xClientID, const int nServerID);

    void Register(const int nServerID);

    void OnSelectServerResultProcess(  const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnServerInfoProcess(  const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    void LogServerInfo(const std::string& strServerInfo);

	void OnOtherMessage(  const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
private:
    struct ClientConnectData
    {
        ClientConnectData()
        {
            strAccount = "";
            strConnectKey = "";
        }

        std::string strAccount;
        std::string strConnectKey;
    };


    AFMapEx<std::string, ClientConnectData> mWantToConnectMap;

private:

    AFILogModule* m_pLogModule;
    AFIProxyLogicModule* m_pProxyLogicModule;
    AFIKernelModule* m_pKernelModule;
    AFIProxyServerNet_ServerModule* m_pProxyServerNet_ServerModule;
    AFIElementModule* m_pElementModule;
    AFIClassModule* m_pClassModule;
    AFIProxyServerToGameModule* m_pProxyServerToGameModule;
	AFINetClientModule* m_pNetClientModule;

};

#endif

