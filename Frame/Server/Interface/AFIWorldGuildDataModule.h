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
#include "SDK/Proto/NFProtocolDefine.hpp"
#include "SDK/Proto/NFDefine.pb.h"

namespace ark
{

    class AFIWorldGuildDataModule : public AFIModule
    {
    public:
        struct SearchGuildObject
        {
            SearchGuildObject()
            {
                mnGuildMemberCount = 0;
                mnGuildMemberMaxCount = 0;
                mnGuildHonor = 0;
                mnGuildRank = 0;
            }

            AFGUID          mxGuildID;
            std::string     mstrGuildName;
            std::string     mnGuildIcon;
            int32_t         mnGuildMemberCount;
            int32_t         mnGuildMemberMaxCount;
            int32_t         mnGuildHonor;
            int32_t         mnGuildRank;
        };

        virtual bool ExitGuild(const NFGUID& self, const std::string& strName, bool& bExit) = 0;
        virtual void CheckLoadGuild(const NFGUID& self, const NFGUID& xGuild) = 0;
        virtual bool GetPlayerInfo(const NFGUID& self, std::string& strRoleName, int& nLevel, int& nJob, int& nDonation, int& nVIP) = 0;

        virtual const AFGUID CreateGuild(const NFGUID& xPlayeID, const std::string& strName, const std::string& strRoleName,
                                         const int nLevel, const int nJob, const int nDonation, const int nVIP, const int nOffLine = 1, const int nPower = AFMsg::GUILD_POWER_TYPE_PRESIDENT) = 0;
        virtual const bool DeleteGuild(const NFGUID& xGuild) = 0;
        virtual ARK_SHARE_PTR<AFIEntity> GetGuild(const AFGUID& xGuild) = 0;

        virtual bool GetPlayerGuild(const NFGUID& self, AFGUID& xGuild) = 0;
        virtual bool SearchGuild(const NFGUID& self, const std::string& strName, std::vector<SearchGuildObject>& xList) = 0;
        virtual bool GetPlayerGameID(const NFGUID& self, int& nGameID) = 0;
    };

}