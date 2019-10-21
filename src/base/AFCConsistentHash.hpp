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

#include "base/AFCRC.hpp"
#include "base/AFMurmurHash.hpp"

namespace ark {

//class AFIVirtualNode
//{
//public:
//    explicit AFIVirtualNode(const int nVirID)
//        : nVirtualIndex(nVirID)
//    {
//    }
//
//    virtual std::string GetDataStr() const
//    {
//        return "";
//    }
//
//    virtual int GetDataID() const
//    {
//        return 0;
//    }
//
//    virtual bool Candidate() const
//    {
//        return false;
//    };
//
//    std::string ToStr() const
//    {
//        std::ostringstream strInfo;
//        strInfo << ARK_LEXICAL_CAST<std::string>(GetDataID()) << "-" << GetDataStr() << "-" << nVirtualIndex;
//        return strInfo.str();
//    }
//
//private:
//    int nVirtualIndex;
//};
//
//class AFCMachineNode : public AFIVirtualNode
//{
//public:
//    AFCMachineNode() = default;
//
//    explicit AFCMachineNode(const int nVirID)
//        : AFIVirtualNode(nVirID)
//    {
//        strIP = "";
//        nPort = 0;
//        nWeight = 0;
//        nMachineID = 0;
//        bCandidate = false;
//    }
//
//    virtual std::string GetDataStr() const
//    {
//        return strIP;
//    }
//
//    virtual int GetDataID() const
//    {
//        return nMachineID;
//    }
//
//    virtual bool Candidate() const
//    {
//        return bCandidate;
//    }
//
//    std::string strIP;
//    int nPort;
//    int nWeight;
//    int nMachineID;
//    bool bCandidate;
//
//    std::list<AFIVirtualNode> xRealMachine;
//};
//
//class AFIHasher
//{
//public:
//    virtual ~AFIHasher() = default;
//    virtual uint32_t GetHashValue(const AFIVirtualNode& vNode) = 0;
//};
//
//class AFCHasher : public AFIHasher
//{
//public:
//    virtual ~AFCHasher() = default;
//
//    virtual uint32_t GetHashValue(const AFIVirtualNode& vNode)
//    {
//        std::string vnode = vNode.ToStr();
//        return AFCRC32::Sum(vnode);
//    }
//};
//
//class AFIConsistentHash
//{
//public:
//    virtual ~AFIConsistentHash() = default;
//
//    virtual std::size_t Size() const = 0;
//
//    virtual bool Empty() const = 0;
//
//    virtual void Insert(const int nID, const std::string& strIP, int nPort) = 0;
//
//    virtual void Insert(const AFCMachineNode& xNode) = 0;
//
//    virtual bool Exist(const AFCMachineNode& xInNode) = 0;
//
//    virtual std::size_t Erase(const AFCMachineNode& xNode) = 0;
//
//    virtual bool GetSuitNode(AFCMachineNode& node) = 0;
//
//    virtual bool GetSuitNode(const std::string& str, AFCMachineNode& node) = 0;
//
//    virtual bool GetSuitNode(uint32_t hashValue, AFCMachineNode& node) = 0;
//
//    virtual bool GetNodeList(std::list<AFCMachineNode>& nodeList) = 0;
//};
//
//class AFCConsistentHash : public AFIConsistentHash
//{
//public:
//    AFCConsistentHash()
//    {
//        m_pHasher = new AFCHasher();
//    }
//
//    virtual ~AFCConsistentHash()
//    {
//        delete m_pHasher;
//        m_pHasher = NULL;
//    }
//
//    std::size_t Size() const override
//    {
//        return mxNodes.size();
//    }
//
//    bool Empty() const override
//    {
//        return mxNodes.empty();
//    }
//
//    void Insert(const int nID, const std::string& strIP, int nPort) override
//    {
//        AFCMachineNode xNode;
//        xNode.nMachineID = nID;
//        xNode.strIP = strIP;
//        xNode.nPort = nPort;
//
//        Insert(xNode);
//    }
//
//    void Insert(const AFCMachineNode& xNode) override
//    {
//        uint32_t hash = m_pHasher->GetHashValue(xNode);
//        auto it = mxNodes.find(hash);
//
//        if (it == mxNodes.end())
//        {
//            mxNodes.insert(std::map<uint32_t, AFCMachineNode>::value_type(hash, xNode));
//        }
//    }
//
//    bool Exist(const AFCMachineNode& xInNode) override
//    {
//        uint32_t hash = m_pHasher->GetHashValue(xInNode);
//        std::map<uint32_t, AFCMachineNode>::iterator it = mxNodes.find(hash);
//
//        if (it != mxNodes.end())
//        {
//            return true;
//        }
//
//        return false;
//    }
//
//    std::size_t Erase(const AFCMachineNode& xNode) override
//    {
//        uint32_t hash = m_pHasher->GetHashValue(xNode);
//        return mxNodes.erase(hash);
//    }
//
//    bool GetSuitNode(AFCMachineNode& node) override
//    {
//        int nID = 0;
//        return GetSuitNode(nID, node);
//    }
//
//    bool GetSuitNode(const std::string& str, AFCMachineNode& node) override
//    {
//        uint32_t nCRC32 = AFCRC32::Sum(str);
//        return GetSuitNode(nCRC32, node);
//    }
//
//    bool GetSuitNode(uint32_t hashValue, AFCMachineNode& node) override
//    {
//        if (mxNodes.empty())
//        {
//            return false;
//        }
//
//        std::map<uint32_t, AFCMachineNode>::iterator it = mxNodes.lower_bound(hashValue);
//
//        if (it == mxNodes.end())
//        {
//            it = mxNodes.begin();
//        }
//
//        node = it->second;
//
//        return true;
//    }
//
//    bool GetNodeList(std::list<AFCMachineNode>& nodeList) override
//    {
//        for (auto it : mxNodes)
//        {
//            nodeList.push_back(it.second);
//        }
//
//        return true;
//    }
//
//private:
//    std::map<uint32_t, AFCMachineNode> mxNodes;
//    AFIHasher* m_pHasher;
//};

class AFVNode
{
public:
    AFVNode(int server_id, const std::string& ip, std::size_t port, std::size_t id)
        : bus_id(server_id)
        , vnode_ip(ip)
        , vnode_port(port)
        , vnode_id(id)
    {
    }

