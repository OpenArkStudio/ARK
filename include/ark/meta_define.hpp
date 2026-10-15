/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2020 ArkNX authors.
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

#pragma once

#include <string>
#include <map>
#include <unordered_map>
namespace ark {
//////////////////////////////////
//Entity meta
enum class BaseEntity : std::uint32_t
{
    enum_empty,
    map_id,
    map_inst_id,
    class_name,
    config_id,
};

using BaseEntity_rep_type = std::underlying_type<BaseEntity>::type;

class AFEntityMetaBaseEntity
{
public:
    AFEntityMetaBaseEntity() = default;
    static const std::string& self_name()
    {
        static const std::string meta_BaseEntity_ = "BaseEntity";
        return meta_BaseEntity_;
    }

    static const std::string& map_id()
    {
        static const std::string map_id_ = "map_id";
        return map_id_;
    } // int
    static uint32_t map_id_index()
    {
        static const int map_id_index_ = static_cast<BaseEntity_rep_type>(BaseEntity::map_id);
        return map_id_index_;
    } // map_id
    static const std::string& map_inst_id()
    {
        static const std::string map_inst_id_ = "map_inst_id";
        return map_inst_id_;
    } // int
    static uint32_t map_inst_id_index()
    {
        static const int map_inst_id_index_ = static_cast<BaseEntity_rep_type>(BaseEntity::map_inst_id);
        return map_inst_id_index_;
    } // map_inst_id
    static const std::string& class_name()
    {
        static const std::string class_name_ = "class_name";
        return class_name_;
    } // string
    static uint32_t class_name_index()
    {
        static const int class_name_index_ = static_cast<BaseEntity_rep_type>(BaseEntity::class_name);
        return class_name_index_;
    } // class_name
    static const std::string& config_id()
    {
        static const std::string config_id_ = "config_id";
        return config_id_;
    } // uint32
    static uint32_t config_id_index()
    {
        static const int config_id_index_ = static_cast<BaseEntity_rep_type>(BaseEntity::config_id);
        return config_id_index_;
    } // config_id
};

enum class CurrentAttr : std::uint32_t
{
    enum_empty,
    hp,
    mp,
    hp_max,
    mp_max,
};

using CurrentAttr_rep_type = std::underlying_type<CurrentAttr>::type;

class AFEntityMetaCurrentAttr
{
public:
    AFEntityMetaCurrentAttr() = default;
    static const std::string& self_name()
    {
        static const std::string meta_CurrentAttr_ = "CurrentAttr";
        return meta_CurrentAttr_;
    }

    static const std::string& hp()
    {
        static const std::string hp_ = "hp";
        return hp_;
    } // uint64
    static uint32_t hp_index()
    {
        static const int hp_index_ = static_cast<CurrentAttr_rep_type>(CurrentAttr::hp);
        return hp_index_;
    } // hp
    static const std::string& mp()
    {
        static const std::string mp_ = "mp";
        return mp_;
    } // uint64
    static uint32_t mp_index()
    {
        static const int mp_index_ = static_cast<CurrentAttr_rep_type>(CurrentAttr::mp);
        return mp_index_;
    } // mp
    static const std::string& hp_max()
    {
        static const std::string hp_max_ = "hp_max";
        return hp_max_;
    } // uint64
    static uint32_t hp_max_index()
    {
        static const int hp_max_index_ = static_cast<CurrentAttr_rep_type>(CurrentAttr::hp_max);
        return hp_max_index_;
    } // hp_max
    static const std::string& mp_max()
    {
        static const std::string mp_max_ = "mp_max";
        return mp_max_;
    } // uint64
    static uint32_t mp_max_index()
    {
        static const int mp_max_index_ = static_cast<CurrentAttr_rep_type>(CurrentAttr::mp_max);
        return mp_max_index_;
    } // mp_max
};

enum class Player : std::uint32_t
{
    enum_empty,
    //parent entity class enum
    map_id,
    map_inst_id,
    class_name,
    config_id,
    hp,
    mp,
    hp_max,
    mp_max,

