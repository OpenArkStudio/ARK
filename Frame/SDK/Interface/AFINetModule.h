// -------------------------------------------------------------------------
//    @FileName         £º    AFINetModule.h
//    @Author           £º    Ark Game Tech
//    @Date             £º    2012-12-15
//    @Module           £º    AFINetModule
//
// -------------------------------------------------------------------------

#ifndef AFI_NET_MODULE_H
#define AFI_NET_MODULE_H

#include <iostream>
#include <iosfwd>
//#include "SDK/Core/AFGUID.h"
#include "AFIModule.h"
#include "AFIPluginManager.h"
#include "SDK/NetEvpp/AFCNetServer.h"
//#include "SDK/Core/AFQueue.h"
#include "SDK/Proto/NFMsgDefine.h"
#include "SDK/Proto/NFDefine.pb.h"

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
        NFMsg::MsgBase xMsg;
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
        NFMsg::MsgBase xMsg;
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

    static AFGUID PBToNF(NFMsg::Ident xID)
    {
        AFGUID  xIdent;
        xIdent.nSerial = xID.svrid();
        xIdent.nIdent = xID.index();

        return xIdent;
    }

    static NFMsg::Ident NFToPB(AFGUID xID)
    {
        NFMsg::Ident  xIdent;
        xIdent.set_svrid(xID.nSerial);
        xIdent.set_index(xID.nIdent);

        return xIdent;
    }

    static Point3D PBToNF(NFMsg::Point3D xPoint)
    {
        Point3D xID;
        xID.x = xPoint.x();
        xID.y = xPoint.y();
        xID.z = xPoint.z();
        return xID;
    }

    static NFMsg::Point3D NFToPB(Point3D xID)
    {
        NFMsg::Point3D xPoint;
        xPoint.set_x(xID.x);
        xPoint.set_y(xID.y);
        xPoint.set_z(xID.z);
        return xPoint;
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
