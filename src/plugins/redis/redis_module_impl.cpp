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
#include "ark/define.hpp"
#include "ark/enum.hpp"
#include "ark/redis/redis_module_impl.hpp"
#include "ark/redis/redis_plugin.hpp"
#include "ark/xml.hpp"

namespace ark {

bool redis_module_impl::init() {
    if (!load_config())
    {
        return false;
    }
    redis_manager::instance()->create_pool(pool_size_, ip_, port_);
    ;
    return connect();
}

bool redis_module_impl::load_config() {
    ///load config
    xml xml(redis_plugin::get_plugin_conf());
    auto root = xml.get_root_node();
    ARK_ASSERT_RET_VAL(root.is_valid(), false);

    std::unordered_map<std::string, std::string> key_value_list;
    for (auto set = root.find_node("set"); set.is_valid(); set.next_node())
    {
        auto name = set.get_string("name");
        auto value = set.get_string("value");
        bool result = key_value_list.insert(std::make_pair(name, value)).second;
        ARK_ASSERT_RET_VAL(result, false);
    }

    auto func = [&key_value_list](const std::string& name) -> const std::string& {
        auto iter = key_value_list.find(name);
        if (iter == key_value_list.end())
        {
            return null_str;
        }

        return iter->second;
    };

    ip_ = func("Ip");
    port_ = misc::from_string<uint16_t>(func("Port"));
    pool_size_ = misc::from_string<uint32_t>(func("PoolSize"));
    auth_keys_ = func("Password");
    return true;
}

bool redis_module_impl::connect() {
    auto all_conn = redis_manager::instance()->get_all_conn();
    for (auto iter : all_conn)
    {
        auto ret = iter->connect();
        if (!ret)
        {
            return false;
        }
        ret = auth_pwd(auth_keys_, iter);
        if (!ret)
        {
            return false;
        }
    }
    ARK_LOG_INFO("Connect to Redis successfully! ip = {}, port = {}", ip_, port_);
    return true;
}

bool redis_module_impl::auth_pwd(const std::string& password, redis_conn* c) {
    if (password.empty())
    {
        return true;
    }

    auto conn = c->get_redis_conn();
    if (conn == nullptr)
    {
        return false;
    }

    redis_buffer rx_buff;
    try
    {
        conn->write(bredis::single_command_t("AUTH", password));
        auto result_markers = conn->read(rx_buff);
        auto err_code = is_ok(result_markers.result);
        rx_buff.consume(result_markers.consumed);
        if (err_code != redis_err_code::ok)
        {
            return false;
        }
    }
    catch ([[maybe_unused]] boost::system::system_error& ec)
    {
        ARK_LOG_ERROR("Redis authentication Failed");
        return false;
    }
    return true;
}

bool redis_module_impl::reconnect() {
    redis_manager::instance()->remove_pool();
    redis_manager::instance()->reconnect();
    return true;
}

bool redis_module_impl::disconnect() {
    redis_manager::instance()->remove_pool();
    return true;
}

//Synchronous Commands Implement

redis_err_code redis_module_impl::get(const std::string& key, std::string& value) {
    return on_ret_str_command("GET", key, value);
}

redis_err_code redis_module_impl::set(const std::string& key, const std::string& value, const uint64_t expire_time) {
    if (expire_time > 0)
    {
        return on_ret_nil_command("SETEX", key, expire_time, value);
    }
    else
    {
        return on_ret_nil_command("SET", key, value);
    }
}

redis_err_code redis_module_impl::set_nx(const std::string& key, const std::string& value) {
    return on_ret_nil_command("SETNX", key, value);
}

redis_err_code redis_module_impl::h_set(const std::string& key, const std::string& field, const std::string& value) {
    return on_ret_nil_command("HSET", key, field, value);
}

redis_err_code redis_module_impl::get_set(const std::string& key, const std::string& value, std::string& str) {
    return on_ret_str_command("GETSET", key, str, value);
}

redis_err_code redis_module_impl::h_get(const std::string& key, const std::string& field, std::string& value) {
    return on_ret_str_command("HGET", key, value, field);
}

redis_err_code redis_module_impl::m_set(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key) {
    return ret_nil_vec_command(command, key, commands);
}

redis_err_code redis_module_impl::incr(const std::string& key, int64_t& value) {
    return on_ret_int_command("INCR", key, value);
}

redis_err_code redis_module_impl::l_len(const std::string& key, int64_t& length) {
    return on_ret_int_command("LLEN", key, length);
}

redis_err_code redis_module_impl::l_range(
    const std::string& key, const int64_t& begin, const int64_t& end, value_vector<std::string>& res_list) {
    return on_ret_array_command("LRANGE", key, res_list, begin, end);
}

redis_err_code redis_module_impl::l_pop(const std::string& key, std::string& value) {
    return on_ret_str_command("LPOP", key, value);
}

redis_err_code redis_module_impl::r_pop(const std::string& key, std::string& value) {
    return on_ret_str_command("RPOP", key, value);
}

redis_err_code redis_module_impl::s_card(const std::string& key, std::int64_t& value) {
    return on_ret_int_command("SCARD", key, value);
}

redis_err_code redis_module_impl::s_members(const std::string& key, value_vector<std::string>& res_list) {
    return on_ret_array_command("SMEMBERS", key, res_list);
}

redis_err_code redis_module_impl::s_rand_member(
    const std::string& key, const uint32_t& count, value_vector<std::string>& res_list) {
    return on_ret_array_command("SRANDMEMBER", key, res_list, count);
}

redis_err_code redis_module_impl::s_pop(
    const std::string& key, const uint32_t& count, value_vector<std::string>& res_list) {
    return on_ret_array_command("SPOP", key, res_list, count);
}

redis_err_code redis_module_impl::s_pop(const std::string& key, std::string& value) {
    return on_ret_str_command("SPOP", key, value);
}

redis_err_code redis_module_impl::s_is_member(const std::string& key, const std::string& member, bool& res) {
    int64_t tmp_value;
    auto err_code = on_ret_int_command("SISMEMBER", key, tmp_value, member);
    if (err_code == redis_err_code::ok)
    {
        res = (tmp_value == 1);
    }
    return err_code;
}

redis_err_code redis_module_impl::z_incr_by(
    const std::string& key, const double& increment, const std::string& member, double& result_value) {
    std::string tmp_str;
    auto err_code = on_ret_str_command("ZINCRBY", key, tmp_str, increment, member);
    if (err_code == redis_err_code::ok)
    {
        result_value = std::stod(tmp_str);
    }
    return err_code;
}

redis_err_code redis_module_impl::z_range(const std::string& key, const int64_t& begin, const int64_t& end,
    value_vector<std::string>& res_list, bool with_score) {
    if (with_score)
    {
        return on_ret_array_command("ZRANGE", key, res_list, begin, end, "WITHSCORES");
    }
    return on_ret_array_command("ZRANGE", key, res_list, begin, end);
}

redis_err_code redis_module_impl::z_range_by_score(const std::string& key, const std::string& min,
    const std::string& max, value_vector<std::string>& res_list, bool with_score) {
    if (with_score)
    {
        return on_ret_array_command("ZRANGEBYSCORE", key, res_list, min, max, "WITHSCORES");
    }
    return on_ret_array_command("ZRANGEBYSCORE", key, res_list, min, max);
}

redis_err_code redis_module_impl::z_range_by_score(const std::string& key, const std::string& min,
    const std::string& max, const int64_t& offset, const uint32_t& count, value_vector<std::string>& res_list,
    bool with_score) {
    if (with_score)
    {
        return on_ret_array_command("ZRANGEBYSCORE", key, res_list, min, max, "WITHSCORES", "LIMIT", offset, count);
    }
    return on_ret_array_command("ZRANGEBYSCORE", key, res_list, min, max, "LIMIT", offset, count);
}

redis_err_code redis_module_impl::z_rev_range(const std::string& key, const std::int64_t& begin, const int64_t& end,
    value_vector<std::string>& res_list, bool with_score) {
    if (with_score)
    {
        return on_ret_array_command("ZREVRANGE", key, res_list, begin, end, "WITHSCORES");
    }
    return on_ret_array_command("ZREVRANGE", key, res_list, begin, end);
}

redis_err_code redis_module_impl::z_rev_range_by_score(const std::string& key, const std::string& min,
    const std::string& max, value_vector<std::string>& res_list, bool with_score) {
    if (with_score)
    {
        return on_ret_array_command("ZREVRANGEBYSCORE", key, res_list, min, max, "WITHSCORES");
    }
    return on_ret_array_command("ZREVRANGEBYSCORE", key, res_list, min, max);
}

redis_err_code redis_module_impl::z_rev_range_by_score(const std::string& key, const std::string& min,
    const std::string& max, const int64_t& offset, const uint32_t& count, value_vector<std::string>& res_list,
    bool with_score) {
    if (with_score)
    {
        return on_ret_array_command("ZREVRANGEBYSCORE", key, res_list, min, max, "WITHSCORES", "LIMIT", offset, count);
    }
    return on_ret_array_command("ZREVRANGEBYSCORE", key, res_list, min, max, "LIMIT", offset, count);
}

redis_err_code redis_module_impl::z_rank(const std::string& key, const std::string& member, int64_t& rank_res) {
    return on_ret_int_command("ZRANK", key, rank_res, member);
}

redis_err_code redis_module_impl::z_rev_rank(const std::string& key, const std::string& member, int64_t& rank_res) {
    return on_ret_int_command("ZREVRANK", key, rank_res, member);
}

redis_err_code redis_module_impl::z_card(const std::string& key, int64_t& count) {
    return on_ret_int_command("ZCARD", key, count);
}

redis_err_code redis_module_impl::z_count(
    const std::string& key, const std::string& min, const std::string& max, int64_t& count) {
    return on_ret_int_command("ZCOUNT", key, count, min, max);
}

redis_err_code redis_module_impl::z_score(const std::string& key, const std::string member, double& score) {
    std::string tmp_str;
    auto err_code = on_ret_str_command("ZSCORE", key, tmp_str, member);
    if (err_code == redis_err_code::ok)
    {
        score = std::stod(tmp_str);
    }
    return err_code;
}

redis_err_code redis_module_impl::z_rem_range_by_rank(
    const std::string& key, const int64_t& begin, const int64_t& end, int64_t& count) {
    return on_ret_int_command("ZREMRANGEBYRANK", key, count, begin, end);
}

redis_err_code redis_module_impl::z_rem_range_by_score(
    const std::string& key, const std::string& min, const std::string& max, int64_t& count) {
    return on_ret_int_command("ZREMRANGEBYSCORE", key, count, min, max);
}

redis_err_code redis_module_impl::h_get_all(const std::string& key, value_vector<std::string>& res_list) {
    return on_ret_array_command("HGETALL", key, res_list);
}

redis_err_code redis_module_impl::h_len(const std::string& key, int64_t& length) {
    return on_ret_int_command("HLEN", key, length);
}

redis_err_code redis_module_impl::h_set_nx(const std::string& key, const std::string& field, const std::string& value) {
    return on_ret_nil_command("HSETNX", key, field, value);
}

redis_err_code redis_module_impl::h_exists(const std::string& key, const std::string& field, bool& exist) {
    int64_t tmp_value;
    auto err_code = on_ret_int_command("HEXISTS", key, tmp_value, field);
    if (err_code == redis_err_code::ok)
    {
        exist = (tmp_value == 1);
    }
    return err_code;
}

redis_err_code redis_module_impl::h_keys(const std::string& key, value_vector<std::string>& fields) {
    return on_ret_array_command("HKEYS", key, fields);
}

redis_err_code redis_module_impl::h_del(const std::string& key, const std::string& field, bool& is_deleted) {
    int64_t tmp_value;
    auto err_code = on_ret_int_command("HDEL", key, tmp_value, field);
    if (err_code == redis_err_code::ok)
    {
        is_deleted = (tmp_value == 1);
    }
    return err_code;
}

redis_err_code redis_module_impl::h_incr_by(
    const std::string& key, const std::string& field, const int64_t& increment, int64_t& result_value) {
    return on_ret_int_command("HINCRBY", key, result_value, field, increment);
}

redis_err_code redis_module_impl::h_incr_by_float(
    const std::string& key, const std::string& field, const double& increment, double& result_value) {
    std::string tmp_str;
    auto err_code = on_ret_str_command("HINCRBYFLOAT", key, tmp_str, field, increment);
    if (err_code == redis_err_code::ok)
    {
        result_value = std::stod(tmp_str);
    }
    return err_code;
}

redis_err_code redis_module_impl::h_scan(const std::string& key, int64_t& ret_cursor,
    value_vector<std::string>& res_list, const int64_t& cursor, const std::string& pattern, const uint32_t& count) {
    auto c = redis_manager::instance()->get_conn();
    if (c == nullptr)
    {
        return redis_err_code::err;
    }
    auto conn = c->get_redis_conn();

    if (conn == nullptr)
    {
        return redis_err_code::err;
    }
    redis_buffer rx_buff;

    try
    {
        conn->write(bredis::single_command_t(
            "HSCAN", key, std::to_string(cursor), "MATCH", pattern, "COUNT", std::to_string(count)));
        auto result_markers = conn->read(rx_buff);
        redis_manager::instance()->release_conn(c);
        rx_buff.consume(result_markers.consumed);
        auto err_code = is_ok(result_markers.result);
        if (err_code != redis_err_code::ok)
        {
            return err_code;
        }
        auto extract = boost::apply_visitor(bredis::extractor<iterator>(), result_markers.result);

        auto tmp_v = boost::get<bredis::extracts::array_holder_t>(extract).elements;
        res_list.clear();
        for (auto iter = tmp_v.begin(); iter != tmp_v.end(); ++iter)
        {
            if (iter == tmp_v.begin())
            {
                ret_cursor = std::stoi(boost::get<bredis::extracts::string_t>(*iter).str);
            }
            else
            {
                auto recur_tmp_v = boost::get<bredis::extracts::array_holder_t>(*iter).elements;
                for (auto recur_iter : recur_tmp_v)
                {
                    res_list.emplace_back(boost::get<bredis::extracts::string_t>(recur_iter).str);
                }
            }
        }
        return err_code;
    }
    catch ([[maybe_unused]] boost::system::system_error& ec)
    {
        ARK_LOG_ERROR("Redis Connection Failed with Command: HSCAN and key: {}", key);
        return redis_err_code::err;
    }
}

redis_err_code redis_module_impl::exists(const std::string& key, bool& exist) {
    int64_t tmp_value;
    auto err_code = on_ret_int_command("EXISTS", key, tmp_value);
    if (err_code == redis_err_code::ok)
    {
        exist = (tmp_value == 1);
    }
    return err_code;
}

redis_err_code redis_module_impl::del(const std::string& key) {
    return on_ret_nil_command("DEL", key);
}

redis_err_code redis_module_impl::incr_by(const std::string& key, const int64_t& increment, int64_t& result) {
    return on_ret_int_command("INCRBY", key, result, std::to_string(increment));
}

redis_err_code redis_module_impl::decr_by(const std::string& key, const int64_t& decrement, int64_t& result) {
    return on_ret_int_command("DECRBY", key, result, std::to_string(decrement));
}

redis_err_code redis_module_impl::expire(const std::string& key, const int64_t& expire_time, bool& result) {
    int64_t tmp_value;
    auto err_code = on_ret_int_command("EXPIRE", key, tmp_value);
    if (err_code == redis_err_code::ok)
    {
        result = (tmp_value == 1);
    }
    return err_code;
}

redis_err_code redis_module_impl::m_get(const std::vector<std::string>& commands, const std::string& command,
    const std::string& key, value_vector<std::string>& str_list) {
    return ret_array_vec_command(command, key, str_list, commands);
}

redis_err_code redis_module_impl::l_push(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return ret_int_vec_command(command, key, length, commands);
}

redis_err_code redis_module_impl::r_push(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return ret_int_vec_command(command, key, length, commands);
}

redis_err_code redis_module_impl::s_rem(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return ret_int_vec_command(command, key, length, commands);
}

redis_err_code redis_module_impl::s_add(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return ret_int_vec_command(command, key, length, commands);
}

redis_err_code redis_module_impl::z_add(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return ret_int_vec_command(command, key, length, commands);
}

redis_err_code redis_module_impl::z_rem(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return ret_int_vec_command(command, key, length, commands);
}

//Asynchronous Commands Implement
void redis_module_impl::get(const std::string& key, redis_str_func func) {
    async_on_ret_str_command("GET", key, func);
}

void redis_module_impl::set(
    const std::string& key, const std::string& value, redis_nil_func func, const uint64_t expire_time) {
    async_on_ret_nil_command("SET", key, func, value, expire_time);
}

void redis_module_impl::incr(const std::string& key, redis_int_func func) {
    async_on_ret_int_command("INCR", key, func);
}

void redis_module_impl::h_set(
    const std::string& key, const std::string& field, const std::string& value, redis_nil_func func) {
    async_on_ret_nil_command("HSET", key, func, field, value);
}

void redis_module_impl::h_get(const std::string& key, const std::string& field, redis_str_func func) {
    async_on_ret_str_command("HGET", key, func, field);
}

void redis_module_impl::set_nx(const std::string& key, const std::string& value, redis_nil_func func) {
    async_on_ret_nil_command("SETNX", key, func, value);
}

void redis_module_impl::get_set(const std::string& key, const std::string& value, redis_str_func func) {
    async_on_ret_str_command("GETSET", key, func, value);
}

void redis_module_impl::l_len(const std::string& key, redis_int_func func) {
    async_on_ret_int_command("LLEN", key, func);
}

void redis_module_impl::l_range(
    const std::string& key, const int64_t& begin, const int64_t& end, redis_array_func func) {
    async_on_ret_array_command("LRANGE", key, func, begin, end);
}

void redis_module_impl::l_pop(const std::string& key, redis_str_func func) {
    async_on_ret_str_command("LPOP", key, func);
}

void redis_module_impl::r_pop(const std::string& key, redis_str_func func) {
    async_on_ret_str_command("RPOP", key, func);
}

void redis_module_impl::s_card(const std::string& key, redis_int_func func) {
    async_on_ret_int_command("SCARD", key, func);
}

void redis_module_impl::s_members(const std::string& key, redis_array_func func) {
    async_on_ret_array_command("SMEMBERS", key, func);
}

void redis_module_impl::s_rand_member(const std::string& key, const uint32_t& count, redis_array_func func) {
    async_on_ret_array_command("SRANDMEMBER", key, func, count);
}

void redis_module_impl::s_pop(const std::string& key, const uint32_t& count, redis_array_func func) {
    async_on_ret_array_command("SPOP", key, func, count);
}

void redis_module_impl::s_pop(const std::string& key, redis_str_func func) {
    async_on_ret_str_command("SPOP", key, func);
}

void redis_module_impl::s_is_member(const std::string& key, const std::string& member, redis_int_func func) {
    async_on_ret_int_command("SISMEMBER", key, func, member);
}

void redis_module_impl::z_incr_by(
    const std::string& key, const double& increment, const std::string& member, redis_str_func func) {
    async_on_ret_str_command("ZINCRBY", key, func, increment, member);
}

void redis_module_impl::z_range(
    const std::string& key, const int64_t& begin, const int64_t& end, redis_array_func func, bool with_score) {
    if (with_score)
    {
        async_on_ret_array_command("ZRANGE", key, func, begin, end, "WITHSCORE");
    }
    else
    {
        async_on_ret_array_command("ZRANGE", key, func, begin, end);
    }
}

void redis_module_impl::z_range_by_score(
    const std::string& key, const std::string& min, const std::string& max, redis_array_func func, bool with_score) {
    if (with_score)
    {
        async_on_ret_array_command("ZRANGEBYSCORE", key, func, min, max, "WITHSCORE");
    }
    else
    {
        async_on_ret_array_command("ZRANGEBYSCORE", key, func, min, max);
    }
}

void redis_module_impl::z_range_by_score(const std::string& key, const std::string& min, const std::string& max,
    const int64_t& offset, const uint32_t& count, redis_array_func func, bool with_score) {
    if (with_score)
    {
        async_on_ret_array_command("ZRANGEBYSCORE", key, func, min, max, "WITHSCORE", "LIMIT", offset, count);
    }
    else
    {
        async_on_ret_array_command("ZRANGEBYSCORE", key, func, min, max, "LIMIT", offset, count);
    }
}

void redis_module_impl::z_rev_range(
    const std::string& key, const std::int64_t& begin, const int64_t& end, redis_array_func func, bool with_score) {
    if (with_score)
    {
        async_on_ret_array_command("ZREVRAMGE", key, func, begin, end, "WITHSCORE");
    }
    else
    {
        async_on_ret_array_command("ZRANGEBYSCORE", key, func, begin, end);
    }
}

void redis_module_impl::z_rev_range_by_score(
    const std::string& key, const std::string& min, const std::string& max, redis_array_func func, bool with_score) {
    if (with_score)
    {
        async_on_ret_array_command("ZREVRANGEBYSCORE", key, func, min, max, "WITHSCORES");
    }
    else
    {
        async_on_ret_array_command("ZREVRANGEBYSCORE", key, func, min, max);
    }
}

void redis_module_impl::z_rev_range_by_score(const std::string& key, const std::string& min, const std::string& max,
    const int64_t& offset, const uint32_t& count, redis_array_func func, bool with_score) {
    if (with_score)
    {
        async_on_ret_array_command("ZREVRANGEBYSCORE", key, func, min, max, "WITHSCORES", "LIMIT", offset, count);
    }
    else
    {
        async_on_ret_array_command("ZREVRANGEBYSCORE", key, func, min, max, "LIMIT", offset, count);
    }
}

void redis_module_impl::z_rank(const std::string& key, const std::string& member, redis_int_func func) {
    async_on_ret_int_command("ZRANK", key, func, member);
}

void redis_module_impl::z_rev_rank(const std::string& key, const std::string& member, redis_int_func func) {
    async_on_ret_int_command("ZREVRANK", key, func, member);
}

void redis_module_impl::z_card(const std::string& key, redis_int_func func) {
    async_on_ret_int_command("ZCARD", key, func);
}

void redis_module_impl::z_count(
    const std::string& key, const std::string& min, const std::string& max, redis_int_func func) {
    async_on_ret_int_command("ZCOUNT", key, func, min, max);
}

void redis_module_impl::z_score(const std::string& key, const std::string member, redis_str_func func) {
    async_on_ret_str_command("ZSCORE", key, func, member);
}

void redis_module_impl::z_rem_range_by_rank(
    const std::string& key, const int64_t& begin, const int64_t& end, redis_int_func func) {
    async_on_ret_int_command("ZREMRANGEBYRANK", key, func, begin, end);
}

void redis_module_impl::z_rem_range_by_score(
    const std::string& key, const std::string& min, const std::string& max, redis_int_func func) {
    async_on_ret_int_command("ZREMRANGEBYSCORE", key, func, min, max);
}

void redis_module_impl::h_get_all(const std::string& key, redis_array_func func) {
    async_on_ret_array_command("HGETALL", key, func);
}

void redis_module_impl::h_len(const std::string& key, redis_int_func func) {
    async_on_ret_int_command("HLEN", key, func);
}

void redis_module_impl::h_set_nx(
    const std::string& key, const std::string& field, const std::string& value, redis_nil_func func) {
    async_on_ret_nil_command("HSETNX", key, func, field, value);
}

void redis_module_impl::h_exists(const std::string& key, const std::string& field, redis_int_func func) {
    async_on_ret_int_command("HEXISTS", key, func, field);
}

void redis_module_impl::h_keys(const std::string& key, redis_array_func func) {
    async_on_ret_array_command("HKEYS", key, func);
}

void redis_module_impl::h_del(const std::string& key, const std::string& field, redis_int_func func) {
    async_on_ret_int_command("HDEL", key, func, field);
}

void redis_module_impl::h_incr_by(
    const std::string& key, const std::string& field, const int64_t& increment, redis_int_func func) {
    async_on_ret_int_command("HINCRBY", key, func, field, increment);
}

void redis_module_impl::h_incr_by_float(
    const std::string& key, const std::string& field, const double& increment, redis_str_func func) {
    async_on_ret_str_command("HINCRBYFLOAT", key, func, field, increment);
}

void redis_module_impl::h_scan(const std::string& key,
    void (*hscan_func)(const redis_err_code&, const int64_t&, const std::vector<std::string>&), const int64_t& cursor,
    const std::string& pattern, const uint32_t& count) {
    std::vector<std::string> result_vector;
    auto c = redis_manager::instance()->get_conn();
    if (c == nullptr)
    {
        hscan_func(redis_err_code::disconnect, -1, result_vector);
    }
    auto conn = c->get_redis_conn();
    if (conn == nullptr)
    {
        hscan_func(redis_err_code::disconnect, -1, result_vector);
    }

    auto io = c->get_io();

    redis_buffer rx_buff, tx_buff;

    auto commands = bredis::single_command_t(
        "HSCAN", key, std::to_string(cursor), "MATCH", pattern, "COUNT", std::to_string(count));

    conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec)
        {
            redis_manager::instance()->release_conn(c);
            hscan_func(redis_err_code::disconnect, -1, result_vector);
            ARK_LOG_ERROR("Redis Connection Failed with Command: HSCAN and key: {}", key);
            return;
        }
        tx_buff.consume(bytes_transferred);
        conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
            if (ec)
            {
                redis_manager::instance()->release_conn(c);
                hscan_func(redis_err_code::disconnect, -1, result_vector);
                ARK_LOG_ERROR("Redis Connection Failed with Command: HSCAN and key: {}", key);
                return;
            }
            redis_manager::instance()->release_conn(c);
            rx_buff.consume(r.consumed);
            auto err_code = is_ok(r.result);
            if (err_code != redis_err_code::ok)
            {
                hscan_func(err_code, -1, result_vector);
                return;
            }
            auto extract = boost::apply_visitor(bredis::extractor<iterator>(), r.result);
            auto& v = boost::get<bredis::extracts::array_holder_t>(extract).elements;
            int64_t new_cursor;
            for (auto iter = v.begin(); iter != v.end(); ++iter)
            {
                if (iter == v.begin())
                {
                    new_cursor = std::stoi(boost::get<bredis::extracts::string_t>(*iter).str);
                }
                else
                {
                    auto& tmp_v = boost::get<bredis::extracts::array_holder_t>(*iter).elements;
                    for (auto& tmp_iter : tmp_v)
                    {
                        result_vector.emplace_back(boost::get<bredis::extracts::string_t>(tmp_iter).str);
                    }
                }
            }
            hscan_func(err_code, new_cursor, result_vector);
        });
    });
    io.get().poll();
}

