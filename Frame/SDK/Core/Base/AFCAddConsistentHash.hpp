/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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

#include "AFPlatform.hpp"
#include "AFCConsistentHash.hpp"

class AFCAddConsistentHash : public AFCConsistentHash
{
public:

    AFCAddConsistentHash()
    {
    }

    virtual ~AFCAddConsistentHash()
    {
    }

public:

    void AddCandidateMachine(const int nServerID)
    {
        std::list<AFCMachineNode> xNodeList;

        if (GetNodeList(xNodeList))
        {
            return;
        }

        //鎶婃墍鏈夌殑鐪熷疄涓绘満浣滀负澶囦唤
        AFCMachineNode xNode;
        xNode.nMachineID = nServerID;
        xNode.strIP = "";
        xNode.nPort = 0;
        xNode.bCandidate = true;

        for (std::list<AFCMachineNode>::iterator it = xNodeList.begin(); it != xNodeList.end(); ++it)
        {
            AFIVirtualNode&  xRealNode = *it;
            if (!xRealNode.Candidate())
            {
                xNode.xRealMachine.push_back(xRealNode);
            }
        }

        Insert(xNode);
    }

    void IntanceCandidateMachine()
    {

    }

};