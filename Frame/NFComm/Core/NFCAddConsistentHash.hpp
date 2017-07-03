// -------------------------------------------------------------------------
//    @FileName         :    NFCAddConsistentHash.hpp
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFCAddConsistentHash.hpp
//
// -------------------------------------------------------------------------

#include <map>
#include <string>
#include <list>
#include <functional> 
#include <algorithm>
#include "NFCConsistentHash.hpp"

#ifndef NFC_ADDCONSISTENT_HASH_H
#define NFC_ADDCONSISTENT_HASH_H

/**
 * @class   NFCAddConsistentHash
 *
 * @brief   A nfc add consistent hash.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFCAddConsistentHash : public NFCConsistentHash
{
public:

    /**
     * @fn  NFCAddConsistentHash::NFCAddConsistentHash()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    NFCAddConsistentHash()
    {
    }

    /**
     * @fn  virtual NFCAddConsistentHash::~NFCAddConsistentHash()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    virtual ~NFCAddConsistentHash()
    {
    }

public:
   //添加备胎

    /**
     * @fn  void NFCAddConsistentHash::AddCandidateMachine(const int nServerID)
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
		std::list<NFCMachineNode> xNodeList;
		GetNodeList(xNodeList);

		//把所有的真实主机作为备份
		NFCMachineNode xNode;
		xNode.nMachineID = nServerID;
		xNode.strIP = "";
		xNode.nPort = 0;
		xNode.bCandidate = true;

		for (std::list<NFCMachineNode>::iterator it = xNodeList.begin(); it != xNodeList.end(); ++it)
		{
			NFIVirtualNode&  xRealNode = *it;
			if (!xRealNode.Candidate())
			{
				xNode.xRealMachine.push_back(xRealNode);
			}
		}

		Insert(xNode);
	}

    /**
     * @fn  void NFCAddConsistentHash::IntanceCandidateMachine()
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
