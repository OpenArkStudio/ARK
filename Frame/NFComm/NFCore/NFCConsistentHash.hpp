// -------------------------------------------------------------------------
//    @FileName         :    NFCConsistentHash.hpp
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFCConsistentHash.hpp
//
// -------------------------------------------------------------------------
#ifndef NFC_CONSISTENT_HASH_H
#define NFC_CONSISTENT_HASH_H

#include <map>
#include <string>
#include <list>
#include <functional> 
#include <algorithm>
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "common/crc32.hpp"

/**
 * @class   NFIVirtualNode
 *
 * @brief   虚拟节点.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFIVirtualNode 
{
public:

    /**
     * @fn  NFIVirtualNode::NFIVirtualNode(const int nVirID)
     *
     * @brief   主机IP，此主机的第几个虚节点序号.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nVirID  Identifier for the vir.
     */

    NFIVirtualNode(const int nVirID)
        :nVirtualIndex(nVirID)
    {

    }

    /**
     * @fn  NFIVirtualNode::NFIVirtualNode()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

	NFIVirtualNode()
	{
		nVirtualIndex = 0;
	}

    /**
     * @fn  virtual NFIVirtualNode::~NFIVirtualNode()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

	virtual ~NFIVirtualNode()
	{
		nVirtualIndex = 0;
	}

    /**
     * @fn  virtual std::string NFIVirtualNode::GetDataStr() const
     *
     * @brief   Gets data string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The data string.
     */

	virtual std::string GetDataStr() const
	{
		return "";
	}

    /**
     * @fn  virtual int NFIVirtualNode::GetDataID() const
     *
     * @brief   Gets data identifier.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The data identifier.
     */

	virtual int GetDataID() const
	{
		return 0;
	}

    /**
     * @fn  virtual bool NFIVirtualNode::Candidate() const
     *
     * @brief   Queries if we candidate.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  True if it succeeds, false if it fails.
     */

	virtual bool Candidate() const
	{
		return false;
	};

    /**
     * @fn  std::string NFIVirtualNode::ToStr() const
     *
     * @brief   Converts this object to a string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  This object as a std::string.
     */

    std::string ToStr() const 
    {
        std::ostringstream strInfo;
        strInfo << lexical_cast<std::string>(GetDataID()) << "-" << GetDataStr() << "-" << nVirtualIndex;
        return strInfo.str();
        //return boost::str(boost::format("%1%-%2%-%3%") % lexical_cast<std::string>(GetDataID()) % GetDataStr() % nVirtualIndex);
    }

private:
    /** @brief   虚拟节点序号. */
    int nVirtualIndex;
};

/**
 * @class   NFCMachineNode
 *
 * @brief   A nfc machine node.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFCMachineNode : public NFIVirtualNode
{
public:

    /**
     * @fn  NFCMachineNode::NFCMachineNode(const int nVirID)
     *
     * @brief   Constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nVirID  Identifier for the vir.
     */

	NFCMachineNode(const int nVirID) : NFIVirtualNode(nVirID)
	{
		strIP = "";
		nPort = 0;
		nWeight = 0;//总共多少权重即是多少虚拟节点
		nMachineID = 0;
		bCandidate = false;
	}

    /**
     * @fn  NFCMachineNode::NFCMachineNode()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

	NFCMachineNode()
	{
		strIP = "";
		nPort = 0;
		nWeight = 0;//总共多少权重即是多少虚拟节点
		nMachineID = 0;
		bCandidate = false;
	}

public:

    /**
     * @fn  virtual std::string NFCMachineNode::GetDataStr() const
     *
     * @brief   Gets data string.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The data string.
     */

	virtual std::string GetDataStr() const 
	{
		return strIP;
	}

    /**
     * @fn  virtual int NFCMachineNode::GetDataID() const
     *
     * @brief   Gets data identifier.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The data identifier.
     */

	virtual int GetDataID() const
	{
		return nMachineID;
	}

    /**
     * @fn  virtual bool NFCMachineNode::Candidate() const
     *
     * @brief   Queries if we candidate.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  True if it succeeds, false if it fails.
     */

	virtual bool Candidate() const
	{
		return bCandidate;
	}

	/** @brief   The IP. */
	std::string strIP;
	/** @brief   The port. */
	int nPort;
	/** @brief   The weight. */
	int nWeight;
	/** @brief   Identifier for the machine. */
	int nMachineID;
	/** @brief   True to candidate. */
	bool bCandidate;

	/** @brief   如果是候选主机，则在没启用之前，他会指向真实主机. */
	std::list<NFIVirtualNode> xRealMachine;
};