void redis_module_impl::exists(const std::string& key, redis_int_func func) {
    async_on_ret_int_command("EXISTS", key, func);
}

void redis_module_impl::del(const std::string& key, redis_nil_func func) {
    async_on_ret_nil_command("DEL", key, func);
}

void redis_module_impl::incr_by(const std::string& key, const int64_t& increment, redis_int_func func) {
    async_on_ret_int_command("INCRBY", key, func, increment);
}

void redis_module_impl::decr_by(const std::string& key, const int64_t& decrement, redis_int_func func) {
    async_on_ret_int_command("DECRBY", key, func, decrement);
}

void redis_module_impl::expire(const std::string& key, const int64_t& expire_time, redis_int_func func) {
    async_on_ret_int_command("EXPIRE", key, func, expire_time);
}

void redis_module_impl::asyn_m_set(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, redis_nil_func func) {
    asyn_ret_nil_vec_command(command, key, func, commands);
}

void redis_module_impl::asyn_m_get(const std::vector<std::string>& commands, const std::string& command,
    const std::string& key, redis_array_func func) {
    asyn_ret_array_vec_command(command, key, func, commands);
}

void redis_module_impl::asyn_l_push(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, redis_int_func func) {
    asyn_ret_int_vec_command(command, key, func, commands);
}

