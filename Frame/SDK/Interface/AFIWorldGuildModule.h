// -------------------------------------------------------------------------
//    @FileName      :    AFIWorldGuildModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2015-05-24 08:51
//    @Module           :   AFIWorldGuildModule
//
// -------------------------------------------------------------------------

#ifndef AFI_WORLD_GUILD_MODULE_H
#define AFI_WORLD_GUILD_MODULE_H

#include "AFILogicModule.h"

class AFIWorldGuildModule
    : public AFILogicModule
{
public:
    virtual const NFGUID& CreateGuild(const NFGUID& self, const std::string& strName, const std::string& strRoleName, const int nLevel, const int nJob , const int nDonation , const int nVIP) = 0;
    virtual bool JoinGuild(const NFGUID& self, const NFGUID& xGuildID) = 0;
    virtual bool LeaveGuild(const NFGUID& self, const NFGUID& xGuildID) = 0;
    virtual bool UpGuildMmember(const NFGUID& self, const NFGUID& xGuildID, const NFGUID& xMmember) = 0;
    virtual bool DownGuildMmember(const NFGUID& self, const NFGUID& xGuildID, const NFGUID& xMmember) = 0;
    virtual bool KickGuildMmember(const NFGUID& self, const NFGUID& xGuildID, const NFGUID& xMmember) = 0;

    virtual bool GetGuildBaseInfo(const NFGUID& self, const NFGUID& xGuildID) = 0;
    virtual bool GetGuildMemberInfo(const NFGUID& self, const NFGUID& xGuildID) = 0;
    virtual bool GetGuildMemberInfo(const NFGUID& self, const NFGUID& xGuildID, const NFGUID& xMmember) = 0;

    virtual bool GetOnlineMember(const NFGUID& self, const NFGUID& xGuild, AFCDataList& varMemberList, AFCDataList& varGameList) = 0;
    virtual bool MemberOnline(const NFGUID& self, const NFGUID& xGuild, const int& nGameID) = 0;
    virtual bool MemberOffeline(const NFGUID& self, const NFGUID& xGuild) = 0;

private:
};

#endif