    //self entity class enum
    openid,
    id,
    name,
    gender,
    career,
    camp,
    last_map_id,
    level,
    exp,
    gold,
    money,
    connect_key,
    online_count,
    total_online_time,
    online_time,
    last_offline_time,
    load_data_finished,
    game_id,
    gate_id,
    items,
    bag,
};

using Player_rep_type = std::underlying_type<Player>::type;

class AFEntityMetaPlayer
{
public:
    AFEntityMetaPlayer() = default;
    static const std::string& self_name()
    {
        static const std::string meta_Player_ = "Player";
        return meta_Player_;
    }

    //parent entity class
    static const std::string& map_id()
    {
        static const std::string map_id_ = "map_id";
        return map_id_;
    } // int
    static uint32_t map_id_index()
    {
        static const int map_id_index_ = static_cast<Player_rep_type>(Player::map_id);
        return map_id_index_;
    } // map_id
    static const std::string& map_inst_id()
    {
        static const std::string map_inst_id_ = "map_inst_id";
        return map_inst_id_;
    } // int
    static uint32_t map_inst_id_index()
    {
        static const int map_inst_id_index_ = static_cast<Player_rep_type>(Player::map_inst_id);
        return map_inst_id_index_;
    } // map_inst_id
    static const std::string& class_name()
    {
        static const std::string class_name_ = "class_name";
        return class_name_;
    } // string
    static uint32_t class_name_index()
    {
        static const int class_name_index_ = static_cast<Player_rep_type>(Player::class_name);
        return class_name_index_;
    } // class_name
    static const std::string& config_id()
    {
        static const std::string config_id_ = "config_id";
        return config_id_;
    } // uint32
    static uint32_t config_id_index()
    {
        static const int config_id_index_ = static_cast<Player_rep_type>(Player::config_id);
        return config_id_index_;
    } // config_id
    static const std::string& hp()
    {
        static const std::string hp_ = "hp";
        return hp_;
    } // uint64
    static uint32_t hp_index()
    {
        static const int hp_index_ = static_cast<Player_rep_type>(Player::hp);
        return hp_index_;
    } // hp
    static const std::string& mp()
    {
        static const std::string mp_ = "mp";
        return mp_;
    } // uint64
    static uint32_t mp_index()
    {
        static const int mp_index_ = static_cast<Player_rep_type>(Player::mp);
        return mp_index_;
    } // mp
    static const std::string& hp_max()
    {
        static const std::string hp_max_ = "hp_max";
        return hp_max_;
    } // uint64
    static uint32_t hp_max_index()
    {
        static const int hp_max_index_ = static_cast<Player_rep_type>(Player::hp_max);
        return hp_max_index_;
    } // hp_max
    static const std::string& mp_max()
    {
        static const std::string mp_max_ = "mp_max";
        return mp_max_;
    } // uint64
    static uint32_t mp_max_index()
    {
        static const int mp_max_index_ = static_cast<Player_rep_type>(Player::mp_max);
        return mp_max_index_;
    } // mp_max

