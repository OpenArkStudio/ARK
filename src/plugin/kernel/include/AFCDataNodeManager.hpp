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


#include "base/AFNoncopyable.hpp"
#include "base/AFArrayPod.hpp"
#include "kernel/interface/AFIDataNodeManager.h"
#include "AFDataNode.hpp"

namespace ark
{

    class AFIClass;
    class AFCDataNodeManager : public AFIDataNodeManager, public AFNoncopyable
    {
    public:
        AFCDataNodeManager() = delete;

        explicit AFCDataNodeManager(const AFGUID& self) : self_(self)
        {
        }

        void Clear() final override
        {
            for (size_t i = 0; i < data_nodes_.size(); ++i)
            {
                ARK_DELETE(data_nodes_[i]);
            }

            data_nodes_.clear();
            node_indices_.Clear();
        }

        const AFGUID& Self() const override
        {
            return self_;
        }

        bool RegisterCallback(DATA_NODE_EVENT_FUNCTOR&& cb) override
        {
            node_callbacks_.push_back(std::forward<DATA_NODE_EVENT_FUNCTOR>(cb));
            return true;
        }

        size_t GetNodeCount() override
        {
            return data_nodes_.size();
        }

        AFDataNode* GetNodeByIndex(size_t index) override
        {
            ARK_ASSERT_RET_VAL(index >= 0 && index <= data_nodes_.size(), nullptr);
            return data_nodes_[index];
        }

        AFDataNode* GetNode(const char* name) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return nullptr;
            }

            return data_nodes_[index];
        }

        bool AddNode(const char* name, const AFIData& value, const AFFeatureType feature) override
        {
            AFDataNode* pNode = ARK_NEW AFDataNode;
            pNode->name = name;
            pNode->value = value;
            pNode->feature = feature;
            node_indices_.Add(name, data_nodes_.size());
            data_nodes_.push_back(pNode);
            return true;
        }

        bool SetNode(const char* name, const AFIData& value) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return false;
            }

            switch (value.GetType())
            {
            case DT_BOOLEAN:
                return SetNodeBool(name, value.GetBool());
                break;
            case DT_INT:
                return SetNodeInt(name, value.GetInt());
                break;
            case DT_INT64:
                return SetNodeInt64(name, value.GetInt64());
                break;
            case DT_FLOAT:
                return SetNodeFloat(name, value.GetFloat());
                break;
            case DT_DOUBLE:
                return SetNodeDouble(name, value.GetDouble());
                break;
            case DT_STRING:
                return SetNodeString(name, value.GetString());
                break;
            default:
                ARK_ASSERT_NO_EFFECT(0);
                break;
            }

            return false;
        }

        bool SetNodeBool(const char* name, const bool value) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return false;
            }

            //old value
            AFCData oldData;
            bool oldValue = data_nodes_[index]->value.GetBool();
            oldData.SetBool(oldValue);

            data_nodes_[index]->value.SetBool(value);

            if (oldValue != value)
            {
                //DataNode callbacks
                OnNodeCallback(name, oldData, data_nodes_[index]->value);
            }

            return true;
        }

        bool SetNodeInt(const char* name, const int32_t value) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return false;
            }

            //old value
            AFCData oldData;
            int32_t oldValue = data_nodes_[index]->value.GetInt();
            oldData.SetInt(oldValue);

            data_nodes_[index]->value.SetInt(value);

            if (oldValue != value)
            {
                //DataNode callbacks
                OnNodeCallback(name, oldData, data_nodes_[index]->value);
            }

            return true;
        }

        bool SetNodeInt64(const char* name, const int64_t value) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return false;
            }

            //old value
            AFCData oldData;
            int64_t oldValue = data_nodes_[index]->value.GetInt64();
            oldData.SetInt64(oldValue);

            data_nodes_[index]->value.SetInt64(value);

            if (oldValue != value)
            {
                //DataNode callbacks
                OnNodeCallback(name, oldData, data_nodes_[index]->value);
            }

            return true;
        }

        bool SetNodeFloat(const char* name, const float value) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return false;
            }

            //old value
            AFCData oldData;
            float oldValue = data_nodes_[index]->value.GetFloat();
            oldData.SetFloat(oldValue);

            data_nodes_[index]->value.SetFloat(value);

            if (!AFMisc::IsFloatEqual(oldValue, value))
            {
                //DataNode callbacks
                OnNodeCallback(name, oldData, data_nodes_[index]->value);
            }

            return true;
        }

        bool SetNodeDouble(const char* name, const double value) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return false;
            }

            //old value
            AFCData oldData;
            double oldValue = data_nodes_[index]->value.GetDouble();
            oldData.SetDouble(oldValue);

            data_nodes_[index]->value.SetDouble(value);

            if (!AFMisc::IsDoubleEqual(oldValue, value))
            {
                //DataNode callbacks
                OnNodeCallback(name, oldData, data_nodes_[index]->value);
            }

            return true;
        }

        bool SetNodeString(const char* name, const std::string& value) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return false;
            }

            //old value
            AFCData oldData;
            std::string oldValue = data_nodes_[index]->value.GetString();
            oldData.SetString(oldValue.c_str());

            data_nodes_[index]->value.SetString(value.c_str());

            if (ARK_STRICMP(oldValue.c_str(), value.c_str()) == 0)
            {
                //DataNode callbacks
                OnNodeCallback(name, oldData, data_nodes_[index]->value);
            }

            return true;
        }

        bool GetNodeBool(const char* name) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return NULL_BOOLEAN;
            }

            return data_nodes_[index]->value.GetBool();
        }

        int32_t GetNodeInt(const char* name) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return NULL_INT;
            }

            return data_nodes_[index]->value.GetInt();
        }

        int64_t GetNodeInt64(const char* name) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return NULL_INT64;
            }

            return data_nodes_[index]->value.GetInt64();
        }

        float GetNodeFloat(const char* name) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return NULL_FLOAT;
            }

            return data_nodes_[index]->value.GetFloat();
        }

        double GetNodeDouble(const char* name) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return NULL_DOUBLE;
            }

            return data_nodes_[index]->value.GetDouble();
        }

        const char* GetNodeString(const char* name) override
        {
            size_t index;

            if (!FindIndex(name, index))
            {
                return NULL_STR.c_str();
            }

            return data_nodes_[index]->value.GetString();
        }

    protected:
        bool FindIndex(const char* name, size_t& index)
        {
            if (!node_indices_.GetData(name, index))
            {
                return false;
            }

            return true;
        }

        bool OnNodeCallback(const char* name, const AFIData& oldData, const AFIData& newData)
        {
            for (auto& cb : node_callbacks_)
            {
                cb(self_, name, oldData, newData);
            }

            return true;
        }

    private:
        AFArrayPod<AFDataNode*, 1, CoreAlloc> data_nodes_;
        AFStringPod<char, size_t, AFStringTraits<char>, CoreAlloc> node_indices_;

        AFGUID self_;
        std::vector<DATA_NODE_EVENT_FUNCTOR> node_callbacks_;
    };

}