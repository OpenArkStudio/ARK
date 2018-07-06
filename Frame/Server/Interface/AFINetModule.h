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

#include "SDK/Interface/AFIModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Net/AFCNetServer.h"
#include "SDK/Proto/AFProtoCPP.hpp"

enum ARK_SERVER_TYPE
{
    ARK_ST_NONE             = 0,    //none
    //cluster
    ARK_ST_MASTER           = 1,    //master
    ARK_ST_DIR              = 2,    //dir
    ARK_ST_LOG              = 3,    //log
    ARK_ST_ROUTER           = 4,    //router, world & cluster middle layer
    ARK_CLUTER_RANK         = 5,    //cluster rand
    ARK_CLUSTER_MAIL        = 6,    //cluster mail
    ARK_CLUSTER_PUB         = 7,    //cluster public
    ARK_IDIP                = 8,    //idip

    //world
    ARK_ST_WORLD            = 50,   //world
    ARK_ST_GAME             = 51,   //game
    ARK_ST_LOGIN            = 52,   //login
    ARK_ST_PROXY            = 53,   //proxy
    ARK_ST_RANK             = 54,   //rank
    ARK_ST_PUB              = 55,   //public

    ARK_ST_CS_PROXY         = 80,   //cs_proxy, produce cross-server things

    //db
    ARK_ST_REDIS = 100,    //
    ARK_ST_MYSQL = 101,    //
};

class AFINetModule : public AFIModule
{

protected:
    AFINetModule() {}

public:
    virtual ~AFINetModule()
    {
    }

