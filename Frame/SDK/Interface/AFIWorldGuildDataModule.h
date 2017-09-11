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
// * @file  	AFIWorldGuildDataModule.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFIWorldGuildDataModule                                                  *
*****************************************************************************/
#ifndef AFI_WORLD_GUILD_DATA_MODULE_H
#define AFI_WORLD_GUILD_DATA_MODULE_H

#include "AFILogicModule.h"
#include "SDK/Proto/NFProtocolDefine.hpp"
#include "SDK/Proto/NFDefine.pb.h"

class AFIWorldGuildDataModule
    : public AFILogicModule
{
public:
    struct SearchGuildObject
    {
        SearchGuildObject()
        {
            mnGuildMemberCount  = 0;
            mnGuildMemberMaxCount  = 0;
            mnGuildHonor = 0;
            mnGuildRank  = 0;
        }

        NFGUID      mxGuildID    ;
        std::string     mstrGuildName  ;
        std::string     mnGuildIcon  ;
        AFINT32         mnGuildMemberCount  ;
        AFINT32         mnGuildMemberMaxCount  ;
        AFINT32         mnGuildHonor ;
        AFINT32         mnGuildRank  ;
    };

    virtual bool ExitGuild(const NFGUID& self, const std::string& strName, bool& bExit) = 0;
    virtual void CheckLoadGuild(const NFGUID& self, const NFGUID& xGuild) = 0;
    virtual bool GetPlayerInfo(const NFGUID& self, std::string& strRoleName, int& nLevel, int& nJob , int& nDonation , int& nVIP) = 0;

    virtual const NFGUID CreateGuild(const NFGUID& xPlayeID, const std::string& strName, const std::string& strRoleName,
                                     const int nLevel, const int nJob , const int nDonation , const int nVIP, const int nOffLine = 1, const int nPower = NFMsg::GUILD_POWER_TYPE_PRESIDENT) = 0;
    virtual const bool DeleteGuild(const NFGUID& xGuild) = 0;
    virtual ARK_SHARE_PTR<AFIObject> GetGuild(const NFGUID& xGuild) = 0;

    virtual bool GetPlayerGuild(const NFGUID& self, NFGUID& xGuild) = 0;
    virtual bool SearchGuild(const NFGUID& self, const std::string& strName, std::vector<SearchGuildObject>& xList) = 0;
    virtual bool GetPlayerGameID(const NFGUID& self, int& nGameID) = 0;

protected:
private:
};

#endif