    //self entity class
    static const std::string& openid()
    {
        static const std::string openid_ = "openid";
        return openid_;
    } // string
    static uint32_t openid_index()
    {
        static const int openid_index_ = static_cast<Player_rep_type>(Player::openid);
        return openid_index_;
    } // openid
    static const std::string& id()
    {
        static const std::string id_ = "id";
        return id_;
    } // uint64
    static uint32_t id_index()
    {
        static const int id_index_ = static_cast<Player_rep_type>(Player::id);
        return id_index_;
    } // id
    static const std::string& name()
    {
        static const std::string name_ = "name";
        return name_;
    } // string
    static uint32_t name_index()
    {
        static const int name_index_ = static_cast<Player_rep_type>(Player::name);
        return name_index_;
    } // name
    static const std::string& gender()
    {
        static const std::string gender_ = "gender";
        return gender_;
    } // int
    static uint32_t gender_index()
    {
        static const int gender_index_ = static_cast<Player_rep_type>(Player::gender);
        return gender_index_;
    } // gender
    static const std::string& career()
    {
        static const std::string career_ = "career";
        return career_;
    } // int
    static uint32_t career_index()
    {
        static const int career_index_ = static_cast<Player_rep_type>(Player::career);
        return career_index_;
    } // career
    static const std::string& camp()
    {
        static const std::string camp_ = "camp";
        return camp_;
    } // int
    static uint32_t camp_index()
    {
        static const int camp_index_ = static_cast<Player_rep_type>(Player::camp);
        return camp_index_;
    } // camp
    static const std::string& last_map_id()
    {
        static const std::string last_map_id_ = "last_map_id";
        return last_map_id_;
    } // int
    static uint32_t last_map_id_index()
    {
        static const int last_map_id_index_ = static_cast<Player_rep_type>(Player::last_map_id);
        return last_map_id_index_;
    } // last_map_id
    static const std::string& level()
    {
        static const std::string level_ = "level";
        return level_;
    } // int
    static uint32_t level_index()
    {
        static const int level_index_ = static_cast<Player_rep_type>(Player::level);
        return level_index_;
    } // level
    static const std::string& exp()
    {
        static const std::string exp_ = "exp";
        return exp_;
    } // int64
    static uint32_t exp_index()
    {
        static const int exp_index_ = static_cast<Player_rep_type>(Player::exp);
        return exp_index_;
    } // exp
    static const std::string& gold()
    {
        static const std::string gold_ = "gold";
        return gold_;
    } // int64
    static uint32_t gold_index()
    {
        static const int gold_index_ = static_cast<Player_rep_type>(Player::gold);
        return gold_index_;
    } // gold
    static const std::string& money()
    {
        static const std::string money_ = "money";
        return money_;
    } // int64
    static uint32_t money_index()
    {
        static const int money_index_ = static_cast<Player_rep_type>(Player::money);
        return money_index_;
    } // money
    static const std::string& connect_key()
    {
        static const std::string connect_key_ = "connect_key";
        return connect_key_;
    } // string
    static uint32_t connect_key_index()
    {
        static const int connect_key_index_ = static_cast<Player_rep_type>(Player::connect_key);
        return connect_key_index_;
    } // connect_key
    static const std::string& online_count()
    {
        static const std::string online_count_ = "online_count";
        return online_count_;
    } // int
    static uint32_t online_count_index()
    {
        static const int online_count_index_ = static_cast<Player_rep_type>(Player::online_count);
        return online_count_index_;
    } // online_count
    static const std::string& total_online_time()
    {
        static const std::string total_online_time_ = "total_online_time";
        return total_online_time_;
    } // int
    static uint32_t total_online_time_index()
    {
        static const int total_online_time_index_ = static_cast<Player_rep_type>(Player::total_online_time);
        return total_online_time_index_;
    } // total_online_time
    static const std::string& online_time()
    {
        static const std::string online_time_ = "online_time";
        return online_time_;
    } // int
    static uint32_t online_time_index()
    {
        static const int online_time_index_ = static_cast<Player_rep_type>(Player::online_time);
        return online_time_index_;
    } // online_time
    static const std::string& last_offline_time()
    {
        static const std::string last_offline_time_ = "last_offline_time";
        return last_offline_time_;
    } // int
    static uint32_t last_offline_time_index()
    {
        static const int last_offline_time_index_ = static_cast<Player_rep_type>(Player::last_offline_time);
        return last_offline_time_index_;
    } // last_offline_time
    static const std::string& load_data_finished()
    {
        static const std::string load_data_finished_ = "load_data_finished";
        return load_data_finished_;
    } // int
    static uint32_t load_data_finished_index()
    {
        static const int load_data_finished_index_ = static_cast<Player_rep_type>(Player::load_data_finished);
        return load_data_finished_index_;
    } // load_data_finished
    static const std::string& game_id()
    {
        static const std::string game_id_ = "game_id";
        return game_id_;
    } // int
    static uint32_t game_id_index()
    {
        static const int game_id_index_ = static_cast<Player_rep_type>(Player::game_id);
        return game_id_index_;
    } // game_id
    static const std::string& gate_id()
    {
        static const std::string gate_id_ = "gate_id";
        return gate_id_;
    } // int
    static uint32_t gate_id_index()
    {
        static const int gate_id_index_ = static_cast<Player_rep_type>(Player::gate_id);
        return gate_id_index_;
    } // gate_id
    static const std::string& items()
    {
        static const std::string items_ = "items";
        return items_;
    } // table
    static uint32_t items_index()
    {
        static const int items_index_ = static_cast<Player_rep_type>(Player::items);
        return items_index_;
    } // items
    static const std::string& bag()
    {
        static const std::string bag_ = "bag";
        return bag_;
    } // container
    static uint32_t bag_index()
    {
        static const int bag_index_ = static_cast<Player_rep_type>(Player::bag);
        return bag_index_;
    } // bag
};

enum class ItemBag : std::uint32_t
{
    enum_empty,

