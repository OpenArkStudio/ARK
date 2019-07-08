/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "AFCRC.hpp"
#include "base/AFMacros.hpp"

namespace ark {

class AFIVirtualNode
{
public:
    AFIVirtualNode() = default;

    explicit AFIVirtualNode(const int nVirID)
        : nVirtualIndex(nVirID)
    {
    }

    virtual ~AFIVirtualNode()
    {
        nVirtualIndex = 0;
    }

    virtual std::string GetDataStr() const
    {
        return "";
    }

    virtual int GetDataID() const
    {
        return 0;
    }

    virtual bool Candidate() const
    {
        return false;
    };

    std::string ToStr() const
    {
        std::ostringstream strInfo;
        strInfo << ARK_LEXICAL_CAST<std::string>(GetDataID()) << "-" << GetDataStr() << "-" << nVirtualIndex;
        return strInfo.str();
    }

private:
    int nVirtualIndex;
};

class AFCMachineNode : public AFIVirtualNode
{
public:
    AFCMachineNode()
    {
        strIP = "";
        nPort = 0;
        nWeight = 0;
        nMachineID = 0;
        bCandidate = false;
    }

    explicit AFCMachineNode(const int nVirID)
        : AFIVirtualNode(nVirID)
    {
        strIP = "";
        nPort = 0;
        nWeight = 0;
        nMachineID = 0;
        bCandidate = false;
    }

    virtual std::string GetDataStr() const
    {
        return strIP;
    }

    virtual int GetDataID() const
    {
        return nMachineID;
    }

    virtual bool Candidate() const
    {
        return bCandidate;
    }

    std::string strIP;
    int nPort;
    int nWeight;
    int nMachineID;
    bool bCandidate;

    std::list<AFIVirtualNode> xRealMachine;
};

class AFIHasher
{
public:
    virtual ~AFIHasher() = default;
    virtual uint32_t GetHashValue(const AFIVirtualNode &vNode) = 0;
};

class AFCHasher : public AFIHasher
{
public:
    virtual ~AFCHasher() = default;

    virtual uint32_t GetHashValue(const AFIVirtualNode &vNode)
    {
        std::string vnode = vNode.ToStr();
        return AFCRC32::Sum(vnode);
    }
};

class AFIConsistentHash
{
public:
    virtual ~AFIConsistentHash() = default;

    virtual std::size_t Size() const = 0;

    virtual bool Empty() const = 0;

    virtual void Insert(const int nID, const std::string &strIP, int nPort) = 0;

    virtual void Insert(const AFCMachineNode &xNode) = 0;

    virtual bool Exist(const AFCMachineNode &xInNode) = 0;

    virtual std::size_t Erase(const AFCMachineNode &xNode) = 0;

    virtual bool GetSuitNode(AFCMachineNode &node) = 0;

    virtual bool GetSuitNode(const std::string &str, AFCMachineNode &node) = 0;

    virtual bool GetSuitNode(uint32_t hashValue, AFCMachineNode &node) = 0;

    virtual bool GetNodeList(std::list<AFCMachineNode> &nodeList) = 0;
};

class AFCConsistentHash : public AFIConsistentHash
{
public:
    AFCConsistentHash()
    {
        m_pHasher = new AFCHasher();
    }

    virtual ~AFCConsistentHash()
    {
        delete m_pHasher;
        m_pHasher = NULL;
    }

    std::size_t Size() const override
    {
        return mxNodes.size();
    }

    bool Empty() const override
    {
        return mxNodes.empty();
    }

    void Insert(const int nID, const std::string &strIP, int nPort) override
    {
        AFCMachineNode xNode;
        xNode.nMachineID = nID;
        xNode.strIP = strIP;
        xNode.nPort = nPort;

        Insert(xNode);
    }

    void Insert(const AFCMachineNode &xNode) override
    {
        uint32_t hash = m_pHasher->GetHashValue(xNode);
        auto it = mxNodes.find(hash);

        if (it == mxNodes.end())
        {
            mxNodes.insert(std::map<uint32_t, AFCMachineNode>::value_type(hash, xNode));
        }
    }

    bool Exist(const AFCMachineNode &xInNode) override
    {
        uint32_t hash = m_pHasher->GetHashValue(xInNode);
        std::map<uint32_t, AFCMachineNode>::iterator it = mxNodes.find(hash);

        if (it != mxNodes.end())
        {
            return true;
        }

        return false;
    }

    std::size_t Erase(const AFCMachineNode &xNode) override
    {
        uint32_t hash = m_pHasher->GetHashValue(xNode);
        return mxNodes.erase(hash);
    }

    bool GetSuitNode(AFCMachineNode &node) override
    {
        int nID = 0;
        return GetSuitNode(nID, node);
    }

    bool GetSuitNode(const std::string &str, AFCMachineNode &node) override
    {
        uint32_t nCRC32 = AFCRC32::Sum(str);
        return GetSuitNode(nCRC32, node);
    }

    bool GetSuitNode(uint32_t hashValue, AFCMachineNode &node) override
    {
        if (mxNodes.empty())
        {
            return false;
        }

        std::map<uint32_t, AFCMachineNode>::iterator it = mxNodes.lower_bound(hashValue);

        if (it == mxNodes.end())
        {
            it = mxNodes.begin();
        }

        node = it->second;

        return true;
    }

    bool GetNodeList(std::list<AFCMachineNode> &nodeList) override
    {
        for (auto it : mxNodes)
        {
            nodeList.push_back(it.second);
        }

        return true;
    }

private:
    std::map<uint32_t, AFCMachineNode> mxNodes;
    AFIHasher *m_pHasher;
};

template<typename T, typename Hash, typename Alloc = std::allocator<std::pair<const typename Hash::result_type, T>>>
class AFConsistentHashmap
{
public:
    using size_type = typename Hash::result_type;
    using map_type = std::map<size_type, T, std::less<size_type>, Alloc>;
    using value_type = typename map_type::value_type;
    using reference = value_type &;
    using const_reference = const value_type &;
    using iterator = typename map_type::iterator;
    using reverse_iterator = typename map_type::reverse_iterator;
    using allocator_type = Alloc;

    AFConsistentHashmap() = default;
    ~AFConsistentHashmap() = default;

    std::size_t size() const
    {
        return nodes_.size();
    }

    bool empty() const
    {
        return nodes_.empty();
    }

    std::pair<iterator, bool> insert(const T &node)
    {
        size_type hash = hasher_(node);
        return nodes_.insert(value_type(hash, node));
    }

    void erase(iterator it)
    {
        nodes_.erase(it);
    }

    std::size_t erase(const T &node)
    {
        size_type hash = hasher_(node);
        return nodes_.erase(hash);
    }

    iterator find(size_type hash)
    {
        if (nodes_.empty())
        {
            return nodes_.end();
        }

        iterator it = nodes_.lower_bound(hash);

        if (it == nodes_.end())
        {
            it = nodes_.begin();
        }

        return it;
    }

    iterator begin()
    {
        return nodes_.begin();
    }

    iterator end()
    {
        return nodes_.end();
    }

    reverse_iterator rbegin()
    {
        return nodes_.rbegin();
    }

    reverse_iterator rend()
    {
        return nodes_.rend();
    }

private:
    Hash hasher_;
    map_type nodes_;
};

class AFCRCHasher
{
public:
    using result_type = uint32_t;

    result_type operator()(const std::string &node)
    {
        return AFCRC32::Sum(node);
    }
};

using AFConsistentHashmapType = AFConsistentHashmap<std::string, AFCRCHasher>;
} // namespace ark