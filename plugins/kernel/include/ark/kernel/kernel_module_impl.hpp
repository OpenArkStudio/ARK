/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "ark/container/array_map.hpp"
#include "ark/container/map.hpp"
#include "ark/kernel/class_meta_module.hpp"
#include "ark/kernel/config_module.hpp"
#include "ark/kernel/container_manager.hpp"
#include "ark/kernel/data_list.hpp"
#include "ark/kernel/entity.hpp"
#include "ark/kernel/event_manager.hpp"
#include "ark/kernel/kernel_module.hpp"
#include "ark/kernel/map_module.hpp"
#include "ark/kernel/node_manager.hpp"
#include "ark/kernel/table_manager.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/utility/guid_module.hpp"

namespace ark {

class kernel_module_impl final : public kernel_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit kernel_module_impl();
    ~kernel_module_impl() override;

    bool init() override;
    bool update() override;
    bool pre_shut() override;

    ///////////////////////////////////////////////////////////////////////
    std::shared_ptr<static_entity> get_static_entity(const id_type config_id) override;
    std::shared_ptr<entity> get_entity(const guid_t& self) override;
    std::shared_ptr<entity> create_entity(const guid_t& self, const int nSceneID, const int nGroupID,
        const std::string& strClassName, const id_type config_id, const data_list& arg) override;

    std::shared_ptr<entity> create_container_entity(const guid_t& self, const uint32_t container_index,
        const std::string& class_name, const id_type config_id) override;

    bool destroy_all() override;
    bool destroy_entity(const guid_t& self) override;

    //////////////////////////////////////////////////////////////////////////
    bool log_info(const guid_t& id) override;
    bool log_self_info(const guid_t& id);
    int log_object_data(const guid_t& guid) override;
    //////////////////////////////////////////////////////////////////////////

    bool do_event(
        const guid_t& self, const std::string& class_name, entity_event class_event, const data_list& args) override;
    bool do_event(const guid_t& self, const int event_id, const data_list& args) override;

    bool exist(const guid_t& self) override;

    bool entity_to_db_data(const guid_t& self, AFMsg::pb_db_entity& pb_data) override;
    std::shared_ptr<entity> create_entity(const AFMsg::pb_db_entity& pb_data) override;

    // send message
    bool send_custom_message(const guid_t& target, const uint32_t msg_id, const data_list& args) override;

    // entity to pb
    bool node_to_pb_data(const uint32_t index, const data& data, AFMsg::pb_entity_data* pb_data) override;
    bool row_to_pb_data(row* pRow, const uint32_t index, AFMsg::pb_entity_data* pb_data) override;
    bool table_row_data_to_pb_data(const uint32_t index, uint32_t row, const uint32_t col, const data& data,
        AFMsg::pb_entity_data* pb_data) override;
    bool entity_node_to_pb_data(
        std::shared_ptr<entity> pEntity, AFMsg::pb_entity_data* pb_data, const mask_type mask = 0) override;
    bool entity_table_to_pb_data(
        std::shared_ptr<entity> pEntity, AFMsg::pb_entity_data* pb_data, const mask_type mask = 0) override;

protected:
    bool destroy_self(const guid_t& self);

    bool inner_destroy_entity(std::shared_ptr<entity> pEntity);

    bool add_event_call_back(const guid_t& self, const int event_id, event_process_functor&& cb) override;
    bool add_class_call_back(const std::string& class_name, class_event_functor&& cb, const int32_t prio) override;
    bool add_node_call_back(const std::string& class_name, const std::string& name, data_node_event_functor&& cb,
        const int32_t prio) override;
    bool add_table_call_back(const std::string& class_name, const std::string& name, data_table_event_functor&& cb,
        const int32_t prio) override;

    bool add_node_call_back(
        const std::string& class_name, const uint32_t index, data_node_event_functor&& cb, const int32_t prio) override;
    bool add_table_call_back(const std::string& class_name, const uint32_t index, data_table_event_functor&& cb,
        const int32_t prio) override;

    bool add_container_call_back(
        const std::string& class_name, const uint32_t index, container_event_functor&& cb, const int32_t prio) override;
    bool add_common_container_call_back(container_event_functor&& cb, const int32_t prio) override;

    bool add_common_class_event(class_event_functor&& cb, const int32_t prio) override;

    bool add_leave_scene_event(const std::string& class_name, scene_event_functor&& cb, const int32_t prio) override;
    bool add_enter_scene_event(const std::string& class_name, scene_event_functor&& cb, const int32_t prio) override;

    bool add_move_event(const std::string& class_name, move_event_functor&& cb, const int32_t prio) override;

    // data sync call back
    void add_sync_call_back();

    int on_sync_node(const guid_t& self, const uint32_t index, const data_mask mask_value, const data& data);
    int on_sync_table(const guid_t& self, const table_event_data& event, const data_mask mask_value, const data& data);
    int on_sync_container(const guid_t& self, const uint32_t index, const data_mask mask,
        const container_op_type op_type, uint32_t src_index, uint32_t dest_index);
    int on_delay_sync_data(const guid_t& self, const data_mask mask_value, const delay_sync_data& data);