    template<typename BaseType>
    bool AddReceiveCallBack(const int nMsgID, BaseType* pBase, void (BaseType::*handleRecieve)(const AFIMsgHead& xHead, const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_RECEIVE_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        NET_RECEIVE_FUNCTOR_PTR functorPtr(new NET_RECEIVE_FUNCTOR(functor));

        return AddReceiveCallBack(nMsgID, functorPtr);
    }

    template<typename BaseType>
    bool AddReceiveCallBack(BaseType* pBase, void (BaseType::*handleRecieve)(const AFIMsgHead& xHead, const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_RECEIVE_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        NET_RECEIVE_FUNCTOR_PTR functorPtr(new NET_RECEIVE_FUNCTOR(functor));

        return AddReceiveCallBack(functorPtr);
    }

    virtual bool AddReceiveCallBack(const int nMsgID, const NET_RECEIVE_FUNCTOR_PTR& cb)
    {
        if (mxReceiveCallBack.find(nMsgID) != mxReceiveCallBack.end())
        {
            return false;
        }

        mxReceiveCallBack.insert(std::map<int, NET_RECEIVE_FUNCTOR_PTR>::value_type(nMsgID, cb));

        return true;
    }

    virtual bool AddReceiveCallBack(const NET_RECEIVE_FUNCTOR_PTR& cb)
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

    static bool ReceivePB(const AFIMsgHead& xHead, const char* msg, const uint32_t nLen, std::string& strMsg, AFGUID& nPlayer)
    {
        strMsg.assign(msg, nLen);
        nPlayer = xHead.GetPlayerID();
        return true;
    }

    static bool ReceivePB(const AFIMsgHead& xHead, const char* msg, const uint32_t nLen, google::protobuf::Message& xData, AFGUID& nPlayer)
    {
        if (!xData.ParseFromString(std::string(msg, nLen)))
        {
            //char szData[MAX_PATH] = { 0 };
            //log
            return false;
        }

        nPlayer = xHead.GetPlayerID();

        return true;
    }

    static AFGUID PBToGUID(AFMsg::PBGUID xID)
    {
        AFGUID xIdent;
        xIdent.nHigh = xID.high();
        xIdent.nLow = xID.low();

        return xIdent;
    }

    static AFMsg::PBGUID GUIDToPB(AFGUID xID)
    {
        AFMsg::PBGUID  xIdent;
        xIdent.set_high(xID.nHigh);
        xIdent.set_low(xID.nLow);

        return xIdent;
    }

    static Point3D PBToVec(AFMsg::Point3D xPoint)
    {
        Point3D xID;
        xID.x = xPoint.x();
        xID.y = xPoint.y();
        xID.z = xPoint.z();
        return xID;
    }

    static AFMsg::Point3D VecToPB(Point3D xID)
    {
        AFMsg::Point3D xPoint;
        xPoint.set_x(xID.x);
        xPoint.set_y(xID.y);
        xPoint.set_z(xID.z);
        return xPoint;
    }

    static bool DataNodeToPBNode(const AFIData& DataVar, const char* name, AFMsg::PBNodeData& xMsg)
    {
        if (nullptr == name)
        {
            return false;
        }

        xMsg.set_node_name(name);
        xMsg.set_data_type(DataVar.GetType());
        AFMsg::VariantData* variantData = xMsg.mutable_variant_data();

        switch (DataVar.GetType())
        {
        case DT_BOOLEAN:
            variantData->set_bool_value(DataVar.GetBool());
            break;

        case DT_INT:
            variantData->set_int_value(DataVar.GetInt());
            break;

        case DT_INT64:
            variantData->set_int64_value(DataVar.GetInt64());
            break;

        case DT_FLOAT:
            variantData->set_float_value(DataVar.GetFloat());
            break;

        case DT_DOUBLE:
            variantData->set_double_value(DataVar.GetDouble());
            break;

        case DT_STRING:
            variantData->set_str_value(DataVar.GetString());
            break;

        case DT_OBJECT:
            *variantData->mutable_guid_value() = GUIDToPB(DataVar.GetObject());
            break;

        default:
            ARK_ASSERT_RET_VAL(0, false);
            break;
        }

        return true;
    }


    static bool TableCellToPBCell(const AFIData& DataVar, const int nRow, const int nCol, AFMsg::PBCellData& xMsg)
    {
        xMsg.set_col(nCol);
        xMsg.set_row(nRow);
        xMsg.set_data_type(DataVar.GetType());
        AFMsg::VariantData* variantData = xMsg.mutable_variant_data();

        switch (DataVar.GetType())
        {
        case DT_BOOLEAN:
            variantData->set_bool_value(DataVar.GetBool());
            break;

        case DT_INT:
            variantData->set_int_value(DataVar.GetInt());
            break;

        case DT_INT64:
            variantData->set_int64_value(DataVar.GetInt64());
            break;

        case DT_FLOAT:
            variantData->set_float_value(DataVar.GetFloat());
            break;

        case DT_DOUBLE:
            variantData->set_double_value(DataVar.GetDouble());
            break;

        case DT_STRING:
            variantData->set_str_value(DataVar.GetString());
            break;

        case DT_OBJECT:
            *variantData->mutable_guid_value() = GUIDToPB(DataVar.GetObject());
            break;

        default:
            ARK_ASSERT_RET_VAL(0, false);
            break;
        }

        return true;
    }

    static bool TableCellToPBCell(const AFIDataList& DataList, const int nRow, const int nCol, AFMsg::PBCellData& xMsg)
    {
        xMsg.set_col(nCol);
        xMsg.set_row(nRow);
        xMsg.set_data_type(DataList.GetType(nCol));
        AFMsg::VariantData* variantData = xMsg.mutable_variant_data();

        switch (DataList.GetType(nCol))
        {
        case DT_BOOLEAN:
            variantData->set_bool_value(DataList.Bool(nCol));
            break;

        case DT_INT:
            variantData->set_int_value(DataList.Int(nCol));
            break;

        case DT_INT64:
            variantData->set_int64_value(DataList.Int64(nCol));
            break;

        case DT_FLOAT:
            variantData->set_float_value(DataList.Float(nCol));
            break;

        case DT_DOUBLE:
            variantData->set_double_value(DataList.Double(nCol));
            break;

        case DT_STRING:
            variantData->set_str_value(DataList.String(nCol));
            break;

        case DT_OBJECT:
            *variantData->mutable_guid_value() = GUIDToPB(DataList.Object(nCol));
            break;

        default:
            ARK_ASSERT_RET_VAL(0, false);
            break;
        }

        return true;
    }

protected:
    void OnReceiveBaseNetPack(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        auto it = mxReceiveCallBack.find(nMsgID);

        if (mxReceiveCallBack.end() != it)
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
    std::map<int, NET_RECEIVE_FUNCTOR_PTR> mxReceiveCallBack;
    std::list<NET_EVENT_FUNCTOR_PTR> mxEventCallBackList;
    std::list<NET_RECEIVE_FUNCTOR_PTR> mxCallBackList;
};

#define ARK_MSG_PROCESS(xHead, nMsgID, msgData, nLen, msgType)                          \
    AFGUID nPlayerID;                                                                   \
    msgType xMsg;                                                                       \
    if (!AFINetModule::ReceivePB(xHead, msgData, nLen, xMsg, nPlayerID))                \
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
    if (!AFINetModule::ReceivePB(xHead, msgData, nLen, xMsg, nPlayerID))                \
    {                                                                                   \
        ARK_LOG_ERROR("Parse msg error, nMsgID = %d", nMsgID);                          \
        return;                                                                         \
    }

#define  ARK_MSG_PROCESS_NO_OBJECT_STRING(xHead, msg, nLen)                     \
    std::string strMsg;                                                         \
    AFGUID nPlayerID;                                                           \
    int nHasKey = 0;                                                            \
    if (AFINetModule::ReceivePB((xHead), (msg), (nLen), strMsg, nPlayerID))     \
    {                                                                           \
        nHasKey = nPlayerID.nLow;                                               \
    }