void redis_module_impl::asyn_r_push(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, redis_int_func func) {
    asyn_ret_int_vec_command(command, key, func, commands);
}

void redis_module_impl::asyn_s_rem(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, redis_int_func func) {
    asyn_ret_int_vec_command(command, key, func, commands);
}

void redis_module_impl::asyn_s_add(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, redis_int_func func) {
    asyn_ret_int_vec_command(command, key, func, commands);
}

void redis_module_impl::asyn_z_add(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, redis_int_func func) {
    asyn_ret_int_vec_command(command, key, func, commands);
}

void redis_module_impl::asyn_z_rem(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, redis_int_func func) {
    asyn_ret_int_vec_command(command, key, func, commands);
}

redis_err_code redis_module_impl::ret_nil_vec_command(
    const std::string& command, const std::string& key, const std::vector<std::string>& args) {
    auto c = redis_manager::instance()->get_conn();
    if (c == nullptr)
    {
        return redis_err_code::disconnect;
    }
    auto conn = c->get_redis_conn();
    if (conn == nullptr)
    {
        return redis_err_code::disconnect;
    }
    redis_buffer rx_buff;

    try
    {
        conn->write(bredis::single_command_t(args.begin(), args.end()));
        auto result_markers = conn->read(rx_buff);
        redis_manager::instance()->release_conn(c);
        auto err_code = is_ok(result_markers.result);
        rx_buff.consume(result_markers.consumed);
        return err_code;
    }
    catch ([[maybe_unused]] boost::system::system_error& ec)
    {
        if (!key.empty())
        {
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
        }
        else
        {
            std::stringstream tmp;
            for (auto iter : args)
            {
                tmp << iter << " ";
            }
            ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
        }
        return redis_err_code::disconnect;
    }
}