    int on_sync_table_add(const guid_t& self, const table_event_data& event, const data_mask mask_value);
    int on_sync_table_delete(const guid_t& self, const table_event_data& event, const data_mask mask_value);
    int on_sync_table_update(
        const guid_t& self, const table_event_data& event, const data_mask mask_value, const data& data);

    int on_sync_container_place(const guid_t& self, const uint32_t index, const data_mask mask, uint32_t src_index);
    int on_sync_container_remove(const guid_t& self, const uint32_t index, const data_mask mask, uint32_t src_index);
    int on_sync_container_destroy(const guid_t& self, const uint32_t index, const data_mask mask, uint32_t src_index);
    int on_sync_container_swap(
        const guid_t& self, const uint32_t index, const data_mask mask, uint32_t src_index, uint32_t dest_index);

    bool delay_table_to_pb(const delay_sync_table& table, const uint32_t index, AFMsg::pb_delay_entity& data,
        AFMsg::pb_entity_data& pb_entity);
    bool delay_container_to_pb(std::shared_ptr<entity> pEntity, const delay_sync_container& container,
        const uint32_t index, AFMsg::pb_delay_entity& data);

    bool try_add_container_pb_entity(std::shared_ptr<container> pContainer, const guid_t& self,
        AFMsg::pb_entity_data& pb_entity_data, guid_t& parent_id, AFMsg::pb_entity_data*& pb_container_entity);

    int send_sync_msg(const guid_t& self, const data_mask mask_value, const google::protobuf::Message& msg);
    int send_to_view(const guid_t& self, const google::protobuf::Message& msg);
    int send_to_self(const guid_t& self, const google::protobuf::Message& msg);

    // convert db data and entity
    bool entity_to_db_data(std::shared_ptr<entity> pEntity, AFMsg::pb_db_entity& pb_data);
    bool node_to_db_data(node* pNode, AFMsg::pb_db_entity_data& pb_data);
    bool table_to_db_data(table* pTable, AFMsg::pb_db_table& pb_data);

    bool db_data_to_node(std::shared_ptr<node_manager> pNodeManager, const AFMsg::pb_db_entity_data& pb_db_entity_data);
    bool db_data_to_table(std::shared_ptr<entity> pEntity, const std::string& name, const AFMsg::pb_db_table& pb_table);
    bool db_data_to_container(
        std::shared_ptr<entity> pEntity, const std::string& name, const AFMsg::pb_db_container& pb_data);

    // convert client pb and entity
    bool node_to_pb_data(node* pNode, AFMsg::pb_entity_data* pb_data);
    bool table_to_pb_data(table* pTable, const uint32_t index, AFMsg::pb_table* pb_data);
    bool container_to_pb_data(std::shared_ptr<container> pContainer, AFMsg::pb_container* pb_data);

    bool entity_to_pb_data(std::shared_ptr<entity> pEntity, AFMsg::pb_entity* pb_data);
    bool entity_to_pb_data_by_mask(std::shared_ptr<entity> pEntity, mask_type mask, AFMsg::pb_entity* pb_data);

    bool entity_container_to_pb_data(
        std::shared_ptr<entity> pEntity, AFMsg::pb_entity_data* pb_data, const mask_type mask = 0);

    // call back
    int on_container_call_back(const guid_t& self, const uint32_t index, const container_op_type op_type,
        const uint32_t src_index, const uint32_t dest_index);

    bool copy_data(std::shared_ptr<entity> pEntity, std::shared_ptr<static_entity> pStaticEntity);

    // get entity data
    std::shared_ptr<node_manager> get_node_manager(std::shared_ptr<static_entity> pStaticEntity) const;
    std::shared_ptr<node_manager> get_node_manager(std::shared_ptr<entity> pEntity) const;
    std::shared_ptr<node_manager> get_node_manager(row* pRow) const;
    std::shared_ptr<table_manager> get_table_manager(std::shared_ptr<entity> pEntity) const;
    std::shared_ptr<container_manager> get_container_manager(std::shared_ptr<entity> pEntity) const;
    std::shared_ptr<event_manager> get_event_manager(std::shared_ptr<entity> pEntity) const;

private:
    std::list<guid_t> delete_list_;

    guid_t cur_exec_object_{NULL_GUID};

    using sync_functor = std::function<bool(const guid_t&, const google::protobuf::Message&)>;
    std::map<data_mask, sync_functor> sync_functors;

    map_module* m_pMapModule{nullptr};
    guid_module* m_pGUIDModule{nullptr};
    config_module* m_pConfigModule{nullptr};
    class_meta_module* m_pClassModule{nullptr};

    array_map<std::string, int32_t> inner_nodes_;
    shared_object_map<guid_t, entity> objects_;
};

} // namespace ark
