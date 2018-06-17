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

#include "AFCDataNodeManager.h"
#include "AFDataNode.h"
#include "SDK/Core/Base/AFMisc.hpp"

AFCDataNodeManager::AFCDataNodeManager(const AFGUID& self) : mxSelf(self)
{

}

AFCDataNodeManager::~AFCDataNodeManager()
{
    Clear();
}

void AFCDataNodeManager::Clear()
{
    for(size_t i = 0; i < mxNodes.size(); ++i)
    {
        delete mxNodes[i];
    }

    for(size_t i = 0; i < mxNodeCBs.size(); ++i)
    {
        delete mxNodeCBs[i];
    }

    mxNodes.clear();
    mxIndices.Clear();
    mxNodeCBs.clear();
    mxCallBackIndices.Clear();
}

const AFGUID& AFCDataNodeManager::Self() const
{
    return mxSelf;
}

bool AFCDataNodeManager::RegisterCallback(const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR& cb)
{
    size_t index(0);
    if(!FindIndex(name.c_str(), index))
    {
        return false;
    }

    size_t indexCallback(0);
    if(mxCallBackIndices.GetData(name.c_str(), indexCallback))
    {
        mxNodeCBs[indexCallback]->mxCallBackList.push_back(cb);
    }
    else
    {
        AFNodeCallBack* pNodeCB = new AFNodeCallBack();
        pNodeCB->mxCallBackList.push_back(cb);
        mxCallBackIndices.Add(name.c_str(), mxNodeCBs.size());
        mxNodeCBs.push_back(pNodeCB);
    }
    return true;
}

bool AFCDataNodeManager::RegisterCommonCallback(const DATA_NODE_EVENT_FUNCTOR_PTR& cb)
{
    mxCommonCallBackList.push_back(cb);
    return true;
}

size_t AFCDataNodeManager::GetNodeCount()
{
    return mxNodes.size();
}

AFDataNode* AFCDataNodeManager::GetNodeByIndex(size_t index)
{
    ARK_ASSERT_RET_VAL(index >= 0 && index <= mxNodes.size(), nullptr);

    return mxNodes[index];
}

AFDataNode* AFCDataNodeManager::GetNode(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return nullptr;
    }

    return mxNodes[index];
}

bool AFCDataNodeManager::FindIndex(const char* name, size_t& index)
{
    if(!mxIndices.GetData(name, index))
    {
        return false;
    }

    return true;
}

bool AFCDataNodeManager::OnNodeCallback(const char* name, const AFIData& oldData, const AFIData& newData)
{
    size_t indexCallBack = 0;
    if(!mxCallBackIndices.GetData(name, indexCallBack))
    {
        return false;
    }

    for(size_t i = 0; i < mxCommonCallBackList.size(); ++i)
    {
        (*mxCommonCallBackList[i])(mxSelf, name, oldData, newData);
    }

    for(size_t i = 0; i < mxNodeCBs[indexCallBack]->mxCallBackList.size(); ++i)
    {
        (*(mxNodeCBs[indexCallBack]->mxCallBackList[i]))(mxSelf, name, oldData, newData);
    }

    return true;
}

bool AFCDataNodeManager::AddNode(const char* name, const AFIData& value, const int8_t feature)
{
    AFDataNode* pNode = new AFDataNode();
    pNode->name = name;
    pNode->value = value;
    pNode->feature = feature;
    mxIndices.Add(name, mxNodes.size());
    mxNodes.push_back(pNode);
    return true;
}

bool AFCDataNodeManager::SetNode(const char* name, const AFIData& value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    switch(value.GetType())
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
    case DT_OBJECT:
        return SetNodeObject(name, value.GetObject());
        break;
    default:
        ARK_ASSERT_NO_EFFECT(0);
        break;
    }

    return false;
}

