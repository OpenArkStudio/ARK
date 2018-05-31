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

#include "AFIDataNodeManager.h"
#include "SDK/Core/Base/AFNoncopyable.hpp"

class AFCDataNodeManager : public AFIDataNodeManager, public AFNoncopyable
{
public:
    AFCDataNodeManager() = delete;

    explicit AFCDataNodeManager(const AFGUID& self);
    virtual ~AFCDataNodeManager();
    virtual void Clear() final;

    virtual const AFGUID& Self() const;

    virtual bool RegisterCallback(const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR& cb);

    virtual size_t GetNodeCount();
    virtual AFDataNode* GetNodeByIndex(size_t index);
    virtual AFDataNode* GetNode(const char* name);
    virtual bool AddNode(const char* name, const AFIData& value, const int8_t feature);
    virtual bool SetNode(const char* name, const AFIData& value);

    virtual bool SetNodeBool(const char* name, const bool value);
    virtual bool SetNodeInt(const char* name, const int32_t value);
    virtual bool SetNodeInt64(const char* name, const int64_t value);
    virtual bool SetNodeFloat(const char* name, const float value);
    virtual bool SetNodeDouble(const char* name, const double value);
    virtual bool SetNodeString(const char* name, const std::string& value);
    virtual bool SetNodeObject(const char* name, const AFGUID& value);

    virtual bool GetNodeBool(const char* name);
    virtual int32_t GetNodeInt(const char* name);
    virtual int64_t GetNodeInt64(const char* name);
    virtual float GetNodeFloat(const char* name);
    virtual double GetNodeDouble(const char* name);
    virtual const char* GetNodeString(const char* name);
    virtual const AFGUID GetNodeObject(const char* name);

protected:
    bool FindIndex(const char* name, size_t& index);

    bool OnNodeCallback(const char* name, const AFIData& oldData, const AFIData& newData);

private:
    struct  AFNodeCallBack
    {
        std::vector<DATA_NODE_EVENT_FUNCTOR_PTR> mxCallBackList;
    };

private:
	ArrayPod<AFDataNode*, 1, CoreAlloc> mxNodes;
    StringPod<char, size_t, StringTraits<char>, CoreAlloc> mxIndices;

	ArrayPod<AFNodeCallBack*, 1, CoreAlloc> mxNodeCBs;
    StringPod<char, size_t, StringTraits<char>, CoreAlloc> mxCallBackIndices;

    AFGUID mxSelf;
};