    //self entity class enum
    guid,
    config_id,
    count,
    bound,
    expired_type,
    expired_time,
};

using ItemBag_rep_type = std::underlying_type<ItemBag>::type;

class AFEntityMetaItemBag
{
public:
    AFEntityMetaItemBag() = default;
    static const std::string& self_name()
    {
        static const std::string meta_ItemBag_ = "ItemBag";
        return meta_ItemBag_;
    }

    //self entity class
    static const std::string& guid()
    {
        static const std::string guid_ = "guid";
        return guid_;
    } // int64
    static uint32_t guid_index()
    {
        static const int guid_index_ = static_cast<ItemBag_rep_type>(ItemBag::guid);
        return guid_index_;
    } // guid
    static const std::string& config_id()
    {
        static const std::string config_id_ = "config_id";
        return config_id_;
    } // uint32
    static uint32_t config_id_index()
    {
        static const int config_id_index_ = static_cast<ItemBag_rep_type>(ItemBag::config_id);
        return config_id_index_;
    } // config_id
    static const std::string& count()
    {
        static const std::string count_ = "count";
        return count_;
    } // int
    static uint32_t count_index()
    {
        static const int count_index_ = static_cast<ItemBag_rep_type>(ItemBag::count);
        return count_index_;
    } // count
    static const std::string& bound()
    {
        static const std::string bound_ = "bound";
        return bound_;
    } // bool
    static uint32_t bound_index()
    {
        static const int bound_index_ = static_cast<ItemBag_rep_type>(ItemBag::bound);
        return bound_index_;
    } // bound
    static const std::string& expired_type()
    {
        static const std::string expired_type_ = "expired_type";
        return expired_type_;
    } // int
    static uint32_t expired_type_index()
    {
        static const int expired_type_index_ = static_cast<ItemBag_rep_type>(ItemBag::expired_type);
        return expired_type_index_;
    } // expired_type
    static const std::string& expired_time()
    {
        static const std::string expired_time_ = "expired_time";
        return expired_time_;
    } // int
    static uint32_t expired_time_index()
    {
        static const int expired_time_index_ = static_cast<ItemBag_rep_type>(ItemBag::expired_time);
        return expired_time_index_;
    } // expired_time
};

enum class EquipBag : std::uint32_t
{
    enum_empty,

    //self entity class enum
    guid,
    config_id,
};

using EquipBag_rep_type = std::underlying_type<EquipBag>::type;

class AFEntityMetaEquipBag
{
public:
    AFEntityMetaEquipBag() = default;
    static const std::string& self_name()
    {
        static const std::string meta_EquipBag_ = "EquipBag";
        return meta_EquipBag_;
    }

    //self entity class
    static const std::string& guid()
    {
        static const std::string guid_ = "guid";
        return guid_;
    } // int64
    static uint32_t guid_index()
    {
        static const int guid_index_ = static_cast<EquipBag_rep_type>(EquipBag::guid);
        return guid_index_;
    } // guid
    static const std::string& config_id()
    {
        static const std::string config_id_ = "config_id";
        return config_id_;
    } // uint32
    static uint32_t config_id_index()
    {
        static const int config_id_index_ = static_cast<EquipBag_rep_type>(EquipBag::config_id);
        return config_id_index_;
    } // config_id
};

//////////////////////////////////
//Config meta
enum class attribute : std::uint32_t
{
    meta_empty,
    id,
    career,
    level,
    hp_max,
    mp_max,
};

using attribute_rep_type = std::underlying_type<attribute>::type;

class AFConfigMetaAttribute
{
public:
    static const std::string& self_name()
    {
        static const std::string meta_attribute_ = "attribute";
        return meta_attribute_;
    }