redis_err_code redis_module_impl::ret_int_vec_command(
    const std::string& command, const std::string& key, int64_t& value, const std::vector<std::string>& args) {
    auto c = redis_manager::instance()->get_conn();
    if (c == nullptr)
    {
        return redis_err_code::disconnect;
    }
    auto conn = c->get_redis_conn();
    if (conn == nullptr)
    {
        return redis_err_code::disconnect;
    }

    redis_buffer rx_buff;

    try
    {
        conn->write(bredis::single_command_t(args.begin(), args.end()));
        auto result_markers = conn->read(rx_buff);
        redis_manager::instance()->release_conn(c);
        rx_buff.consume(result_markers.consumed);
        auto err_code = is_ok(result_markers.result);
        if (err_code != redis_err_code::ok)
        {
            return err_code;
        }
        auto extract = boost::apply_visitor(bredis::extractor<iterator>(), result_markers.result);
        value = boost::get<bredis::extracts::int_t>(extract);
        return err_code;
    }
    catch ([[maybe_unused]] boost::system::system_error& ec)
    {
        if (!key.empty())
        {
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
        }
        else
        {
            std::stringstream tmp;
            for (auto iter : args)
            {
                tmp << iter << " ";
            }
            ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
        }
        return redis_err_code::disconnect;
    }
}

