/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
*
* Licensed under the Apache License, Version 2.0 (the 'License');
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an 'AS IS' BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

# pragma once

# include <string>

namespace ark
{
	//////////////////////////////////
	//Entity meta
	class AFEntityMetaPlayer: public AFEntityMetaBaseEntity
	{
	public:
		static const std::string& self_name() { static const std:: string meta_Player_ = "Player"; return meta_Player_; }

		static const std::string& openid() { static const std::string openid_ = "openid"; return openid_; } // string
		static const std::string& id() { static const std::string id_ = "id"; return id_; } // int64
		static const std::string& name() { static const std::string name_ = "name"; return name_; } // string
		static const std::string& gender() { static const std::string gender_ = "gender"; return gender_; } // int
		static const std::string& career() { static const std::string career_ = "career"; return career_; } // int
		static const std::string& camp() { static const std::string camp_ = "camp"; return camp_; } // int
		static const std::string& last_map_id() { static const std::string last_map_id_ = "last_map_id"; return last_map_id_; } // int
		static const std::string& level() { static const std::string level_ = "level"; return level_; } // int
		static const std::string& exp() { static const std::string exp_ = "exp"; return exp_; } // int64
		static const std::string& gold() { static const std::string gold_ = "gold"; return gold_; } // int64
		static const std::string& money() { static const std::string money_ = "money"; return money_; } // int64
		static const std::string& connect_key() { static const std::string connect_key_ = "connect_key"; return connect_key_; } // string
		static const std::string& online_count() { static const std::string online_count_ = "online_count"; return online_count_; } // int
		static const std::string& total_online_time() { static const std::string total_online_time_ = "total_online_time"; return total_online_time_; } // int
		static const std::string& online_time() { static const std::string online_time_ = "online_time"; return online_time_; } // int
		static const std::string& last_offline_time() { static const std::string last_offline_time_ = "last_offline_time"; return last_offline_time_; } // int
		static const std::string& load_data_finished() { static const std::string load_data_finished_ = "load_data_finished"; return load_data_finished_; } // int
		static const std::string& game_id() { static const std::string game_id_ = "game_id"; return game_id_; } // int
		static const std::string& gate_id() { static const std::string gate_id_ = "gate_id"; return gate_id_; } // int
		static const std::string& cur_attr() { static const std::string cur_attr_ = "cur_attr"; return cur_attr_; } // sub_struct - AFEntityMetaCurrentAttr
		static const std::string& items() { static const std::string items_ = "items"; return items_; } // data_table - AFEntityMetaItemBag
		static const std::string& equips() { static const std::string equips_ = "equips"; return equips_; } // data_table - AFEntityMetaEquipBag
	};

	class AFEntityMetaItemBag
	{
	public:
		static const std::string& self_name() { static const std:: string meta_ItemBag_ = "ItemBag"; return meta_ItemBag_; }

		static const std::string& guid() { static const std::string guid_ = "guid"; return guid_; } // int64
		static const std::string& config_id() { static const std::string config_id_ = "config_id"; return config_id_; } // uint32
		static const std::string& count() { static const std::string count_ = "count"; return count_; } // int
		static const std::string& bound() { static const std::string bound_ = "bound"; return bound_; } // bool
		static const std::string& expired_type() { static const std::string expired_type_ = "expired_type"; return expired_type_; } // int
		static const std::string& expired_time() { static const std::string expired_time_ = "expired_time"; return expired_time_; } // int
	};

	class AFEntityMetaBaseEntity
	{
	public:
		static const std::string& self_name() { static const std:: string meta_BaseEntity_ = "BaseEntity"; return meta_BaseEntity_; }

		static const std::string& map_id() { static const std::string map_id_ = "map_id"; return map_id_; } // int
		static const std::string& map_inst_id() { static const std::string map_inst_id_ = "map_inst_id"; return map_inst_id_; } // int
		static const std::string& class_name() { static const std::string class_name_ = "class_name"; return class_name_; } // string
	};

	class AFEntityMetaCurrentAttr
	{
	public:
		static const std::string& self_name() { static const std:: string meta_CurrentAttr_ = "CurrentAttr"; return meta_CurrentAttr_; }

