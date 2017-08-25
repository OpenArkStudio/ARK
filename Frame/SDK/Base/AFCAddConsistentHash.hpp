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
// * @FileName  AFCAddConsistentHash.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFCAddConsistentHash                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFCAddConsistentHash                                                  *
*****************************************************************************/
#include <map>
#include <string>
#include <list>
#include <functional> 
#include <algorithm>
#include "AFCConsistentHash.hpp"

#ifndef AFC_ADDCONSISTENT_HASH_H
#define AFC_ADDCONSISTENT_HASH_H

/**
 * @class   AFCAddConsistentHash
 *
 * @brief   A nfc add consistent hash.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class AFCAddConsistentHash : public AFCConsistentHash
{
public:

    /**
     * @fn  AFCAddConsistentHash::AFCAddConsistentHash()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    AFCAddConsistentHash()
    {
    }

    /**
     * @fn  virtual AFCAddConsistentHash::~AFCAddConsistentHash()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    virtual ~AFCAddConsistentHash()
    {
    }

public:
   //添加备胎

    /**
     * @fn  void AFCAddConsistentHash::AddCandidateMachine(const int nServerID)
     *
     * @brief   Adds a candidate machine.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nServerID   Identifier for the server.
     */

	void AddCandidateMachine(const int nServerID) 
	{
		std::list<AFCMachineNode> xNodeList;
		GetNodeList(xNodeList);

		//把所有的真实主机作为备份
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

    /**
     * @fn  void AFCAddConsistentHash::IntanceCandidateMachine()
     *
     * @brief   实例话一个备胎.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

	void IntanceCandidateMachine()
	{

	}

};


#endif