redis_err_code redis_module_impl::ret_str_vec_command(
    const std::string& command, const std::string& key, std::string& value, const std::vector<std::string>& args) {
    auto c = redis_manager::instance()->get_conn();
    if (c == nullptr)
    {
        return redis_err_code::disconnect;
    }
    auto conn = c->get_redis_conn();
    if (conn == nullptr)
    {
        return redis_err_code::disconnect;
    }

    redis_buffer rx_buff;

    try
    {
        conn->write(bredis::single_command_t(args.begin(), args.end()));

        auto result_markers = conn->read(rx_buff);
        redis_manager::instance()->release_conn(c);
        ;
        rx_buff.consume(result_markers.consumed);
        auto err_code = is_ok(result_markers.result);
        if (err_code != redis_err_code::ok)
        {
            return err_code;
        }
        auto extract = boost::apply_visitor(bredis::extractor<iterator>(), result_markers.result);
        value = boost::get<bredis::extracts::string_t>(extract).str;
        return err_code;
    }
    catch ([[maybe_unused]] boost::system::system_error& ec)
    {
        if (!key.empty())
        {
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
        }
        else
        {
            std::stringstream tmp;
            for (auto iter : args)
            {
                tmp << iter << " ";
            }
            ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
        }
        return redis_err_code::disconnect;
    }
}

