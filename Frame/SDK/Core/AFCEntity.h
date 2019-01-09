/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include "SDK/Core/AFIEntity.h"
#include "SDK/Core/AFIDataTableManager.h"
#include "SDK/Core/AFIDataNodeManager.h"

namespace ark
{

    class AFCEntity : public AFIEntity
    {
    public:
        AFCEntity() = delete;

        explicit AFCEntity(const AFGUID& self);

        void Update() override;
        ///////////////////////////////////////////////////////////////////////
        const AFGUID& Self() override;

        bool CheckNodeExist(const std::string& name) override;

        bool SetNodeBool(const std::string& name, const bool value) override;
        bool SetNodeInt(const std::string& name, const int32_t value) override;
        bool SetNodeInt64(const std::string& name, const int64_t value) override;
        bool SetNodeFloat(const std::string& name, const float value) override;
        bool SetNodeDouble(const std::string& name, const double value) override;
        bool SetNodeString(const std::string& name, const std::string& value) override;

        bool GetNodeBool(const std::string& name) override;
        int32_t GetNodeInt(const std::string& name) override;
        int64_t GetNodeInt64(const std::string& name) override;
        float GetNodeFloat(const std::string& name) override;
        double GetNodeDouble(const std::string& name) override;
        const char*  GetNodeString(const std::string& name) override;

        bool CheckTableExist(const std::string& name) override;

        bool SetTableBool(const std::string& name, const int row, const int col, const bool value) override;
        bool SetTableInt(const std::string& name, const int row, const int col, const int32_t value) override;
        bool SetTableInt64(const std::string& name, const int row, const int col, const int64_t value) override;
        bool SetTableFloat(const std::string& name, const int row, const int col, const float value) override;
        bool SetTableDouble(const std::string& name, const int row, const int col, const double value) override;
        bool SetTableString(const std::string& name, const int row, const int col, const std::string& value) override;

        bool GetTableBool(const std::string& name, const int row, const int col) override;
        int32_t GetTableInt(const std::string& name, const int row, const int col) override;
        int64_t GetTableInt64(const std::string& name, const int row, const int col) override;
        float GetTableFloat(const std::string& name, const int row, const int col) override;
        double GetTableDouble(const std::string& name, const int row, const int col) override;
        const char* GetTableString(const std::string& name, const int row, const int col) override;

        //////////////////////////////////////////////////////////////////////////
        ARK_SHARE_PTR<AFIDataNodeManager>& GetNodeManager() override;
        ARK_SHARE_PTR<AFIDataTableManager>& GetTableManager() override;
        ARK_SHARE_PTR<AFIEventManager>& GetEventManager() override;

    private:
        AFGUID mSelf;

        ARK_SHARE_PTR<AFIDataNodeManager> m_pNodeManager;
        ARK_SHARE_PTR<AFIDataTableManager> m_pTableManager;
        ARK_SHARE_PTR<AFIEventManager> m_pEventManager;
    };

}