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

#include "SDK/Core/AFNoncopyable.hpp"
#include "AFIDataNodeManager.h"

namespace ark
{

    class AFIClass;
    class AFCDataNodeManager : public AFIDataNodeManager, public AFNoncopyable
    {
    public:
        AFCDataNodeManager() = delete;

        explicit AFCDataNodeManager(const AFGUID& self);
        ~AFCDataNodeManager() override;
        void Clear() final override;

        const AFGUID& Self() const override;

        bool RegisterCallback(const DATA_NODE_EVENT_FUNCTOR_PTR& cb) override;

        size_t GetNodeCount() override;
        AFDataNode* GetNodeByIndex(size_t index) override;
        AFDataNode* GetNode(const char* name) override;
        bool AddNode(const char* name, const AFIData& value, const AFFeatureType feature) override;
        bool SetNode(const char* name, const AFIData& value) override;

        bool SetNodeBool(const char* name, const bool value) override;
        bool SetNodeInt(const char* name, const int32_t value) override;
        bool SetNodeInt64(const char* name, const int64_t value) override;
        bool SetNodeFloat(const char* name, const float value) override;
        bool SetNodeDouble(const char* name, const double value) override;
        bool SetNodeString(const char* name, const std::string& value) override;
        bool SetNodeObject(const char* name, const AFGUID& value) override;

        bool GetNodeBool(const char* name) override;
        int32_t GetNodeInt(const char* name) override;
        int64_t GetNodeInt64(const char* name) override;
        float GetNodeFloat(const char* name) override;
        double GetNodeDouble(const char* name) override;
        const char* GetNodeString(const char* name) override;
        const AFGUID GetNodeObject(const char* name) override;

    protected:
        bool FindIndex(const char* name, size_t& index);
        bool OnNodeCallback(const char* name, const AFIData& oldData, const AFIData& newData);

    private:
        ArrayPod<AFDataNode*, 1, CoreAlloc> mxNodes;
        StringPod<char, size_t, StringTraits<char>, CoreAlloc> mxIndices;

        AFGUID mxSelf;
        std::vector<DATA_NODE_EVENT_FUNCTOR_PTR> mxCallBackList;
    };

}