redis_err_code redis_module_impl::ret_array_vec_command(const std::string& command, const std::string& key,
    std::vector<std::string>& value, const std::vector<std::string>& args) {
    auto c = redis_manager::instance()->get_conn();
    if (c == nullptr)
    {
        return redis_err_code::disconnect;
    }
    auto conn = c->get_redis_conn();
    if (conn == nullptr)
    {
        return redis_err_code::disconnect;
    }

    redis_buffer rx_buff;
    try
    {
        conn->write(bredis::single_command_t(args.begin(), args.end()));

        auto result_markers = conn->read(rx_buff);
        redis_manager::instance()->release_conn(c);
        rx_buff.consume(result_markers.consumed);
        auto err_code = is_ok(result_markers.result);
        if (err_code != redis_err_code::ok)
        {
            return err_code;
        }
        auto extract = boost::apply_visitor(bredis::extractor<iterator>(), result_markers.result);

        auto tmp_v = boost::get<bredis::extracts::array_holder_t>(extract).elements;
        value.clear();
        for (auto iter : tmp_v)
        {
            value.emplace_back(boost::get<bredis::extracts::string_t>(iter).str);
        }
        return err_code;
    }
    catch ([[maybe_unused]] boost::system::system_error& ec)
    {
        if (!key.empty())
        {
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
        }
        else
        {
            std::stringstream tmp;
            for (auto iter : args)
            {
                tmp << iter << " ";
            }
            ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
        }
        return redis_err_code::disconnect;
    }
}

