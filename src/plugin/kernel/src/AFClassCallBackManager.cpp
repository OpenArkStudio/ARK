/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "kernel/include/AFClassCallBackManager.hpp"

namespace ark {

AFClassCallBackManager::NodeSyncCallBacks AFClassCallBackManager::node_sync_call_back_list_;
AFClassCallBackManager::TableSyncCallBacks AFClassCallBackManager::table_sync_call_back_list_;
AFClassCallBackManager::ContainerSyncCallBack AFClassCallBackManager::container_sync_call_backs_list_;
AFClassCallBackManager::DataDelaySyncCallBacks AFClassCallBackManager::delay_sync_call_back_list_;
AFClassCallBackManager::DelaySyncDataList AFClassCallBackManager::delay_sync_data_list_;

bool AFClassCallBackManager::AddClassCallBack(CLASS_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    class_events_.insert(std::make_pair(prio, std::forward<CLASS_EVENT_FUNCTOR>(cb)));
    return true;
}

bool AFClassCallBackManager::AddDataCallBack(const uint32_t index, DATA_NODE_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto iter = data_call_backs_list_.find(index);
    if (iter == data_call_backs_list_.end())
    {
        NodeCallBacks call_backs;
        call_backs.insert(std::make_pair(prio, std::forward<DATA_NODE_EVENT_FUNCTOR>(cb)));
        data_call_backs_list_.insert(std::make_pair(index, call_backs));
    }
    else
    {
        iter->second.insert(std::make_pair(prio, std::forward<DATA_NODE_EVENT_FUNCTOR>(cb)));
    }

    return true;
}

bool AFClassCallBackManager::AddTableCallBack(const uint32_t index, DATA_TABLE_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto iter = table_call_backs_list_.find(index);
    if (iter == table_call_backs_list_.end())
    {
        TableCallBacks call_backs;

        call_backs.insert(std::make_pair(prio, std::forward<DATA_TABLE_EVENT_FUNCTOR>(cb)));
        table_call_backs_list_.insert(std::make_pair(index, call_backs));
    }
    else
    {
        iter->second.insert(std::make_pair(prio, std::forward<DATA_TABLE_EVENT_FUNCTOR>(cb)));
    }

    return true;
}

bool AFClassCallBackManager::AddContainerCallBack(
    const uint32_t index, CONTAINER_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto iter = container_call_backs_list_.find(index);
    if (iter == container_call_backs_list_.end())
    {
        ContainerCallBacks call_backs;

        call_backs.insert(std::make_pair(prio, std::forward<CONTAINER_EVENT_FUNCTOR>(cb)));
        container_call_backs_list_.insert(std::make_pair(index, call_backs));
    }
    else
    {
        iter->second.insert(std::make_pair(prio, std::forward<CONTAINER_EVENT_FUNCTOR>(cb)));
    }

    return true;
}

bool AFClassCallBackManager::OnClassEvent(
    const AFGUID& id, const std::string& class_name, const ArkEntityEvent eClassEvent, const AFIDataList& valueList)
{
    for (auto& iter : class_events_)
    {
        iter.second(id, class_name, eClassEvent, valueList);
    }

    return true;
}

bool AFClassCallBackManager::OnNodeCallBack(
    const AFGUID& self, AFINode* pNode, const AFIData& old_data, const AFIData& new_data)
{
    auto index = pNode->GetIndex();
    auto& name = pNode->GetName();

    // sync call back
    if (pNode->HaveMask(ArkDataMask::PF_REAL_TIME))
    {
        for (auto& iter : node_sync_call_back_list_)
        {
            auto mask_value = iter.first;
            if (!pNode->HaveMask(mask_value))
            {
                continue;
            }

            iter.second(self, index, mask_value, new_data);
        }
    }
    else
    {
        UpdateDelayNodeList(self, pNode);
    }

    // common call back
    auto iter_call_backs = data_call_backs_list_.find(index);
    if (iter_call_backs != data_call_backs_list_.end())
    {
        for (auto& iter : iter_call_backs->second)
        {
            iter.second(self, name, index, old_data, new_data);
        }
    }

    return true;
}

bool AFClassCallBackManager::OnTableCallBack(const AFGUID& self, const ArkMaskType mask, AFINode* pNode,
    const TABLE_EVENT_DATA& event_data, const AFIData& old_data, const AFIData& new_data)
{
    // sync call back
    if (mask.test((size_t)ArkDataMask::PF_REAL_TIME))
    {
        for (auto& iter : table_sync_call_back_list_)
        {
            auto mask_value = iter.first;
            if (!mask.test((size_t)mask_value))
            {
                continue;
            }

            iter.second(self, event_data, mask_value, new_data);
        }
    }
    else
    {
        UpdateDelayTableList(
            self, mask, event_data.table_index_, event_data.row_, (ArkTableOpType)event_data.op_type_, pNode);
    }

    // common call back
    auto iter_call_backs = table_call_backs_list_.find(event_data.table_index_);
    if (iter_call_backs != table_call_backs_list_.end())
    {
        for (auto& iter : iter_call_backs->second)
        {
            iter.second(self, event_data, old_data, new_data);
        }
    }

    return true;
}

bool AFClassCallBackManager::OnContainerCallBack(const AFGUID& self, const uint32_t index, const ArkMaskType mask,
    const ArkContainerOpType op_type, uint32_t src_index, uint32_t dest_index,
    std::shared_ptr<AFIEntity> src_entity /* = nullptr*/)
{
    // sync call back
    if (mask.test((size_t)ArkDataMask::PF_REAL_TIME))
    {
        for (auto& iter : container_sync_call_backs_list_)
        {
            auto mask_value = iter.first;
            if (!mask.test((size_t)mask_value))
            {
                continue;
            }

            iter.second(self, index, mask_value, op_type, src_index, dest_index);
        }
    }
    else
    {
        UpdateDelayContainerList(self, index, mask, op_type, src_index, dest_index, src_entity);
    }

    auto iter_call_backs = container_call_backs_list_.find(index);
    if (iter_call_backs != container_call_backs_list_.end())
    {
        for (auto& iter : iter_call_backs->second)
        {
            iter.second(self, index, op_type, src_index, dest_index);
        }
    }

    return true;
}

// sync call back
void AFClassCallBackManager::AddNodeSyncCallBack(const ArkDataMask mask_value, NODE_SYNC_FUNCTOR&& cb)
{
    node_sync_call_back_list_.insert(std::make_pair(mask_value, std::forward<NODE_SYNC_FUNCTOR>(cb)));
}

void AFClassCallBackManager::AddTableSyncCallBack(const ArkDataMask mask_value, TABLE_SYNC_FUNCTOR&& cb)
{
    table_sync_call_back_list_.insert(std::make_pair(mask_value, std::forward<TABLE_SYNC_FUNCTOR>(cb)));
}

void AFClassCallBackManager::AddContainerSyncCallBack(const ArkDataMask mask_value, CONTAINER_SYNC_FUNCTOR&& cb)
{
    container_sync_call_backs_list_.insert(std::make_pair(mask_value, std::forward<CONTAINER_SYNC_FUNCTOR>(cb)));
}

void AFClassCallBackManager::AddDelaySyncCallBack(const ArkDataMask mask_value, DELAY_SYNC_FUNCTOR&& cb)
{
    delay_sync_call_back_list_.insert(std::make_pair(mask_value, cb));
}

bool AFClassCallBackManager::OnDelaySync()
{
    for (auto& iter : delay_sync_data_list_)
    {
        auto& guid = iter.first;
        auto& mask_data_map = iter.second;
        for (auto& iter_data : mask_data_map)
        {
            auto mask_value = iter_data.first;
            auto iter_call_back = delay_sync_call_back_list_.find(mask_value);
            if (iter_call_back == delay_sync_call_back_list_.end())
            {
                continue;
            }

            iter_call_back->second(guid, mask_value, iter_data.second);
        }
    }

    delay_sync_data_list_.clear();
    return true;
}

AFClassCallBackManager::DelaySyncMaskData& AFClassCallBackManager::GetDelaySyncMaskData(const AFGUID& self)
{
    auto iter_data = delay_sync_data_list_.find(self);
    if (iter_data == delay_sync_data_list_.end())
    {
        DelaySyncMaskData mask_data_map;
        iter_data = delay_sync_data_list_.insert(std::make_pair(self, mask_data_map)).first;
    }

    return iter_data->second;
}

AFDelaySyncTable& AFClassCallBackManager::GetDelaySyncMaskTable(
    DelaySyncMaskData& mask_data_map, const ArkDataMask mask_value, uint32_t index)
{
    auto iter_mask_data = mask_data_map.find(mask_value);
    if (iter_mask_data == mask_data_map.end())
    {
        AFDelaySyncData data;
        iter_mask_data = mask_data_map.insert(std::make_pair(mask_value, data)).first;
    }

    auto& table_list = iter_mask_data->second.table_list_;
    auto iter_table = table_list.find(index);
    if (iter_table == table_list.end())
    {
        AFDelaySyncTable table;
        iter_table = table_list.insert(std::make_pair(index, table)).first;
    }

    return iter_table->second;
}

AFDelaySyncContainer& AFClassCallBackManager::GetDelaySyncMaskContainer(
    DelaySyncMaskData& mask_data_map, const ArkDataMask mask_value, uint32_t index)
{
    auto iter_mask_data = mask_data_map.find(mask_value);
    if (iter_mask_data == mask_data_map.end())
    {
        AFDelaySyncData data;
        iter_mask_data = mask_data_map.insert(std::make_pair(mask_value, data)).first;
    }

    auto& contaier_list = iter_mask_data->second.container_list_;
    auto iter_container = contaier_list.find(index);
    if (iter_container == contaier_list.end())
    {
        AFDelaySyncContainer container;
        iter_container = contaier_list.insert(std::make_pair(index, container)).first;
    }

    return iter_container->second;
}

void AFClassCallBackManager::UpdateDelayNodeList(const AFGUID& self, AFINode* pNode)
{
    // update node list
    auto& mask_data_map = GetDelaySyncMaskData(self);
    for (auto& iter : delay_sync_call_back_list_)
    {
        auto mask_value = iter.first;
        if (!pNode->HaveMask(mask_value))
        {
            continue;
        }

        auto iter_mask_data = mask_data_map.find(mask_value);
        if (iter_mask_data == mask_data_map.end())
        {
            AFDelaySyncData data;
            data.node_list_.insert(pNode);
            mask_data_map.insert(std::make_pair(mask_value, data));
        }
        else
        {
            iter_mask_data->second.node_list_.insert(pNode);
        }
    }
}

void AFClassCallBackManager::UpdateDelayTableList(const AFGUID& self, const ArkMaskType mask,
    const uint32_t table_index, const uint32_t row_index, ArkTableOpType op_type, AFINode* pNode)
{
    auto& mask_data_map = GetDelaySyncMaskData(self);
    for (auto& iter : delay_sync_call_back_list_)
    {
        auto mask_value = iter.first;
        if (!mask.test((size_t)mask_value))
        {
            continue;
        }

        auto& table = GetDelaySyncMaskTable(mask_data_map, mask_value, table_index);
        if (op_type == ArkTableOpType::TABLE_ADD)
        {
            OnTableAdd(table, row_index);
        }
        else if (op_type == ArkTableOpType::TABLE_DELETE)
        {
            OnTableDelete(table, row_index);
        }
        else if (op_type == ArkTableOpType::TABLE_UPDATE)
        {
            OnTableRowUpdate(table, row_index, pNode);
        }
        else if (op_type == ArkTableOpType::TABLE_SWAP)
        {
            // todo : for now there is no operation swap
        }
        else if (op_type == ArkTableOpType::TABLE_CLEAR)
        {
            OnTableClear(table);
        }
    }
}

void AFClassCallBackManager::UpdateDelayContainerList(const AFGUID& self, const uint32_t index, const ArkMaskType mask,
    const ArkContainerOpType op_type, uint32_t src_index, uint32_t dest_index,
    std::shared_ptr<AFIEntity> src_entity = nullptr)
{
    // update container list
    auto& mask_data_map = GetDelaySyncMaskData(self);
    for (auto& iter : delay_sync_call_back_list_)
    {
        auto mask_value = iter.first;
        if (!mask.test((size_t)mask_value))
        {
            continue;
        }

        auto& container = GetDelaySyncMaskContainer(mask_data_map, mask_value, index);
        if (op_type == ArkContainerOpType::OP_PLACE)
        {
            OnContainerPlace(container, src_index);
        }
        else if (op_type == ArkContainerOpType::OP_REMOVE)
        {
            OnContainerRemove(container, src_index);
        }
        else if (op_type == ArkContainerOpType::OP_DESTROY)
        {
            OnContainerDestroy(container, src_index, src_entity);
        }
        else if (op_type == ArkContainerOpType::OP_SWAP)
        {
            OnContainerSwap(container, src_index, dest_index);
        }
    }
}

void AFClassCallBackManager::OnTableAdd(AFDelaySyncTable& table, const int row)
{
    auto iter = table.row_list_.find(row);
    if (iter == table.row_list_.end())
    {
        AFDelaySyncRow row_data(row);
        table.row_list_.insert(std::make_pair(row, row_data));
    }
    else
    {
        iter->second.node_list_.clear();
    }
}

void AFClassCallBackManager::OnTableDelete(AFDelaySyncTable& table, const int row)
{
    auto iter = table.row_list_.find(row);
    if (iter == table.row_list_.end())
    {
        AFDelaySyncRow row_data(row);
        row_data.need_clear_ = true;

        table.row_list_.insert(std::make_pair(row, row_data));
    }
    else
    {
        iter->second.need_clear_ = true;
        iter->second.node_list_.clear();
    }
}

void AFClassCallBackManager::OnTableRowUpdate(AFDelaySyncTable& table, const int row, AFINode* pNode)
{
    auto iter = table.row_list_.find(row);
    if (iter == table.row_list_.end())
    {
        AFDelaySyncRow row_data(row);
        row_data.node_list_.insert(pNode);
        table.row_list_.insert(std::make_pair(row, row_data));
    }
    else
    {
        iter->second.node_list_.insert(pNode);
    }
}

void AFClassCallBackManager::OnTableClear(AFDelaySyncTable& table)
{
    table.need_clear_ = true;
    table.row_list_.clear();
}

void AFClassCallBackManager::OnContainerPlace(AFDelaySyncContainer& container, uint32_t index)
{
    container.index_list_.insert(index);
}

void AFClassCallBackManager::OnContainerRemove(AFDelaySyncContainer& container, uint32_t index)
{
    container.index_list_.insert(index);
}

void AFClassCallBackManager::OnContainerDestroy(
    AFDelaySyncContainer& container, uint32_t index, std::shared_ptr<AFIEntity> src_entity)
{
    if (src_entity->IsSent())
    {
        container.destroy_list_.insert(index);
    }

    container.index_list_.insert(index);
}

void AFClassCallBackManager::OnContainerSwap(AFDelaySyncContainer& container, uint32_t src_index, uint32_t dest_index)
{
    container.index_list_.insert(src_index);
    container.index_list_.insert(dest_index);
}

} // namespace ark
