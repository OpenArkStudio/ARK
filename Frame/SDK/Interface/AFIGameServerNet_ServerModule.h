// -------------------------------------------------------------------------
//    @FileName         ：    AFIGameServerNet_ServerModule.h
//    @Author           ：    Ark Game Tech
//    @Date             ：    2012-12-15
//    @Module           ：    AFIGameServerNet_ServerModule
//
// -------------------------------------------------------------------------

#ifndef AFI_GAMESERVERNET_SERVERMODULE_H
#define AFI_GAMESERVERNET_SERVERMODULE_H

#include <iostream>
#include "AFIModule.h"
#include "AFINetServerModule.h"

class AFIGameServerNet_ServerModule
    : public AFIModule
{
public:
    //要管理当前所有的对象所在的actor,gateid,fd等
    struct GateBaseInfo
    {
        GateBaseInfo()
        {
            nActorID = 0;
            nGateID = 0;
        }

        GateBaseInfo(const int gateID, const AFGUID xIdent)
        {
            nActorID = 0;
            nGateID = gateID;
            xClientID = xIdent;
        }

        int nActorID;
        int nGateID;
        AFGUID xClientID;
    };

    struct GateServerInfo
    {
        ServerData xServerData;
        //此网关上所有的对象<角色ID,gate_FD>
        std::map<AFGUID, AFGUID> xRoleInfo;
    };

public:
    virtual AFINetServerModule* GetNetModule() = 0;
    virtual void SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const AFGUID& self) = 0;
    virtual void SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const AFGUID& self) = 0;
    virtual bool AddPlayerGateInfo(const AFGUID& nRoleID, const AFGUID& nClientID, const int nGateID) = 0;
    virtual bool RemovePlayerGateInfo(const AFGUID& nRoleID) = 0;
    virtual NF_SHARE_PTR<GateBaseInfo> GetPlayerGateInfo(const AFGUID& nRoleID) = 0;

    virtual int OnPropertyEnter(const AFIDataList& argVar, const AFGUID& self) = 0;
    virtual int OnRecordEnter(const AFIDataList& argVar, const AFGUID& self) = 0;

    virtual int OnObjectListEnter(const AFIDataList& self, const AFIDataList& argVar) = 0;
    virtual int OnObjectListLeave(const AFIDataList& self, const AFIDataList& argVar) = 0;
};

#endif