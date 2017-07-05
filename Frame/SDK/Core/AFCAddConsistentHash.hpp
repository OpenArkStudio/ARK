// -------------------------------------------------------------------------
//    @FileName         :    AFCAddConsistentHash.hpp
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    AFCAddConsistentHash.hpp
//
// -------------------------------------------------------------------------

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
