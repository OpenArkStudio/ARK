/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "Common/AFApp.hpp"
#include "SDK/Interface/AFINet.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFIModule.h"

class AFINetModule : public AFIModule
{
protected:
    AFINetModule() = default;

public:
    virtual ~AFINetModule() = default;

    template<typename BaseType>
    bool AddRecvCallback(const int nMsgID, BaseType* pBase, void (BaseType::*handleRecv)(const AFIMsgHead&, const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_RECV_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        NET_RECV_FUNCTOR_PTR functorPtr = std::make_shared<NET_RECV_FUNCTOR>(functor);

        return AddRecvCallback(nMsgID, functorPtr);
    }

    template<typename BaseType>
    bool AddRecvCallback(BaseType* pBase, void (BaseType::*handleRecv)(const AFIMsgHead&, const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_RECV_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        NET_RECV_FUNCTOR_PTR functorPtr = std::make_shared < NET_RECV_FUNCTOR>(functor);

        return AddRecvCallback(functorPtr);
    }

    virtual bool AddRecvCallback(const int nMsgID, const NET_RECV_FUNCTOR_PTR& cb)
    {
        if (mxRecvCallBack.find(nMsgID) != mxRecvCallBack.end())
        {
            return false;
        }
        else
        {
            mxRecvCallBack.insert(std::make_pair(nMsgID, cb));
            return true;
        }
    }

    virtual bool AddRecvCallback(const NET_RECV_FUNCTOR_PTR& cb)
    {
        mxCallBackList.push_back(cb);

        return true;
    }

    template<typename BaseType>
    bool AddEventCallBack(BaseType* pBase, void (BaseType::*handler)(const NetEventType, const AFGUID&, const int))
    {
        NET_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        NET_EVENT_FUNCTOR_PTR functorPtr(new NET_EVENT_FUNCTOR(functor));

        return AddEventCallBack(functorPtr);
    }

    virtual bool AddEventCallBack(const NET_EVENT_FUNCTOR_PTR& cb)
    {
        mxEventCallBackList.push_back(cb);

        return true;
    }

protected:
    void OnRecvBaseNetPack(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        auto it = mxRecvCallBack.find(nMsgID);

        if (mxRecvCallBack.end() != it)
        {
            (*it->second)(xHead, nMsgID, msg, nLen, xClientID);
        }
        else
        {
            for (auto iter : mxCallBackList)
            {
                (*iter)(xHead, nMsgID, msg, nLen, xClientID);
            }
        }
    }

    void OnSocketBaseNetEvent(const NetEventType eEvent, const AFGUID& xClientID, int nServerID)
    {
        for (auto it : mxEventCallBackList)
        {
            (*it)(eEvent, xClientID, nServerID);
        }
    }

private:
    std::map<int, NET_RECV_FUNCTOR_PTR> mxRecvCallBack;
    std::list<NET_EVENT_FUNCTOR_PTR> mxEventCallBackList;
    std::list<NET_RECV_FUNCTOR_PTR> mxCallBackList;
};

#define ARK_MSG_PROCESS(xHead, nMsgID, msgData, nLen, msgType)                          \
    AFGUID nPlayerID;                                                                   \
    msgType xMsg;                                                                       \
    if (!AFIMsgModule::RecvPB(xHead, msgData, nLen, xMsg, nPlayerID))                   \
    {                                                                                   \
        ARK_LOG_ERROR("Parse msg error, nMsgID = %d", nMsgID);                          \
        return;                                                                         \
    }                                                                                   \
                                                                                        \
    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(nPlayerID);           \
    if (nullptr == pEntity)                                                                                                 \
    {                                                                                                                       \
        ARK_LOG_ERROR("FromClient Object do not Exist, nMsgID = %d player_id = %s", nMsgID, nPlayerID.ToString().c_str());  \
        return;                                                                                                             \
    }

#define ARK_MSG_PROCESS_NO_OBJECT(xHead, msgData, nLen, msgType)                        \
    AFGUID nPlayerID;                                                                   \
    msgType xMsg;                                                                       \
    if (!AFIMsgModule::RecvPB(xHead, msgData, nLen, xMsg, nPlayerID))                   \
    {                                                                                   \
        ARK_LOG_ERROR("Parse msg error, nMsgID = %d", nMsgID);                          \
        return;                                                                         \
    }

#define  ARK_MSG_PROCESS_NO_OBJECT_STRING(xHead, msg, nLen)                     \
    std::string strMsg;                                                         \
    AFGUID nPlayerID;                                                           \
    int nHasKey = 0;                                                            \
    if (AFIMsgModule::RecvPB((xHead), (msg), (nLen), strMsg, nPlayerID))        \
    {                                                                           \
        nHasKey = nPlayerID.nLow;                                               \
    }