/**
 * @class   NFIHasher
 *
 * @brief   A nfi hasher.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class NFIHasher
{
public:

    /**
     * @fn  virtual uint32_t NFIHasher::GetHashValue(const NFIVirtualNode& vNode) = 0;
     *
     * @brief   Gets hash value.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   vNode   The node.
     *
     * @return  The hash value.
     */

    virtual uint32_t GetHashValue(const NFIVirtualNode& vNode) = 0;
};

/**
 * @class   NFCHasher
 *
 * @brief   A nfc hasher.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class NFCHasher : public NFIHasher
{
public:

    /**
     * @fn  virtual uint32_t NFCHasher::GetHashValue(const NFIVirtualNode& vNode)
     *
     * @brief   Gets hash value.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   vNode   The node.
     *
     * @return  The hash value.
     */

    virtual uint32_t GetHashValue(const NFIVirtualNode& vNode)
    {
        //boost::crc_32_type ret;
        //std::string vnode = vNode.ToStr();
        //ret.process_bytes(vnode.c_str(), vnode.size());
        
        //return ret.checksum();

        std::string vnode = vNode.ToStr();
        return NFrame::CRC32(vnode);
    }
};

/**
 * @class   NFIConsistentHash
 *
 * @brief   A nfi consistent hash.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class NFIConsistentHash
{
    /**
     * @fn  virtual std::size_t NFIConsistentHash::Size() const = 0;
     *
     * @brief   Gets the size.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  A std::size_t.
     */

	virtual std::size_t Size() const = 0;

    /**
     * @fn  virtual bool NFIConsistentHash::Empty() const = 0;
     *
     * @brief   Empties this object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  True if it succeeds, false if it fails.
     */

	virtual bool Empty() const = 0;

    /**
     * @fn  virtual void NFIConsistentHash::Insert(const int nID, const std::string& strIP, int nPort) = 0;
     *
     * @brief   Inserts.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nID     The identifier.
     * @param   strIP   The IP.
     * @param   nPort   The port.
     */

	virtual void Insert(const int nID, const std::string& strIP, int nPort) = 0;

    /**
     * @fn  virtual void NFIConsistentHash::Insert(const NFCMachineNode& xNode) = 0;
     *
     * @brief   Inserts the given x coordinate node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   xNode   The node.
     */

	virtual void Insert(const NFCMachineNode& xNode) = 0;

    /**
     * @fn  virtual bool NFIConsistentHash::Exist(const NFCMachineNode& xInNode) = 0;
     *
     * @brief   Exists the given x coordinate in node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   xInNode The in node.
     *
     * @return  True if it succeeds, false if it fails.
     */

	virtual bool Exist(const NFCMachineNode& xInNode) = 0;

    /**
     * @fn  virtual std::size_t NFIConsistentHash::Erase(const NFCMachineNode& xNode) = 0;
     *
     * @brief   Erases the given x coordinate node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   xNode   The node.
     *
     * @return  A std::size_t.
     */

	virtual std::size_t Erase(const NFCMachineNode& xNode)  = 0;

    /**
     * @fn  virtual bool NFIConsistentHash::GetSuitNode(NFCMachineNode& node) = 0;
     *
     * @brief   Gets suit node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param [in,out]  node    The node.
     *
     * @return  True if it succeeds, false if it fails.
     */

	virtual bool GetSuitNode(NFCMachineNode& node) = 0;

    /**
     * @fn  virtual bool NFIConsistentHash::GetSuitNode(const std::string& str, NFCMachineNode& node) = 0;
     *
     * @brief   Gets suit node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           str     The string.
     * @param [in,out]  node    The node.
     *
     * @return  True if it succeeds, false if it fails.
     */

	virtual bool GetSuitNode(const std::string& str, NFCMachineNode& node) = 0;

    /**
     * @fn  virtual bool NFIConsistentHash::GetSuitNode(uint32_t hashValue, NFCMachineNode& node) = 0;
     *
     * @brief   Gets suit node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           hashValue   The hash value.
     * @param [in,out]  node        The node.
     *
     * @return  True if it succeeds, false if it fails.
     */

	virtual bool GetSuitNode(uint32_t hashValue, NFCMachineNode& node) = 0;

    /**
     * @fn  virtual bool NFIConsistentHash::GetNodeList(std::list<NFCMachineNode>& nodeList) = 0;
     *
     * @brief   Gets node list.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param [in,out]  nodeList    List of nodes.
     *
     * @return  True if it succeeds, false if it fails.
     */

	virtual bool GetNodeList(std::list<NFCMachineNode>& nodeList) = 0;
};