    static const std::string& id()
    {
        static const std::string id_ = "id";
        return id_;
    } // uint64
    static uint32_t id_index()
    {
        static const int id_index_ = static_cast<attribute_rep_type>(attribute::id);
        return id_index_;
    } // id
    static const std::string& career()
    {
        static const std::string career_ = "career";
        return career_;
    } // int
    static uint32_t career_index()
    {
        static const int career_index_ = static_cast<attribute_rep_type>(attribute::career);
        return career_index_;
    } // career
    static const std::string& level()
    {
        static const std::string level_ = "level";
        return level_;
    } // int
    static uint32_t level_index()
    {
        static const int level_index_ = static_cast<attribute_rep_type>(attribute::level);
        return level_index_;
    } // level
    static const std::string& hp_max()
    {
        static const std::string hp_max_ = "hp_max";
        return hp_max_;
    } // uint32
    static uint32_t hp_max_index()
    {
        static const int hp_max_index_ = static_cast<attribute_rep_type>(attribute::hp_max);
        return hp_max_index_;
    } // hp_max
    static const std::string& mp_max()
    {
        static const std::string mp_max_ = "mp_max";
        return mp_max_;
    } // uint32
    static uint32_t mp_max_index()
    {
        static const int mp_max_index_ = static_cast<attribute_rep_type>(attribute::mp_max);
        return mp_max_index_;
    } // mp_max
};

enum class item : std::uint32_t
{
    meta_empty,
    id,
    type,
    sub_type,
    level,
    career,
    quality,
    show_name_id,
    desc_id,
    comment,
    cd,
    max,
    expired_type,
    expired_time,
    sell_price,
    icon,
};

using item_rep_type = std::underlying_type<item>::type;

class AFConfigMetaItem
{
public:
    static const std::string& self_name()
    {
        static const std::string meta_item_ = "item";
        return meta_item_;
    }

    static const std::string& id()
    {
        static const std::string id_ = "id";
        return id_;
    } // uint64
    static uint32_t id_index()
    {
        static const int id_index_ = static_cast<item_rep_type>(item::id);
        return id_index_;
    } // id
    static const std::string& type()
    {
        static const std::string type_ = "type";
        return type_;
    } // int
    static uint32_t type_index()
    {
        static const int type_index_ = static_cast<item_rep_type>(item::type);
        return type_index_;
    } // type
    static const std::string& sub_type()
    {
        static const std::string sub_type_ = "sub_type";
        return sub_type_;
    } // int
    static uint32_t sub_type_index()
    {
        static const int sub_type_index_ = static_cast<item_rep_type>(item::sub_type);
        return sub_type_index_;
    } // sub_type
    static const std::string& level()
    {
        static const std::string level_ = "level";
        return level_;
    } // int
    static uint32_t level_index()
    {
        static const int level_index_ = static_cast<item_rep_type>(item::level);
        return level_index_;
    } // level
    static const std::string& career()
    {
        static const std::string career_ = "career";
        return career_;
    } // string
    static uint32_t career_index()
    {
        static const int career_index_ = static_cast<item_rep_type>(item::career);
        return career_index_;
    } // career
    static const std::string& quality()
    {
        static const std::string quality_ = "quality";
        return quality_;
    } // int
    static uint32_t quality_index()
    {
        static const int quality_index_ = static_cast<item_rep_type>(item::quality);
        return quality_index_;
    } // quality
    static const std::string& show_name_id()
    {
        static const std::string show_name_id_ = "show_name_id";
        return show_name_id_;
    } // string
    static uint32_t show_name_id_index()
    {
        static const int show_name_id_index_ = static_cast<item_rep_type>(item::show_name_id);
        return show_name_id_index_;
    } // show_name_id
    static const std::string& desc_id()
    {
        static const std::string desc_id_ = "desc_id";
        return desc_id_;
    } // string
    static uint32_t desc_id_index()
    {
        static const int desc_id_index_ = static_cast<item_rep_type>(item::desc_id);
        return desc_id_index_;
    } // desc_id
    static const std::string& comment()
    {
        static const std::string comment_ = "comment";
        return comment_;
    } // string
    static uint32_t comment_index()
    {
        static const int comment_index_ = static_cast<item_rep_type>(item::comment);
        return comment_index_;
    } // comment
    static const std::string& cd()
    {
        static const std::string cd_ = "cd";
        return cd_;
    } // float
    static uint32_t cd_index()
    {
        static const int cd_index_ = static_cast<item_rep_type>(item::cd);
        return cd_index_;
    } // cd
    static const std::string& max()
    {
        static const std::string max_ = "max";
        return max_;
    } // int
    static uint32_t max_index()
    {
        static const int max_index_ = static_cast<item_rep_type>(item::max);
        return max_index_;
    } // max
    static const std::string& expired_type()
    {
        static const std::string expired_type_ = "expired_type";
        return expired_type_;
    } // int
    static uint32_t expired_type_index()
    {
        static const int expired_type_index_ = static_cast<item_rep_type>(item::expired_type);
        return expired_type_index_;
    } // expired_type
    static const std::string& expired_time()
    {
        static const std::string expired_time_ = "expired_time";
        return expired_time_;
    } // int
    static uint32_t expired_time_index()
    {
        static const int expired_time_index_ = static_cast<item_rep_type>(item::expired_time);
        return expired_time_index_;
    } // expired_time
    static const std::string& sell_price()
    {
        static const std::string sell_price_ = "sell_price";
        return sell_price_;
    } // int
    static uint32_t sell_price_index()
    {
        static const int sell_price_index_ = static_cast<item_rep_type>(item::sell_price);
        return sell_price_index_;
    } // sell_price
    static const std::string& icon()
    {
        static const std::string icon_ = "icon";
        return icon_;
    } // string
    static uint32_t icon_index()
    {
        static const int icon_index_ = static_cast<item_rep_type>(item::icon);
        return icon_index_;
    } // icon
};

enum class language : std::uint32_t
{
    meta_empty,
    id,
    cn,
    en,
};

using language_rep_type = std::underlying_type<language>::type;

class AFConfigMetaLanguage
{
public:
    static const std::string& self_name()
    {
        static const std::string meta_language_ = "language";
        return meta_language_;
    }