		static const std::string& hp() { static const std::string hp_ = "hp"; return hp_; } // uint64
		static const std::string& mp() { static const std::string mp_ = "mp"; return mp_; } // uint64
		static const std::string& hp_max() { static const std::string hp_max_ = "hp_max"; return hp_max_; } // uint64
	};

	class AFEntityMetaEquipBag
	{
	public:
		static const std::string& self_name() { static const std:: string meta_EquipBag_ = "EquipBag"; return meta_EquipBag_; }

		static const std::string& guid() { static const std::string guid_ = "guid"; return guid_; } // int64
		static const std::string& config_id() { static const std::string config_id_ = "config_id"; return config_id_; } // uint32
	};

	//////////////////////////////////
	//Config meta
	class AFConfigMetaAttribute
	{
	public:
		static const std::string& self_name() { static const std::string meta_attribute_ = "attribute"; return meta_attribute_; }

		static const std::string& id() { static const std::string id_ = "id"; return id_; } // uint32
		static const std::string& career() { static const std::string career_ = "career"; return career_; } // int
		static const std::string& level() { static const std::string level_ = "level"; return level_; } // int
		static const std::string& hp_max() { static const std::string hp_max_ = "hp_max"; return hp_max_; } // uint32
	};

	class AFConfigMetaItem
	{
	public:
		static const std::string& self_name() { static const std::string meta_item_ = "item"; return meta_item_; }

		static const std::string& id() { static const std::string id_ = "id"; return id_; } // int
		static const std::string& type() { static const std::string type_ = "type"; return type_; } // int
		static const std::string& sub_type() { static const std::string sub_type_ = "sub_type"; return sub_type_; } // int
		static const std::string& level() { static const std::string level_ = "level"; return level_; } // int
		static const std::string& career() { static const std::string career_ = "career"; return career_; } // string
		static const std::string& quality() { static const std::string quality_ = "quality"; return quality_; } // int
		static const std::string& show_name_id() { static const std::string show_name_id_ = "show_name_id"; return show_name_id_; } // string
		static const std::string& desc_id() { static const std::string desc_id_ = "desc_id"; return desc_id_; } // string
		static const std::string& comment() { static const std::string comment_ = "comment"; return comment_; } // string
		static const std::string& cd() { static const std::string cd_ = "cd"; return cd_; } // float
		static const std::string& max() { static const std::string max_ = "max"; return max_; } // int
		static const std::string& expired_type() { static const std::string expired_type_ = "expired_type"; return expired_type_; } // int
		static const std::string& expired_time() { static const std::string expired_time_ = "expired_time"; return expired_time_; } // int
		static const std::string& sell_price() { static const std::string sell_price_ = "sell_price"; return sell_price_; } // int
	};

	class AFConfigMetaLanguage
	{
	public:
		static const std::string& self_name() { static const std::string meta_language_ = "language"; return meta_language_; }

		static const std::string& id() { static const std::string id_ = "id"; return id_; } // uint32
		static const std::string& cn() { static const std::string cn_ = "cn"; return cn_; } // string
	};

	class AFConfigMetaMap
	{
	public:
		static const std::string& self_name() { static const std::string meta_map_ = "map"; return meta_map_; }

		static const std::string& id() { static const std::string id_ = "id"; return id_; } // uint32
		static const std::string& name() { static const std::string name_ = "name"; return name_; } // uint32
		static const std::string& desc() { static const std::string desc_ = "desc"; return desc_; } // uint32
		static const std::string& comment() { static const std::string comment_ = "comment"; return comment_; } // string
		static const std::string& max_instance() { static const std::string max_instance_ = "max_instance"; return max_instance_; } // int
		static const std::string& instance_type() { static const std::string instance_type_ = "instance_type"; return instance_type_; } // int
		static const std::string& max_player() { static const std::string max_player_ = "max_player"; return max_player_; } // int
		static const std::string& map_info() { static const std::string map_info_ = "map_info"; return map_info_; } // string
		static const std::string& RelivePos() { static const std::string RelivePos_ = "RelivePos"; return RelivePos_; } // string
		static const std::string& length() { static const std::string length_ = "length"; return length_; } // int
		static const std::string& width() { static const std::string width_ = "width"; return width_; } // int
	};

}