/**
 * @class   NFCConsistentHash
 *
 * @brief   A nfc consistent hash.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class NFCConsistentHash
{
public:

public:

    /**
     * @fn  NFCConsistentHash::NFCConsistentHash()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    NFCConsistentHash()
    {
        m_pHasher = new NFCHasher();
    }

    /**
     * @fn  virtual NFCConsistentHash::~NFCConsistentHash()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    virtual ~NFCConsistentHash()
    {
        delete m_pHasher;
        m_pHasher = NULL;
    }

public:

    /**
     * @fn  std::size_t NFCConsistentHash::Size() const
     *
     * @brief   Gets the size.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  A std::size_t.
     */

    std::size_t Size() const
    {
        return mxNodes.size();
    }

    /**
     * @fn  bool NFCConsistentHash::Empty() const
     *
     * @brief   Empties this object.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool Empty() const 
    {
        return mxNodes.empty();
    }

    /**
     * @fn  void NFCConsistentHash::Insert(const int nID, const std::string& strIP, int nPort)
     *
     * @brief   Inserts.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nID     The identifier.
     * @param   strIP   The IP.
     * @param   nPort   The port.
     */

	void Insert(const int nID, const std::string& strIP, int nPort) 
	{
		NFCMachineNode xNode;
		xNode.nMachineID = nID;
		xNode.strIP = strIP;
		xNode.nPort = nPort;

		Insert(xNode);
	}

    /**
     * @fn  void NFCConsistentHash::Insert(const NFCMachineNode& xNode)
     *
     * @brief   Inserts the given x coordinate node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   xNode   The node.
     */

    void Insert(const NFCMachineNode& xNode) 
    {
        uint32_t hash = m_pHasher->GetHashValue(xNode);
        auto it = mxNodes.find(hash);
        if (it == mxNodes.end())
        {
            mxNodes.insert(std::map<uint32_t, NFCMachineNode>::value_type(hash, xNode));
        }
    }

    /**
     * @fn  bool NFCConsistentHash::Exist(const NFCMachineNode& xInNode)
     *
     * @brief   Exists the given x coordinate in node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   xInNode The in node.
     *
     * @return  True if it succeeds, false if it fails.
     */

	bool Exist(const NFCMachineNode& xInNode)
	{
		uint32_t hash = m_pHasher->GetHashValue(xInNode);
		std::map<uint32_t, NFCMachineNode>::iterator it = mxNodes.find(hash);
		if (it != mxNodes.end())
		{
			return true;
		}

		return false;
	}

    /**
     * @fn  std::size_t NFCConsistentHash::Erase(const NFCMachineNode& xNode)
     *
     * @brief   Erases the given x coordinate node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   xNode   The node.
     *
     * @return  A std::size_t.
     */

    std::size_t Erase(const NFCMachineNode& xNode) 
    {
        uint32_t hash = m_pHasher->GetHashValue(xNode);
        return mxNodes.erase(hash);
    }

    /**
     * @fn  bool NFCConsistentHash::GetSuitNode(NFCMachineNode& node)
     *
     * @brief   Gets suit node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param [in,out]  node    The node.
     *
     * @return  True if it succeeds, false if it fails.
     */

	bool GetSuitNode(NFCMachineNode& node)
	{
		int nID = 0;
		return GetSuitNode(nID, node);
	}

    /**
     * @fn  bool NFCConsistentHash::GetSuitNode(const std::string& str, NFCMachineNode& node)
     *
     * @brief   Gets suit node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           str     The string.
     * @param [in,out]  node    The node.
     *
     * @return  True if it succeeds, false if it fails.
     */

	bool GetSuitNode(const std::string& str, NFCMachineNode& node)
	{
		uint32_t nCRC32 = NFrame::CRC32(str);
        return GetSuitNode(nCRC32, node);
	}

    /**
     * @fn  bool NFCConsistentHash::GetSuitNode(uint32_t hashValue, NFCMachineNode& node)
     *
     * @brief   Gets suit node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param           hashValue   The hash value.
     * @param [in,out]  node        The node.
     *
     * @return  True if it succeeds, false if it fails.
     */

	bool GetSuitNode(uint32_t hashValue, NFCMachineNode& node)
	{
		if(mxNodes.empty())
		{
			return false;
		}

		std::map<uint32_t, NFCMachineNode>::iterator it = mxNodes.lower_bound(hashValue);

		if (it == mxNodes.end())
		{
			it = mxNodes.begin();
		}

		node = it->second;

		return true;
	}

    /**
     * @fn  bool NFCConsistentHash::GetNodeList(std::list<NFCMachineNode>& nodeList)
     *
     * @brief   Gets node list.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param [in,out]  nodeList    List of nodes.
     *
     * @return  True if it succeeds, false if it fails.
     */

	bool GetNodeList(std::list<NFCMachineNode>& nodeList)
	{
		for (std::map<uint32_t, NFCMachineNode>::iterator it = mxNodes.begin(); it != mxNodes.end(); ++it)
		{
			nodeList.push_back(it->second);
		}

		return true;
	}

private:
	/** @brief   The mx nodes. */
	std::map<uint32_t, NFCMachineNode> mxNodes;
    /** @brief   The hasher. */
    NFIHasher* m_pHasher;
};


#endif
