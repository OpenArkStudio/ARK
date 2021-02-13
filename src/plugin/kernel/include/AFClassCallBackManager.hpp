/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "base/AFMacros.hpp"
#include "base/container/AFMap.hpp"
#include "base/container/AFList.hpp"
#include "base/AFDefine.hpp"
#include "kernel/include/AFCData.hpp"
#include <set>
#include "kernel/interface/AFIEntity.hpp"

namespace ark {

struct AFDelaySyncRow
{
    AFDelaySyncRow(const uint32_t row)
        : row_(row)
    {
    }

    bool need_clear_{false};
    uint32_t row_{0};
    std::set<AFINode*> node_list_;
};

struct AFDelaySyncTable
{
    bool need_clear_{false};
    std::map<uint32_t, AFDelaySyncRow> row_list_;
};

struct AFDelaySyncContainer
{
    std::set<uint32_t> index_list_;
    std::set<uint32_t> destroy_list_;
};

struct AFDelaySyncData
{
    // node list
    std::set<AFINode*> node_list_;

    // table list
    std::map<uint32_t, AFDelaySyncTable> table_list_;

    // container list
    std::map<uint32_t, AFDelaySyncContainer> container_list_;
};

class AFClassCallBackManager final
{
public:
    // delay sync data
    using DelaySyncMaskData = std::map<ArkDataMask, AFDelaySyncData>;
    using DelaySyncDataList = std::map<guid_t, DelaySyncMaskData>;

    // call back functor
    using NODE_SYNC_FUNCTOR = std::function<int(const guid_t&, const uint32_t, const ArkDataMask, const AFIData&)>;
    using TABLE_SYNC_FUNCTOR =
        std::function<int(const guid_t&, const TABLE_EVENT_DATA&, const ArkDataMask, const AFIData&)>;
    using CONTAINER_SYNC_FUNCTOR = std::function<int(
        const guid_t&, const uint32_t, const ArkDataMask, const ArkContainerOpType, uint32_t, uint32_t)>;

    using DELAY_SYNC_FUNCTOR = std::function<int(const guid_t&, const ArkDataMask, const AFDelaySyncData& data)>;

private:
    // class event list
    std::multimap<int32_t, CLASS_EVENT_FUNCTOR> class_events_;

    // data call backs list
    using NodeCallBacks = std::multimap<int32_t, DATA_NODE_EVENT_FUNCTOR>;
    std::map<uint32_t, NodeCallBacks> data_call_backs_list_;

    // table call backs list
    using TableCallBacks = std::multimap<int32_t, DATA_TABLE_EVENT_FUNCTOR>;
    std::map<uint32_t, TableCallBacks> table_call_backs_list_;

    // container call backs list
    using ContainerCallBacks = std::multimap<int32_t, CONTAINER_EVENT_FUNCTOR>;
    std::map<uint32_t, ContainerCallBacks> container_call_backs_list_;

    // scene event
    using SceneEvents = std::multimap<int32_t, SCENE_EVENT_FUNCTOR>;
    SceneEvents leave_scene_events_;
    SceneEvents enter_scene_events_;

    // move event
    using MoveEvents = std::multimap<int32_t, MOVE_EVENT_FUNCTOR>;
    MoveEvents move_events_;

    // sync call back
    using NodeSyncCallBacks = std::map<ArkDataMask, NODE_SYNC_FUNCTOR>;
    static NodeSyncCallBacks node_sync_call_back_list_;

    using TableSyncCallBacks = std::map<ArkDataMask, TABLE_SYNC_FUNCTOR>;
    static TableSyncCallBacks table_sync_call_back_list_;

    using ContainerSyncCallBack = std::map<ArkDataMask, CONTAINER_SYNC_FUNCTOR>;
    static ContainerSyncCallBack container_sync_call_backs_list_;

    // delay sync call back
    using DataDelaySyncCallBacks = std::map<ArkDataMask, DELAY_SYNC_FUNCTOR>;
    static DataDelaySyncCallBacks delay_sync_call_back_list_;