void redis_module_impl::asyn_ret_nil_vec_command(
    const std::string& command, const std::string& key, redis_nil_func func, const std::vector<std::string>& args) {
    auto c = redis_manager::instance()->get_conn();
    if (c == nullptr)
    {
        func(redis_err_code::disconnect);
        return;
    }
    auto conn = c->get_redis_conn();
    if (conn == nullptr)
    {
        func(redis_err_code::disconnect);
        return;
    }

    auto io = c->get_io();

    redis_buffer rx_buff, tx_buff;

    conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()),
        [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->release_conn(c);
                func(redis_err_code::disconnect);
                if (!key.empty())
                {
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                }
                else
                {
                    std::stringstream tmp;
                    for (auto iter : args)
                    {
                        tmp << iter << " ";
                    }
                    ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                }
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->release_conn(c);
                    func(redis_err_code::disconnect);
                    if (!key.empty())
                    {
                        ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    }
                    else
                    {
                        std::stringstream tmp;
                        for (auto iter : args)
                        {
                            tmp << iter << " ";
                        }
                        ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                    }
                    return;
                }
                redis_manager::instance()->release_conn(c);
                rx_buff.consume(r.consumed);
                auto err_code = is_ok(r.result);
                func(err_code);
                return;
            });
        });
    io.get().poll();
}

