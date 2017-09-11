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

#include "SDK/Base/AFPlatform.hpp"
#include "common/crc32.hpp"
#include "AFMacros.hpp"

/**
 * @class   AFIVirtualNode
 *
 * @brief   虚拟节点.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class AFIVirtualNode
{
public:

    /**
     * @fn  AFIVirtualNode::AFIVirtualNode(const int nVirID)
     *
     * @brief   主机IP，此主机的第几个虚节点序号.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nVirID  Identifier for the vir.
     */

    AFIVirtualNode(const int nVirID)
        : nVirtualIndex(nVirID)
    {

    }

    /**
     * @fn  AFIVirtualNode::AFIVirtualNode()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    AFIVirtualNode()
    {
        nVirtualIndex = 0;
    }

    /**
     * @fn  virtual AFIVirtualNode::~AFIVirtualNode()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    virtual ~AFIVirtualNode()
    {
        nVirtualIndex = 0;
    }

    /**
     * @fn  virtual std::string AFIVirtualNode::GetDataStr() const
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
     * @fn  virtual int AFIVirtualNode::GetDataID() const
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
     * @fn  virtual bool AFIVirtualNode::Candidate() const
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
     * @fn  std::string AFIVirtualNode::ToStr() const
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
        strInfo << ARK_LEXICAL_CAST<std::string>(GetDataID()) << "-" << GetDataStr() << "-" << nVirtualIndex;
        return strInfo.str();
        //return boost::str(boost::format("%1%-%2%-%3%") % ARK_LEXICAL_CAST<std::string>(GetDataID()) % GetDataStr() % nVirtualIndex);
    }

private:
    /** @brief   虚拟节点序号. */
    int nVirtualIndex;
};