bool AFCDataNodeManager::SetNodeBool(const char* name, const bool value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    bool oldValue = mxNodes[index]->value.GetBool();
    oldData.SetBool(oldValue);

    mxNodes[index]->value.SetBool(value);

    if(oldValue != value)
    {
        //DataNode callbacks
        OnNodeCallback(name, oldData, mxNodes[index]->value);
    }

    return true;
}

bool AFCDataNodeManager::SetNodeInt(const char* name, const int32_t value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    int32_t oldValue = mxNodes[index]->value.GetInt();
    oldData.SetInt(oldValue);

    mxNodes[index]->value.SetInt(value);

    if(oldValue != value)
    {
        //DataNode callbacks
        OnNodeCallback(name, oldData, mxNodes[index]->value);
    }

    return true;
}

bool AFCDataNodeManager::SetNodeInt64(const char* name, const int64_t value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    int64_t oldValue = mxNodes[index]->value.GetInt64();
    oldData.SetInt64(oldValue);

    mxNodes[index]->value.SetInt64(value);

    if(oldValue != value)
    {
        //DataNode callbacks
        OnNodeCallback(name, oldData, mxNodes[index]->value);
    }

    return true;
}

bool AFCDataNodeManager::SetNodeFloat(const char* name, const float value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    float oldValue = mxNodes[index]->value.GetFloat();
    oldData.SetFloat(oldValue);

    mxNodes[index]->value.SetFloat(value);

    if(!AFMisc::IsFloatEqual(oldValue, value))
    {
        //DataNode callbacks
        OnNodeCallback(name, oldData, mxNodes[index]->value);
    }

    return true;
}

bool AFCDataNodeManager::SetNodeDouble(const char* name, const double value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    double oldValue = mxNodes[index]->value.GetDouble();
    oldData.SetDouble(oldValue);

    mxNodes[index]->value.SetDouble(value);

    if(!AFMisc::IsDoubleEqual(oldValue, value))
    {
        //DataNode callbacks
        OnNodeCallback(name, oldData, mxNodes[index]->value);
    }

    return true;
}

bool AFCDataNodeManager::SetNodeString(const char* name, const std::string& value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    std::string oldValue = mxNodes[index]->value.GetString();
    oldData.SetString(oldValue.c_str());

    mxNodes[index]->value.SetString(value.c_str());

    if(ARK_STRICMP(oldValue.c_str(), value.c_str()) == 0)
    {
        //DataNode callbacks
        OnNodeCallback(name, oldData, mxNodes[index]->value);
    }

    return true;
}

bool AFCDataNodeManager::SetNodeObject(const char* name, const AFGUID& value)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return false;
    }

    //old value
    AFCData oldData;
    AFGUID oldValue = mxNodes[index]->value.GetObject();
    oldData.SetObject(oldValue);

    mxNodes[index]->value.SetObject(value);

    if(oldValue != value)
    {
        //DataNode callbacks
        OnNodeCallback(name, oldData, mxNodes[index]->value);
    }

    return true;
}

bool AFCDataNodeManager::GetNodeBool(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_BOOLEAN;
    }

    return mxNodes[index]->value.GetBool();
}

int32_t AFCDataNodeManager::GetNodeInt(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_INT;
    }

    return mxNodes[index]->value.GetInt();
}

int64_t AFCDataNodeManager::GetNodeInt64(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_INT64;
    }

    return mxNodes[index]->value.GetInt64();
}

float AFCDataNodeManager::GetNodeFloat(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_FLOAT;
    }

    return mxNodes[index]->value.GetFloat();
}

double AFCDataNodeManager::GetNodeDouble(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_DOUBLE;
    }

    return mxNodes[index]->value.GetDouble();
}

const char* AFCDataNodeManager::GetNodeString(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_STR.c_str();
    }

    return mxNodes[index]->value.GetString();
}

const AFGUID AFCDataNodeManager::GetNodeObject(const char* name)
{
    size_t index;
    if(!FindIndex(name, index))
    {
        return NULL_GUID;
    }

    return mxNodes[index]->value.GetObject();
}