void redis_module_impl::asyn_ret_int_vec_command(
    const std::string& command, const std::string& key, redis_int_func func, const std::vector<std::string>& args) {
    auto c = redis_manager::instance()->get_conn();
    if (c == nullptr)
    {
        func(redis_err_code::disconnect, -1);
        return;
    }
    auto conn = c->get_redis_conn();
    if (conn == nullptr)
    {
        func(redis_err_code::disconnect, -1);
        return;
    }

    auto io = c->get_io();

    redis_buffer rx_buff, tx_buff;

    conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()),
        [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->release_conn(c);
                func(redis_err_code::disconnect, -1);
                if (!key.empty())
                {
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                }
                else
                {
                    std::stringstream tmp;
                    for (auto iter : args)
                    {
                        tmp << iter << " ";
                    }
                    ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                }
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->release_conn(c);
                    func(redis_err_code::disconnect, -1);
                    if (!key.empty())
                    {
                        ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    }
                    else
                    {
                        std::stringstream tmp;
                        for (auto iter : args)
                        {
                            tmp << iter << " ";
                        }
                        ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                    }
                    return;
                }
                redis_manager::instance()->release_conn(c);
                rx_buff.consume(r.consumed);
                auto err_code = is_ok(r.result);
                if (err_code != redis_err_code::ok)
                {
                    func(err_code, -1);
                    return;
                }
                auto extract = boost::apply_visitor(bredis::extractor<iterator>(), r.result);
                auto result_value = boost::get<bredis::extracts::int_t>(extract);
                func(err_code, result_value);
                return;
            });
        });
    io.get().poll();
}

void redis_module_impl::asyn_ret_str_vec_command(
    const std::string& command, const std::string& key, redis_str_func func, const std::vector<std::string>& args) {
    auto c = redis_manager::instance()->get_conn();
    if (c == nullptr)
    {
        func(redis_err_code::disconnect, std::string());
        return;
    }
    auto conn = c->get_redis_conn();
    if (conn == nullptr)
    {
        func(redis_err_code::disconnect, std::string());
        return;
    }

    auto io = c->get_io();

    redis_buffer rx_buff, tx_buff;
    conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()),
        [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->release_conn(c);
                func(redis_err_code::disconnect, std::string());
                if (!key.empty())
                {
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                }
                else
                {
                    std::stringstream tmp;
                    for (auto iter : args)
                    {
                        tmp << iter << " ";
                    }
                    ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                }
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->release_conn(c);
                    func(redis_err_code::disconnect, std::string());
                    if (!key.empty())
                    {
                        ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    }
                    else
                    {
                        std::stringstream tmp;
                        for (auto iter : args)
                        {
                            tmp << iter << " ";
                        }
                        ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                    }
                    return;
                }
                redis_manager::instance()->release_conn(c);
                rx_buff.consume(r.consumed);
                auto err_code = is_ok(r.result);
                if (err_code != redis_err_code::ok)
                {
                    func(err_code, std::string());
                    return;
                }
                auto extract = boost::apply_visitor(bredis::extractor<iterator>(), r.result);
                auto result_value = boost::get<bredis::extracts::string_t>(extract).str;
                func(err_code, result_value);
                return;
            });
        });
    io.get().poll();
}

void redis_module_impl::asyn_ret_array_vec_command(
    const std::string& command, const std::string& key, redis_array_func func, const std::vector<std::string>& args) {
    std::vector<std::string> result_vector;
    auto c = redis_manager::instance()->get_conn();
    if (c == nullptr)
    {
        func(redis_err_code::disconnect, result_vector);
        return;
    }
    auto conn = c->get_redis_conn();
    if (conn == nullptr)
    {
        func(redis_err_code::disconnect, result_vector);
        return;
    }

    auto io = c->get_io();

    redis_buffer rx_buff, tx_buff;

    conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()),
        [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->release_conn(c);
                func(redis_err_code::disconnect, result_vector);
                if (!key.empty())
                {
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                }
                else
                {
                    std::stringstream tmp;
                    for (auto iter : args)
                    {
                        tmp << iter << " ";
                    }
                    ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                }

                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->release_conn(c);
                    func(redis_err_code::disconnect, result_vector);
                    if (!key.empty())
                    {
                        ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    }
                    else
                    {
                        std::stringstream tmp;
                        for (auto iter : args)
                        {
                            tmp << iter << " ";
                        }
                        ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                    }
                    return;
                }
                redis_manager::instance()->release_conn(c);
                rx_buff.consume(r.consumed);
                auto err_code = is_ok(r.result);
                if (err_code != redis_err_code::ok)
                {
                    func(err_code, result_vector);
                    return;
                }

                auto extract = boost::apply_visitor(bredis::extractor<iterator>(), r.result);
                auto& v = boost::get<bredis::extracts::array_holder_t>(extract).elements;
                for (auto iter : v)
                {
                    result_vector.emplace_back(boost::get<bredis::extracts::string_t>(iter).str);
                }
                func(err_code, result_vector);
                return;
            });
        });
    io.get().poll();
}

} // namespace ark
