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

#include "SDK/Core/AFCDataList.hpp"
#include "SDK/Core/AFDataNode.hpp"
#include "SDK/Core/AFDataTable.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "AFCPropertyTrailModule.h"

namespace ark
{

    bool AFCPropertyTrailModule::Init()
    {
        m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
        m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
        m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();

        return true;
    }

    void AFCPropertyTrailModule::StartTrail(const AFGUID self)
    {
        LogObjectData(self);
    }

    void AFCPropertyTrailModule::EndTrail(const AFGUID self)
    {
        //Will complete this
    }

    int AFCPropertyTrailModule::LogObjectData(const AFGUID& self)
    {
        ARK_SHARE_PTR<AFIEntity> xEntity = m_pKernelModule->GetEntity(self);

        if (nullptr == xEntity)
        {
            return -1;
        }

        ARK_SHARE_PTR<AFIDataNodeManager> xNodeManager = xEntity->GetNodeManager();

        if (nullptr != xNodeManager)
        {
            size_t nodeCount = xNodeManager->GetNodeCount();

            for (size_t i = 0; i < nodeCount; ++i)
            {
                AFDataNode* pNode = xNodeManager->GetNodeByIndex(i);
                ARK_LOG_TRACE("Player[{}] Node[{}] Value[{}]", self.ToString(), pNode->GetName(), pNode->ToString());
            }
        }

        ARK_SHARE_PTR<AFIDataTableManager> xTableManager = xEntity->GetTableManager();

        if (nullptr != xTableManager)
        {
            size_t tableCount = xTableManager->GetCount();

            for (size_t i = 0; i < tableCount; ++i)
            {
                AFDataTable* pTable = xTableManager->GetTableByIndex(i);
                size_t rowCount = pTable->GetRowCount();

                for (size_t j = 0; j < rowCount; ++j)
                {
                    AFCDataList xDataList;
                    bool ret = pTable->QueryRow(j, xDataList);

                    if (!ret)
                    {
                        continue;
                    }

                    for (size_t k = 0; k < xDataList.GetCount(); ++k)
                    {
                        ARK_LOG_TRACE("Player[{}] Table[{}] Row[{}] Col[{}] Value[{}]", self.ToString(), pTable->GetName(), j, k, xDataList.ToString(k));
                    }
                }
            }
        }

        return 0;
    }

    int AFCPropertyTrailModule::OnObjectPropertyEvent(const AFGUID& self, const std::string& nodeName, const AFIData& oldVar, const AFIData& newVar)
    {
        ARK_LOG_INFO("Trace id[{}] Name[{}] Old[{}] New[{}]", self.ToString(), nodeName, oldVar.GetString(), newVar.GetString());
        return 0;
    }

    int AFCPropertyTrailModule::OnEntityTableEvent(const AFGUID& self, const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar)
    {
        //will add
        return 0;
    }

    int AFCPropertyTrailModule::TrailObjectData(const AFGUID& self)
    {
        //will add
        return 0;
    }

}