/**
 * @class   AFCMachineNode
 *
 * @brief   A nfc machine node.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class AFCMachineNode : public AFIVirtualNode
{
public:

    /**
     * @fn  AFCMachineNode::AFCMachineNode(const int nVirID)
     *
     * @brief   Constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   nVirID  Identifier for the vir.
     */

    AFCMachineNode(const int nVirID) : AFIVirtualNode(nVirID)
    {
        strIP = "";
        nPort = 0;
        nWeight = 0;//总共多少权重即是多少虚拟节点
        nMachineID = 0;
        bCandidate = false;
    }

    /**
     * @fn  AFCMachineNode::AFCMachineNode()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    AFCMachineNode()
    {
        strIP = "";
        nPort = 0;
        nWeight = 0;//总共多少权重即是多少虚拟节点
        nMachineID = 0;
        bCandidate = false;
    }

public:

    /**
     * @fn  virtual std::string AFCMachineNode::GetDataStr() const
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
     * @fn  virtual int AFCMachineNode::GetDataID() const
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
     * @fn  virtual bool AFCMachineNode::Candidate() const
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
    std::list<AFIVirtualNode> xRealMachine;
};

/**
 * @class   AFIHasher
 *
 * @brief   A nfi hasher.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class AFIHasher
{
public:

    /**
     * @fn  virtual uint32_t AFIHasher::GetHashValue(const AFIVirtualNode& vNode) = 0;
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

    virtual uint32_t GetHashValue(const AFIVirtualNode& vNode) = 0;
};

/**
 * @class   AFCHasher
 *
 * @brief   A nfc hasher.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class AFCHasher : public AFIHasher
{
public:

    /**
     * @fn  virtual uint32_t AFCHasher::GetHashValue(const AFIVirtualNode& vNode)
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

    virtual uint32_t GetHashValue(const AFIVirtualNode& vNode)
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
 * @class   AFIConsistentHash
 *
 * @brief   A nfi consistent hash.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class AFIConsistentHash
{
    /**
     * @fn  virtual std::size_t AFIConsistentHash::Size() const = 0;
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
     * @fn  virtual bool AFIConsistentHash::Empty() const = 0;
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
     * @fn  virtual void AFIConsistentHash::Insert(const int nID, const std::string& strIP, int nPort) = 0;
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
     * @fn  virtual void AFIConsistentHash::Insert(const AFCMachineNode& xNode) = 0;
     *
     * @brief   Inserts the given x coordinate node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   xNode   The node.
     */

    virtual void Insert(const AFCMachineNode& xNode) = 0;

    /**
     * @fn  virtual bool AFIConsistentHash::Exist(const AFCMachineNode& xInNode) = 0;
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

    virtual bool Exist(const AFCMachineNode& xInNode) = 0;

    /**
     * @fn  virtual std::size_t AFIConsistentHash::Erase(const AFCMachineNode& xNode) = 0;
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

    virtual std::size_t Erase(const AFCMachineNode& xNode)  = 0;

    /**
     * @fn  virtual bool AFIConsistentHash::GetSuitNode(AFCMachineNode& node) = 0;
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

    virtual bool GetSuitNode(AFCMachineNode& node) = 0;

    /**
     * @fn  virtual bool AFIConsistentHash::GetSuitNode(const std::string& str, AFCMachineNode& node) = 0;
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

    virtual bool GetSuitNode(const std::string& str, AFCMachineNode& node) = 0;

    /**
     * @fn  virtual bool AFIConsistentHash::GetSuitNode(uint32_t hashValue, AFCMachineNode& node) = 0;
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

    virtual bool GetSuitNode(uint32_t hashValue, AFCMachineNode& node) = 0;

    /**
     * @fn  virtual bool AFIConsistentHash::GetNodeList(std::list<AFCMachineNode>& nodeList) = 0;
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

    virtual bool GetNodeList(std::list<AFCMachineNode>& nodeList) = 0;
};

/**
 * @class   AFCConsistentHash
 *
 * @brief   A nfc consistent hash.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class AFCConsistentHash
{
public:

public:

    /**
     * @fn  AFCConsistentHash::AFCConsistentHash()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    AFCConsistentHash()
    {
        m_pHasher = new AFCHasher();
    }

    /**
     * @fn  virtual AFCConsistentHash::~AFCConsistentHash()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    virtual ~AFCConsistentHash()
    {
        delete m_pHasher;
        m_pHasher = NULL;
    }

public:

    /**
     * @fn  std::size_t AFCConsistentHash::Size() const
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
     * @fn  bool AFCConsistentHash::Empty() const
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
     * @fn  void AFCConsistentHash::Insert(const int nID, const std::string& strIP, int nPort)
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
        AFCMachineNode xNode;
        xNode.nMachineID = nID;
        xNode.strIP = strIP;
        xNode.nPort = nPort;

        Insert(xNode);
    }

    /**
     * @fn  void AFCConsistentHash::Insert(const AFCMachineNode& xNode)
     *
     * @brief   Inserts the given x coordinate node.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   xNode   The node.
     */

    void Insert(const AFCMachineNode& xNode)
    {
        uint32_t hash = m_pHasher->GetHashValue(xNode);
        auto it = mxNodes.find(hash);
        if(it == mxNodes.end())
        {
            mxNodes.insert(std::map<uint32_t, AFCMachineNode>::value_type(hash, xNode));
        }
    }

    /**
     * @fn  bool AFCConsistentHash::Exist(const AFCMachineNode& xInNode)
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

    bool Exist(const AFCMachineNode& xInNode)
    {
        uint32_t hash = m_pHasher->GetHashValue(xInNode);
        std::map<uint32_t, AFCMachineNode>::iterator it = mxNodes.find(hash);
        if(it != mxNodes.end())
        {
            return true;
        }

        return false;
    }

    /**
     * @fn  std::size_t AFCConsistentHash::Erase(const AFCMachineNode& xNode)
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

    std::size_t Erase(const AFCMachineNode& xNode)
    {
        uint32_t hash = m_pHasher->GetHashValue(xNode);
        return mxNodes.erase(hash);
    }

    /**
     * @fn  bool AFCConsistentHash::GetSuitNode(AFCMachineNode& node)
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

    bool GetSuitNode(AFCMachineNode& node)
    {
        int nID = 0;
        return GetSuitNode(nID, node);
    }

    /**
     * @fn  bool AFCConsistentHash::GetSuitNode(const std::string& str, AFCMachineNode& node)
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

    bool GetSuitNode(const std::string& str, AFCMachineNode& node)
    {
        uint32_t nCRC32 = NFrame::CRC32(str);
        return GetSuitNode(nCRC32, node);
    }

    /**
     * @fn  bool AFCConsistentHash::GetSuitNode(uint32_t hashValue, AFCMachineNode& node)
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

    bool GetSuitNode(uint32_t hashValue, AFCMachineNode& node)
    {
        if(mxNodes.empty())
        {
            return false;
        }

        std::map<uint32_t, AFCMachineNode>::iterator it = mxNodes.lower_bound(hashValue);

        if(it == mxNodes.end())
        {
            it = mxNodes.begin();
        }

        node = it->second;

        return true;
    }

    /**
     * @fn  bool AFCConsistentHash::GetNodeList(std::list<AFCMachineNode>& nodeList)
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

    bool GetNodeList(std::list<AFCMachineNode>& nodeList)
    {
        for(std::map<uint32_t, AFCMachineNode>::iterator it = mxNodes.begin(); it != mxNodes.end(); ++it)
        {
            nodeList.push_back(it->second);
        }

        return true;
    }

private:
    /** @brief   The mx nodes. */
    std::map<uint32_t, AFCMachineNode> mxNodes;
    /** @brief   The hasher. */
    AFIHasher* m_pHasher;
};