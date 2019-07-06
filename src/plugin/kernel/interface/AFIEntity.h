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

#include "kernel/include/AFCDataList.hpp"
#include "kernel/include/AFDataTable.hpp"
#include "kernel/interface/AFIDataTableManager.h"
#include "kernel/interface/AFIDataNodeManager.h"
#include "kernel/interface/AFIEventManager.h"

namespace ark
{

class AFIEntity
{
public:
    virtual ~AFIEntity() = default;

    virtual void Update() = 0;
    virtual const AFGUID &Self() = 0;

    virtual bool CheckNodeExist(const std::string &name) = 0;

    virtual bool SetNodeBool(const std::string &name, const bool value) = 0;
    virtual bool SetNodeInt(const std::string &name, const int32_t value) = 0;
    virtual bool SetNodeInt64(const std::string &name, const int64_t value) = 0;
    virtual bool SetNodeFloat(const std::string &name, const float value) = 0;
    virtual bool SetNodeDouble(const std::string &name, const double value) = 0;
    virtual bool SetNodeString(const std::string &name, const std::string &value) = 0;

    virtual bool GetNodeBool(const std::string &name) = 0;
    virtual int32_t GetNodeInt(const std::string &name) = 0;
    virtual int64_t GetNodeInt64(const std::string &name) = 0;
    virtual float GetNodeFloat(const std::string &name) = 0;
    virtual double GetNodeDouble(const std::string &name) = 0;
    virtual const char *GetNodeString(const std::string &name) = 0;

    virtual bool CheckTableExist(const std::string &name) = 0;

    virtual bool SetTableBool(const std::string &name, const int row, const int col, const bool value) = 0;
    virtual bool SetTableInt(const std::string &name, const int row, const int col, const int32_t value) = 0;
    virtual bool SetTableInt64(const std::string &name, const int row, const int col, const int64_t value) = 0;
    virtual bool SetTableFloat(const std::string &name, const int row, const int col, const float value) = 0;
    virtual bool SetTableDouble(const std::string &name, const int row, const int col, const double value) = 0;
    virtual bool SetTableString(const std::string &name, const int row, const int col, const std::string &value) = 0;

    virtual bool GetTableBool(const std::string &name, const int row, const int col) = 0;
    virtual int32_t GetTableInt(const std::string &name, const int row, const int col) = 0;
    virtual int64_t GetTableInt64(const std::string &name, const int row, const int col) = 0;
    virtual float GetTableFloat(const std::string &name, const int row, const int col) = 0;
    virtual double GetTableDouble(const std::string &name, const int row, const int col) = 0;
    virtual const char *GetTableString(const std::string &name, const int row, const int col) = 0;

    virtual ARK_SHARE_PTR<AFIDataNodeManager> &GetNodeManager() = 0;
    virtual ARK_SHARE_PTR<AFIDataTableManager> &GetTableManager() = 0;
    virtual ARK_SHARE_PTR<AFIEventManager> &GetEventManager() = 0;
};

} // namespace ark