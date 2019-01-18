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

#include "interface/AFIEntity.h"
#include "base/AFCDataTableManager.hpp"
#include "base/AFCDataNodeManager.hpp"
#include "base/AFCEventManager.hpp"

namespace ark
{

    class AFCEntity : public AFIEntity
    {
    public:
        AFCEntity() = delete;

        explicit AFCEntity(const AFGUID& self) :
            AFIEntity(self),
            mSelf(self)
        {
            m_pNodeManager = std::make_shared<AFCDataNodeManager>(mSelf);
            m_pTableManager = std::make_shared<AFCDataTableManager>(mSelf);
            m_pEventManager = std::make_shared<AFCEventManager>(mSelf);
        }

        void Update() override
        {
            GetEventManager()->Update();
        }

        ///////////////////////////////////////////////////////////////////////
        const AFGUID& Self() override
        {
            return mSelf;
        }

        bool CheckNodeExist(const std::string& name) override
        {
            return (GetNodeManager()->GetNode(name.c_str()) != nullptr);
        }

        bool SetNodeBool(const std::string& name, const bool value)
        {
            return GetNodeManager()->SetNodeBool(name.c_str(), value);
        }

        bool SetNodeInt(const std::string& name, const int32_t value)
        {
            return GetNodeManager()->SetNodeInt(name.c_str(), value);
        }

        bool SetNodeInt64(const std::string& name, const int64_t value)
        {
            return GetNodeManager()->SetNodeInt64(name.c_str(), value);
        }

        bool SetNodeFloat(const std::string& name, const float value)
        {
            return GetNodeManager()->SetNodeFloat(name.c_str(), value);
        }

        bool SetNodeDouble(const std::string& name, const double value)
        {
            return GetNodeManager()->SetNodeDouble(name.c_str(), value);
        }

        bool SetNodeString(const std::string& name, const std::string& value)
        {
            return GetNodeManager()->SetNodeString(name.c_str(), value);
        }

        bool GetNodeBool(const std::string& name)
        {
            return GetNodeManager()->GetNodeBool(name.c_str());
        }

        int32_t GetNodeInt(const std::string& name)
        {
            return GetNodeManager()->GetNodeInt(name.c_str());
        }

        int64_t GetNodeInt64(const std::string& name)
        {
            return GetNodeManager()->GetNodeInt64(name.c_str());
        }

        float GetNodeFloat(const std::string& name)
        {
            return GetNodeManager()->GetNodeFloat(name.c_str());
        }

        double GetNodeDouble(const std::string& name)
        {
            return GetNodeManager()->GetNodeDouble(name.c_str());
        }

        const char* GetNodeString(const std::string& name)
        {
            return GetNodeManager()->GetNodeString(name.c_str());
        }

        //////////////////////////////////////////////////////////////////////////

        bool CheckTableExist(const std::string& name) override
        {
            AFDataTable* pTable = GetTableManager()->GetTable(name.c_str());
            return (nullptr != pTable);
        }

        bool SetTableBool(const std::string& name, const int row, const int col, const bool value)
        {
            return GetTableManager()->SetTableBool(name.c_str(), row, col, value);
        }

        bool SetTableInt(const std::string& name, const int row, const int col, const int32_t value)
        {
            return GetTableManager()->SetTableInt(name.c_str(), row, col, value);
        }

        bool SetTableInt64(const std::string& name, const int row, const int col, const int64_t value)
        {
            return GetTableManager()->SetTableInt64(name.c_str(), row, col, value);
        }

        bool SetTableFloat(const std::string& name, const int row, const int col, const float value)
        {
            return GetTableManager()->SetTableFloat(name.c_str(), row, col, value);
        }

        bool SetTableDouble(const std::string& name, const int row, const int col, const double value)
        {
            return GetTableManager()->SetTableDouble(name.c_str(), row, col, value);
        }

        bool SetTableString(const std::string& name, const int row, const int col, const std::string& value)
        {
            return GetTableManager()->SetTableString(name.c_str(), row, col, value.c_str());
        }


        bool GetTableBool(const std::string& name, const int row, const int col)
        {
            return GetTableManager()->GetTableBool(name.c_str(), row, col);
        }

        int32_t GetTableInt(const std::string& name, const int row, const int col)
        {
            return GetTableManager()->GetTableInt(name.c_str(), row, col);
        }

        int64_t GetTableInt64(const std::string& name, const int row, const int col)
        {
            return GetTableManager()->GetTableInt64(name.c_str(), row, col);
        }

        float GetTableFloat(const std::string& name, const int row, const int col)
        {
            return GetTableManager()->GetTableFloat(name.c_str(), row, col);
        }

        double GetTableDouble(const std::string& name, const int row, const int col)
        {
            return GetTableManager()->GetTableDouble(name.c_str(), row, col);
        }

        const char* GetTableString(const std::string& name, const int row, const int col)
        {
            return GetTableManager()->GetTableString(name.c_str(), row, col);
        }

        //////////////////////////////////////////////////////////////////////////
        ARK_SHARE_PTR<AFIDataNodeManager>& GetNodeManager() override
        {
            return m_pNodeManager;
        }

        ARK_SHARE_PTR<AFIDataTableManager>& GetTableManager() override
        {
            return m_pTableManager;
        }

        ARK_SHARE_PTR<AFIEventManager>& GetEventManager() override
        {
            return m_pEventManager;
        }

    private:
        AFGUID mSelf;

        ARK_SHARE_PTR<AFIDataNodeManager> m_pNodeManager;
        ARK_SHARE_PTR<AFIDataTableManager> m_pTableManager;
        ARK_SHARE_PTR<AFIEventManager> m_pEventManager;
    };

}