    // delay data
    static DelaySyncDataList delay_sync_data_list_;

public:
    // add call back
    bool AddClassCallBack(CLASS_EVENT_FUNCTOR&& cb, const int32_t prio);
    bool AddDataCallBack(const uint32_t index, DATA_NODE_EVENT_FUNCTOR&& cb, const int32_t prio);
    bool AddTableCallBack(const uint32_t index, DATA_TABLE_EVENT_FUNCTOR&& cb, const int32_t prio);
    bool AddContainerCallBack(const uint32_t index, CONTAINER_EVENT_FUNCTOR&& cb, const int32_t prio);
    bool AddLeaveSceneEvent(SCENE_EVENT_FUNCTOR&& cb, const int32_t prio);
    bool AddEnterSceneEvent(SCENE_EVENT_FUNCTOR&& cb, const int32_t prio);
    bool AddMoveEvent(MOVE_EVENT_FUNCTOR&& cb, const int32_t prio);

    // data call back
    bool OnClassEvent(const guid_t& id, const std::string& class_name, const ArkEntityEvent eClassEvent,
        const AFIDataList& valueList);
    bool OnNodeCallBack(const guid_t& self, AFINode* pNode, const AFIData& old_data, const AFIData& new_data);
    bool OnTableCallBack(const guid_t& self, const ArkMaskType mask, AFINode* pNode, const TABLE_EVENT_DATA& event_data,
        const AFIData& old_data, const AFIData& new_data);
    bool OnContainerCallBack(const guid_t& self, const uint32_t index, const ArkMaskType mask,
        const ArkContainerOpType op_type, uint32_t src_index, uint32_t dest_index,
        std::shared_ptr<AFIEntity> src_entity = nullptr);

    // scene call back
    bool OnLeaveSceneEvent(const guid_t& self, const int map_id, const int map_inst_id);
    bool OnEnterSceneEvent(const guid_t& self, const int map_id, const int map_inst_id);

    // move call back
    bool OnMoveEvent(const guid_t& self, const AFVector3D& old_pos, const AFVector3D& new_pos);

    // data sync call back
    static void AddNodeSyncCallBack(const ArkDataMask mask_value, NODE_SYNC_FUNCTOR&& cb);
    static void AddTableSyncCallBack(const ArkDataMask mask_value, TABLE_SYNC_FUNCTOR&& cb);
    static void AddContainerSyncCallBack(const ArkDataMask mask_value, CONTAINER_SYNC_FUNCTOR&& cb);
    static void AddDelaySyncCallBack(const ArkDataMask mask_value, DELAY_SYNC_FUNCTOR&& cb);

    // delay sync
    static bool OnDelaySync();

private:
    DelaySyncMaskData& GetDelaySyncMaskData(const guid_t& self);
    AFDelaySyncTable& GetDelaySyncMaskTable(
        DelaySyncMaskData& mask_data_map, const ArkDataMask mask_value, uint32_t index);
    AFDelaySyncContainer& GetDelaySyncMaskContainer(
        DelaySyncMaskData& mask_data_map, const ArkDataMask mask_value, uint32_t index);

    void UpdateDelayNodeList(const guid_t& self, AFINode* pNode);
    void UpdateDelayTableList(const guid_t& self, const ArkMaskType mask, const uint32_t table_index,
        const uint32_t row_index, ArkTableOpType op_type, AFINode* pNode);
    void UpdateDelayContainerList(const guid_t& self, const uint32_t index, const ArkMaskType mask,
        const ArkContainerOpType op_type, uint32_t src_index, uint32_t dest_index,
        std::shared_ptr<AFIEntity> src_entity);

    void OnTableAdd(AFDelaySyncTable& table, const int row);
    void OnTableDelete(AFDelaySyncTable& table, const int row);
    void OnTableRowUpdate(AFDelaySyncTable& table, const int row, AFINode* pNode);
    void OnTableClear(AFDelaySyncTable& table);

    void OnContainerPlace(AFDelaySyncContainer& container, uint32_t index);
    void OnContainerRemove(AFDelaySyncContainer& container, uint32_t index);
    void OnContainerDestroy(AFDelaySyncContainer& container, uint32_t index, std::shared_ptr<AFIEntity> src_entity);
    void OnContainerSwap(AFDelaySyncContainer& container, uint32_t src_index, uint32_t dest_index);
};

} // namespace ark
