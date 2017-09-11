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
// * @file      AFINetModule.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFINetModule                                                  *
*****************************************************************************/
#ifndef AFI_NET_MODULE_H
#define AFI_NET_MODULE_H

#include <iostream>
#include <iosfwd>
//#include "SDK/Core/AFGUID.h"
#include "AFIModule.h"
#include "AFIPluginManager.h"
#include "SDK/Net/AFCNetServer.h"
//#include "SDK/Core/AFQueue.h"
#include "SDK/Proto/AFMsgDefine.h"
#include "SDK/Proto/AFDefine.pb.h"

enum NF_SERVER_TYPES
{
    NF_ST_NONE = 0,    // NONE
    NF_ST_REDIS = 1,    //
    NF_ST_MYSQL = 2,    //
    NF_ST_MASTER = 3,    //
    NF_ST_LOGIN = 4,    //
    NF_ST_PROXY = 5,    //
    NF_ST_GAME = 6,    //
    NF_ST_WORLD = 7,    //

};


class AFINetModule
    : public AFIModule
{
protected:
    AFINetModule()
    {
    }
public:

    virtual ~AFINetModule()
    {
    }

    template<typename BaseType>
    bool AddReceiveCallBack(const int nMsgID, BaseType* pBase, void (BaseType::*handleRecieve)(const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_RECEIVE_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        NET_RECEIVE_FUNCTOR_PTR functorPtr(new NET_RECEIVE_FUNCTOR(functor));

        return AddReceiveCallBack(nMsgID, functorPtr);
    }

    template<typename BaseType>
    bool AddReceiveCallBack(BaseType* pBase, void (BaseType::*handleRecieve)(const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_RECEIVE_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        NET_RECEIVE_FUNCTOR_PTR functorPtr(new NET_RECEIVE_FUNCTOR(functor));

        return AddReceiveCallBack(functorPtr);
    }

    virtual bool AddReceiveCallBack(const int nMsgID, const NET_RECEIVE_FUNCTOR_PTR& cb)
    {
        if(mxReceiveCallBack.find(nMsgID) != mxReceiveCallBack.end())
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

    static bool ReceivePB(const int nMsgID, const char* msg, const uint32_t nLen, std::string& strMsg, AFGUID& nPlayer)
    {
        AFMsg::MsgBase xMsg;
        if(!xMsg.ParseFromArray(msg, nLen))
        {
            //char szData[MAX_PATH] = { 0 };
            //NFSPRINTF(szData, MAX_PATH, "Parse Message Failed from Packet to MsgBase, MessageID: %d\n", nMsgID);
            //LogRecive(szData);

            return false;
        }

        strMsg.assign(xMsg.msg_data().data(), xMsg.msg_data().length());

        nPlayer = PBToNF(xMsg.player_id());

        return true;
    }

    static bool ReceivePB(const int nMsgID, const char* msg, const uint32_t nLen, google::protobuf::Message& xData, AFGUID& nPlayer)
    {
        AFMsg::MsgBase xMsg;
        if(!xMsg.ParseFromArray(msg, nLen))
        {
            //char szData[MAX_PATH] = { 0 };
            //NFSPRINTF(szData, MAX_PATH, "Parse Message Failed from Packet to MsgBase, MessageID: %d\n", nMsgID);
            //LogRecive(szData);

            return false;
        }

        if(!xData.ParseFromString(xMsg.msg_data()))
        {
            char szData[MAX_PATH] = { 0 };
            //NFSPRINTF(szData, MAX_PATH, "Parse Message Failed from MsgData to ProtocolData, MessageID: %d\n", nMsgID);
            //LogRecive(szData);

            return false;
        }

        nPlayer = PBToNF(xMsg.player_id());

        return true;
    }

    static AFGUID PBToNF(AFMsg::Ident xID)
    {
        AFGUID  xIdent;
        xIdent.nSerial = xID.svrid();
        xIdent.nIdent = xID.index();

        return xIdent;
    }

    static AFMsg::Ident NFToPB(AFGUID xID)
    {
        AFMsg::Ident  xIdent;
        xIdent.set_svrid(xID.nSerial);
        xIdent.set_index(xID.nIdent);

        return xIdent;
    }

    static Point3D PBToNF(AFMsg::Point3D xPoint)
    {
        Point3D xID;
        xID.x = xPoint.x();
        xID.y = xPoint.y();
        xID.z = xPoint.z();
        return xID;
    }

    static AFMsg::Point3D NFToPB(Point3D xID)
    {
        AFMsg::Point3D xPoint;
        xPoint.set_x(xID.x);
        xPoint.set_y(xID.y);
        xPoint.set_z(xID.z);
        return xPoint;
    }

    static bool DataToPBProperty(const AFIData& DataVar, const char* name, AFMsg::PropertyPBData& xMsg)
    {
        if(nullptr == name)
        {
            return false;
        }

        xMsg.set_property_name(name);
        xMsg.set_ndatatype(DataVar.GetType());
        switch(DataVar.GetType())
        {
        case DT_BOOLEAN:
            {
                xMsg.set_mbvalue(DataVar.GetBool());
            }
            break;
        case DT_INT:
            {
                xMsg.set_mnvalue(DataVar.GetInt());
            }
            break;
        case DT_INT64:
            {
                xMsg.set_mn64value(DataVar.GetInt64());
            }
            break;
        case DT_FLOAT:
            {

                xMsg.set_mfvalue(DataVar.GetFloat());
            }
            break;

        case DT_DOUBLE:
            {

                xMsg.set_mdvalue(DataVar.GetDouble());
            }
            break;

        case DT_STRING:
            {

                xMsg.set_mstrvalue(DataVar.GetString());
            }
            break;

        case DT_OBJECT:
            {

                *xMsg.mutable_mguid() = NFToPB(DataVar.GetObject());
            }
            break;
        default:
            break;
        }

        return true;
    }


    static bool RecordToPBRecord(const AFIData& DataVar, const int nRow, const int nCol, AFMsg::RecordPBData& xMsg)
    {
        xMsg.set_col(nCol);
        xMsg.set_row(nRow);
        xMsg.set_ndatatype(DataVar.GetType());
        switch(DataVar.GetType())
        {
        case DT_BOOLEAN:
            {
                xMsg.set_mbvalue(DataVar.GetBool());
            }
            break;
        case DT_INT:
            {
                xMsg.set_mnvalue(DataVar.GetInt());
            }
            break;
        case DT_INT64:
            {
                xMsg.set_mn64value(DataVar.GetInt64());
            }
            break;
        case DT_FLOAT:
            {

                xMsg.set_mfvalue(DataVar.GetFloat());
            }
            break;

        case DT_DOUBLE:
            {

                xMsg.set_mdvalue(DataVar.GetDouble());
            }
            break;

        case DT_STRING:
            {

                xMsg.set_mstrvalue(DataVar.GetString());
            }
            break;

        case DT_OBJECT:
            {

                *xMsg.mutable_mguid() = NFToPB(DataVar.GetObject());
            }
            break;
        default:
            break;
        }

        return true;
    }

    static bool RecordToPBRecord(const AFIDataList& DataList, const int nRow, const int nCol, AFMsg::RecordPBData& xMsg)
    {
        xMsg.set_col(nCol);
        xMsg.set_row(nRow);
        xMsg.set_ndatatype(DataList.GetType(nCol));
        switch(DataList.GetType(nCol))
        {
        case DT_BOOLEAN:
            {
                xMsg.set_mbvalue(DataList.Bool(nCol));
            }
            break;
        case DT_INT:
            {
                xMsg.set_mnvalue(DataList.Int(nCol));
            }
            break;
        case DT_INT64:
            {
                xMsg.set_mn64value(DataList.Int64(nCol));
            }
            break;
        case DT_FLOAT:
            {

                xMsg.set_mfvalue(DataList.Float(nCol));
            }
            break;

        case DT_DOUBLE:
            {

                xMsg.set_mdvalue(DataList.Double(nCol));
            }
            break;

        case DT_STRING:
            {

                xMsg.set_mstrvalue(DataList.String(nCol));
            }
            break;

        case DT_OBJECT:
            {

                *xMsg.mutable_mguid() = NFToPB(DataList.Object(nCol));
            }
            break;
        default:
            break;
        }

        return true;
    }

protected:
    void OnReceiveBaseNetPack(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        std::map<int, NET_RECEIVE_FUNCTOR_PTR>::iterator it = mxReceiveCallBack.find(nMsgID);
        if(mxReceiveCallBack.end() != it)
        {
            NET_RECEIVE_FUNCTOR_PTR& pFunPtr = it->second;
            NET_RECEIVE_FUNCTOR* pFunc = pFunPtr.get();
            pFunc->operator()(nMsgID, msg, nLen, xClientID);
        }
        else
        {
            for(std::list<NET_RECEIVE_FUNCTOR_PTR>::iterator it = mxCallBackList.begin(); it != mxCallBackList.end(); ++it)
            {
                NET_RECEIVE_FUNCTOR_PTR& pFunPtr = *it;
                NET_RECEIVE_FUNCTOR* pFunc = pFunPtr.get();
                pFunc->operator()(nMsgID, msg, nLen, xClientID);
            }
        }
    }

    void OnSocketBaseNetEvent(const NetEventType eEvent, const AFGUID& xClientID, int nServerID)
    {
        for(std::list<NET_EVENT_FUNCTOR_PTR>::iterator it = mxEventCallBackList.begin(); it != mxEventCallBackList.end(); ++it)
        {
            NET_EVENT_FUNCTOR_PTR& pFunPtr = *it;
            NET_EVENT_FUNCTOR* pFunc = pFunPtr.get();
            pFunc->operator()(eEvent, xClientID, nServerID);
        }
    }

protected:
    std::map<int, NET_RECEIVE_FUNCTOR_PTR> mxReceiveCallBack;
    std::list<NET_EVENT_FUNCTOR_PTR> mxEventCallBackList;
    std::list<NET_RECEIVE_FUNCTOR_PTR> mxCallBackList;
};

#endif