    AFVNode(int server_id, const char* ip, std::size_t port, std::size_t id)
        : bus_id(server_id)
        , vnode_ip(ip)
        , vnode_port(port)
        , vnode_id(id)
    {
    }

    std::string to_str() const
    {
        return ARK_FORMAT("{}-{}-{}-{}", bus_id, vnode_ip, vnode_port, vnode_id);
    }

    int bus_id{0};             // server bus id
    std::string vnode_ip;      // server ip
    std::size_t vnode_port{0}; // server port
    std::size_t vnode_id;      // node id
};

template<typename T, typename Hash, typename Alloc = std::allocator<std::pair<const typename Hash::result_type, T>>>
class AFConsistentHashmap
{
public:
    using size_type = typename Hash::result_type;
    using map_type = std::map<size_type, T, std::less<size_type>, Alloc>;
    using value_type = typename map_type::value_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = typename map_type::iterator;
    using reverse_iterator = typename map_type::reverse_iterator;
    using allocator_type = Alloc;

    AFConsistentHashmap() = default;
    ~AFConsistentHashmap() = default;

    void set_vnode_count(const std::size_t count)
    {
        vnode_count_ = count;
    }

    void insert(const int bus_id, const std::string& ip, const std::size_t port)
    {
        AFVNode vnode(bus_id, ip, port, 0);
        for (std::size_t i = 0; i < vnode_count_; ++i)
        {
            vnode.vnode_id = i;
            insert(vnode);
        }
    }

    std::size_t size() const
    {
        return nodes_.size();
    }

    bool empty() const
    {
        return nodes_.empty();
    }

    iterator find(const std::string& key)
    {
        return find(hasher_.hash(key));
    }

    std::size_t erase(const T& node)
    {
        size_type hash = hasher_(node);
        return nodes_.erase(hash);
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

protected:
    std::pair<iterator, bool> insert(const T& node)
    {
        size_type hash = hasher_(node);
        return nodes_.insert(value_type(hash, node));
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

    void erase(iterator it)
    {
        nodes_.erase(it);
    }

private:
    Hash hasher_;
    map_type nodes_;
    std::size_t vnode_count_{100}; // default virtual node count is 100
};

class AFCRCHasher
{
public:
    using result_type = uint32_t;

    static result_type hash(const std::string& key)
    {
        result_type ret = 0;
        return AFCRC::crc32(key);
    }

    result_type operator()(const AFVNode& node)
    {
        return hash(node.to_str());
    }
};

class AFMurmurHasher
{
public:
    using result_type = uint32_t;

    static result_type hash(const std::string& key)
    {
        result_type ret = 0;
        AFMurmurHash::MurmurHash2(key.c_str(), key.size(), &ret);
        return ret;
    }

    result_type operator()(const AFVNode& node)
    {
        return hash(node.to_str());
    }
};

using AFConsistentHashmapType = AFConsistentHashmap<AFVNode, AFMurmurHasher>;

} // namespace ark