    static const std::string& id()
    {
        static const std::string id_ = "id";
        return id_;
    } // uint64
    static uint32_t id_index()
    {
        static const int id_index_ = static_cast<language_rep_type>(language::id);
        return id_index_;
    } // id
    static const std::string& cn()
    {
        static const std::string cn_ = "cn";
        return cn_;
    } // string
    static uint32_t cn_index()
    {
        static const int cn_index_ = static_cast<language_rep_type>(language::cn);
        return cn_index_;
    } // cn
    static const std::string& en()
    {
        static const std::string en_ = "en";
        return en_;
    } // string
    static uint32_t en_index()
    {
        static const int en_index_ = static_cast<language_rep_type>(language::en);
        return en_index_;
    } // en
};

enum class map : std::uint32_t
{
    meta_empty,
    id,
    name,
    desc,
    comment,
    max_instance,
    instance_type,
    max_player,
    map_info,
    RelivePos,
    length,
    width,
    sound,
};

using map_rep_type = std::underlying_type<map>::type;

class AFConfigMetaMap
{
public:
    static const std::string& self_name()
    {
        static const std::string meta_map_ = "map";
        return meta_map_;
    }

    static const std::string& id()
    {
        static const std::string id_ = "id";
        return id_;
    } // uint64
    static uint32_t id_index()
    {
        static const int id_index_ = static_cast<map_rep_type>(map::id);
        return id_index_;
    } // id
    static const std::string& name()
    {
        static const std::string name_ = "name";
        return name_;
    } // uint32
    static uint32_t name_index()
    {
        static const int name_index_ = static_cast<map_rep_type>(map::name);
        return name_index_;
    } // name
    static const std::string& desc()
    {
        static const std::string desc_ = "desc";
        return desc_;
    } // uint32
    static uint32_t desc_index()
    {
        static const int desc_index_ = static_cast<map_rep_type>(map::desc);
        return desc_index_;
    } // desc
    static const std::string& comment()
    {
        static const std::string comment_ = "comment";
        return comment_;
    } // string
    static uint32_t comment_index()
    {
        static const int comment_index_ = static_cast<map_rep_type>(map::comment);
        return comment_index_;
    } // comment
    static const std::string& max_instance()
    {
        static const std::string max_instance_ = "max_instance";
        return max_instance_;
    } // int
    static uint32_t max_instance_index()
    {
        static const int max_instance_index_ = static_cast<map_rep_type>(map::max_instance);
        return max_instance_index_;
    } // max_instance
    static const std::string& instance_type()
    {
        static const std::string instance_type_ = "instance_type";
        return instance_type_;
    } // int
    static uint32_t instance_type_index()
    {
        static const int instance_type_index_ = static_cast<map_rep_type>(map::instance_type);
        return instance_type_index_;
    } // instance_type
    static const std::string& max_player()
    {
        static const std::string max_player_ = "max_player";
        return max_player_;
    } // int
    static uint32_t max_player_index()
    {
        static const int max_player_index_ = static_cast<map_rep_type>(map::max_player);
        return max_player_index_;
    } // max_player
    static const std::string& map_info()
    {
        static const std::string map_info_ = "map_info";
        return map_info_;
    } // string
    static uint32_t map_info_index()
    {
        static const int map_info_index_ = static_cast<map_rep_type>(map::map_info);
        return map_info_index_;
    } // map_info
    static const std::string& RelivePos()
    {
        static const std::string RelivePos_ = "RelivePos";
        return RelivePos_;
    } // string
    static uint32_t RelivePos_index()
    {
        static const int RelivePos_index_ = static_cast<map_rep_type>(map::RelivePos);
        return RelivePos_index_;
    } // RelivePos
    static const std::string& length()
    {
        static const std::string length_ = "length";
        return length_;
    } // int
    static uint32_t length_index()
    {
        static const int length_index_ = static_cast<map_rep_type>(map::length);
        return length_index_;
    } // length
    static const std::string& width()
    {
        static const std::string width_ = "width";
        return width_;
    } // int
    static uint32_t width_index()
    {
        static const int width_index_ = static_cast<map_rep_type>(map::width);
        return width_index_;
    } // width
    static const std::string& sound()
    {
        static const std::string sound_ = "sound";
        return sound_;
    } // string
    static uint32_t sound_index()
    {
        static const int sound_index_ = static_cast<map_rep_type>(map::sound);
        return sound_index_;
    } // sound
};

class AFMetaNameIndex
{
public:
    static uint32_t GetIndex(const std::string& class_name, const std::string& field_name)
    {
        static const std::unordered_map<std::string, std::unordered_map<std::string, std::uint32_t>> class_name_index =
            {
                {"BaseEntity",
                    {
                        {"map_id", static_cast<BaseEntity_rep_type>(BaseEntity::map_id)},
                        {"map_inst_id", static_cast<BaseEntity_rep_type>(BaseEntity::map_inst_id)},
                        {"class_name", static_cast<BaseEntity_rep_type>(BaseEntity::class_name)},
                        {"config_id", static_cast<BaseEntity_rep_type>(BaseEntity::config_id)},
                    }},
                {"CurrentAttr",
                    {
                        {"hp", static_cast<CurrentAttr_rep_type>(CurrentAttr::hp)},
                        {"mp", static_cast<CurrentAttr_rep_type>(CurrentAttr::mp)},
                        {"hp_max", static_cast<CurrentAttr_rep_type>(CurrentAttr::hp_max)},
                        {"mp_max", static_cast<CurrentAttr_rep_type>(CurrentAttr::mp_max)},
                    }},
                {"Player",
                    {
                        {"map_id", static_cast<Player_rep_type>(Player::map_id)},
                        {"map_inst_id", static_cast<Player_rep_type>(Player::map_inst_id)},
                        {"class_name", static_cast<Player_rep_type>(Player::class_name)},
                        {"config_id", static_cast<Player_rep_type>(Player::config_id)},
                        {"openid", static_cast<Player_rep_type>(Player::openid)},
                        {"id", static_cast<Player_rep_type>(Player::id)},
                        {"name", static_cast<Player_rep_type>(Player::name)},
                        {"gender", static_cast<Player_rep_type>(Player::gender)},
                        {"career", static_cast<Player_rep_type>(Player::career)},
                        {"camp", static_cast<Player_rep_type>(Player::camp)},
                        {"last_map_id", static_cast<Player_rep_type>(Player::last_map_id)},
                        {"level", static_cast<Player_rep_type>(Player::level)},
                        {"exp", static_cast<Player_rep_type>(Player::exp)},
                        {"gold", static_cast<Player_rep_type>(Player::gold)},
                        {"money", static_cast<Player_rep_type>(Player::money)},
                        {"connect_key", static_cast<Player_rep_type>(Player::connect_key)},
                        {"online_count", static_cast<Player_rep_type>(Player::online_count)},
                        {"total_online_time", static_cast<Player_rep_type>(Player::total_online_time)},
                        {"online_time", static_cast<Player_rep_type>(Player::online_time)},
                        {"last_offline_time", static_cast<Player_rep_type>(Player::last_offline_time)},
                        {"load_data_finished", static_cast<Player_rep_type>(Player::load_data_finished)},
                        {"game_id", static_cast<Player_rep_type>(Player::game_id)},
                        {"gate_id", static_cast<Player_rep_type>(Player::gate_id)},
                        {"hp", static_cast<Player_rep_type>(Player::hp)},
                        {"mp", static_cast<Player_rep_type>(Player::mp)},
                        {"hp_max", static_cast<Player_rep_type>(Player::hp_max)},
                        {"mp_max", static_cast<Player_rep_type>(Player::mp_max)},
                        {"items", static_cast<Player_rep_type>(Player::items)},
                        {"bag", static_cast<Player_rep_type>(Player::bag)},
                    }},
                {"ItemBag",
                    {
                        {"guid", static_cast<ItemBag_rep_type>(ItemBag::guid)},
                        {"config_id", static_cast<ItemBag_rep_type>(ItemBag::config_id)},
                        {"count", static_cast<ItemBag_rep_type>(ItemBag::count)},
                        {"bound", static_cast<ItemBag_rep_type>(ItemBag::bound)},
                        {"expired_type", static_cast<ItemBag_rep_type>(ItemBag::expired_type)},
                        {"expired_time", static_cast<ItemBag_rep_type>(ItemBag::expired_time)},
                    }},
                {"EquipBag",
                    {
                        {"guid", static_cast<EquipBag_rep_type>(EquipBag::guid)},
                        {"config_id", static_cast<EquipBag_rep_type>(EquipBag::config_id)},
                    }},
                {"attribute",
                    {
                        {"id", static_cast<attribute_rep_type>(attribute::id)},
                        {"career", static_cast<attribute_rep_type>(attribute::career)},
                        {"level", static_cast<attribute_rep_type>(attribute::level)},
                        {"hp_max", static_cast<attribute_rep_type>(attribute::hp_max)},
                        {"mp_max", static_cast<attribute_rep_type>(attribute::mp_max)},
                    }},
                {"item",
                    {
                        {"id", static_cast<item_rep_type>(item::id)},
                        {"type", static_cast<item_rep_type>(item::type)},
                        {"sub_type", static_cast<item_rep_type>(item::sub_type)},
                        {"level", static_cast<item_rep_type>(item::level)},
                        {"career", static_cast<item_rep_type>(item::career)},
                        {"quality", static_cast<item_rep_type>(item::quality)},
                        {"show_name_id", static_cast<item_rep_type>(item::show_name_id)},
                        {"desc_id", static_cast<item_rep_type>(item::desc_id)},
                        {"comment", static_cast<item_rep_type>(item::comment)},
                        {"cd", static_cast<item_rep_type>(item::cd)},
                        {"max", static_cast<item_rep_type>(item::max)},
                        {"expired_type", static_cast<item_rep_type>(item::expired_type)},
                        {"expired_time", static_cast<item_rep_type>(item::expired_time)},
                        {"sell_price", static_cast<item_rep_type>(item::sell_price)},
                        {"icon", static_cast<item_rep_type>(item::icon)},
                    }},
                {"language",
                    {
                        {"id", static_cast<language_rep_type>(language::id)},
                        {"cn", static_cast<language_rep_type>(language::cn)},
                        {"en", static_cast<language_rep_type>(language::en)},
                    }},
                {"map",
                    {
                        {"id", static_cast<map_rep_type>(map::id)},
                        {"name", static_cast<map_rep_type>(map::name)},
                        {"desc", static_cast<map_rep_type>(map::desc)},
                        {"comment", static_cast<map_rep_type>(map::comment)},
                        {"max_instance", static_cast<map_rep_type>(map::max_instance)},
                        {"instance_type", static_cast<map_rep_type>(map::instance_type)},
                        {"max_player", static_cast<map_rep_type>(map::max_player)},
                        {"map_info", static_cast<map_rep_type>(map::map_info)},
                        {"RelivePos", static_cast<map_rep_type>(map::RelivePos)},
                        {"length", static_cast<map_rep_type>(map::length)},
                        {"width", static_cast<map_rep_type>(map::width)},
                        {"sound", static_cast<map_rep_type>(map::sound)},
                    }},
            };
        auto iter_class = class_name_index.find(class_name);
        if (iter_class == class_name_index.end())
        {
            return 0;
        }
        auto iter_field = iter_class->second.find(field_name);
        if (iter_field == iter_class->second.end())
        {
            return 0;
        }
        return iter_field->second;
    }
};
} // namespace ark