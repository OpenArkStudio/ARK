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

#include "ark/container/vector.hpp"
#include "ark/module.hpp"

namespace ark {

enum class redis_err_code
{
    ok = 0,
    err = 1,
    miss = 2,
    disconnect = 3
};
typedef void (*redis_int_func)(const redis_err_code&, const int64_t&);
typedef void (*redis_nil_func)(const redis_err_code&);
typedef void (*redis_str_func)(const redis_err_code&, const std::string&);
typedef void (*redis_array_func)(const redis_err_code&, const std::vector<std::string>&);

class redis_module : public module
{
public:
    // Synchronous commands
    virtual redis_err_code get(const std::string& key, OUT std::string& value) = 0;
    virtual redis_err_code set(const std::string& key, const std::string& value, const uint64_t expire_time = 0) = 0;
    virtual redis_err_code incr(const std::string& key, OUT int64_t& value) = 0;
    virtual redis_err_code h_set(const std::string& key, const std::string& field, const std::string& value) = 0;
    virtual redis_err_code h_get(const std::string& key, const std::string& field, OUT std::string& value) = 0;
    virtual redis_err_code set_nx(const std::string& key, const std::string& value) = 0;
    virtual redis_err_code get_set(const std::string& key, const std::string& value, OUT std::string& str) = 0;
    virtual redis_err_code l_len(const std::string& key, OUT int64_t& length) = 0;
    virtual redis_err_code l_range(
        const std::string& key, const int64_t& begin, const int64_t& end, OUT value_vector<std::string>& res_list) = 0;
    virtual redis_err_code l_pop(const std::string& key, OUT std::string& value) = 0;
    virtual redis_err_code r_pop(const std::string& key, OUT std::string& value) = 0;
    virtual redis_err_code s_card(const std::string& key, OUT int64_t& value) = 0;
    virtual redis_err_code s_members(const std::string& key, OUT value_vector<std::string>& res_liste) = 0;
    virtual redis_err_code s_rand_member(
        const std::string& key, const uint32_t& count, OUT value_vector<std::string>& res_list) = 0;
    virtual redis_err_code s_pop(
        const std::string& key, const uint32_t& count, OUT value_vector<std::string>& res_list) = 0; // For redis 3.2+
    virtual redis_err_code s_pop(const std::string& key, OUT std::string& value) = 0;
    virtual redis_err_code s_is_member(const std::string& key, const std::string& member, OUT bool& res) = 0;
    virtual redis_err_code z_incr_by(
        const std::string& key, const double& increment, const std::string& member, OUT double& result_value) = 0;
    virtual redis_err_code z_range(const std::string& key, const int64_t& begin, const int64_t& end,
        OUT value_vector<std::string>& res_list, bool with_score = false) = 0;
    virtual redis_err_code z_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        OUT value_vector<std::string>& res_list, bool with_score = false) = 0;
    virtual redis_err_code z_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, OUT value_vector<std::string>& res_list,
        bool with_score = false) = 0; //with limit
    virtual redis_err_code z_rev_range(const std::string& key, const std::int64_t& begin, const int64_t& end,
        OUT value_vector<std::string>& res_list, bool with_score = false) = 0;
    virtual redis_err_code z_rev_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        OUT value_vector<std::string>& res_list, bool with_score = false) = 0;
    virtual redis_err_code z_rev_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, OUT value_vector<std::string>& res_list,
        bool with_score = false) = 0; // with limit
    virtual redis_err_code z_rank(const std::string& key, const std::string& member, OUT int64_t& rank_res) = 0;
    virtual redis_err_code z_rev_rank(const std::string& key, const std::string& member, OUT int64_t& rank_res) = 0;
    virtual redis_err_code z_card(const std::string& key, OUT int64_t& count) = 0;
    virtual redis_err_code z_count(
        const std::string& key, const std::string& min, const std::string& max, OUT int64_t& count) = 0;
    virtual redis_err_code z_score(const std::string& key, const std::string member, OUT double& score) = 0;
    virtual redis_err_code z_rem_range_by_rank(
        const std::string& key, const int64_t& begin, const int64_t& end, OUT int64_t& count) = 0;
    virtual redis_err_code z_rem_range_by_score(
        const std::string& key, const std::string& min, const std::string& max, OUT int64_t& count) = 0;
    virtual redis_err_code h_get_all(const std::string& key, OUT value_vector<std::string>& res_list) = 0;
    virtual redis_err_code h_len(const std::string& key, OUT int64_t& length) = 0;
    virtual redis_err_code h_set_nx(const std::string& key, const std::string& field, const std::string& value) = 0;
    virtual redis_err_code h_exists(const std::string& key, const std::string& field, OUT bool& exist) = 0;
    virtual redis_err_code h_keys(const std::string& key, OUT value_vector<std::string>& fields) = 0;
    virtual redis_err_code h_del(const std::string& key, const std::string& field, OUT bool& is_deleted) = 0;
    virtual redis_err_code h_incr_by(
        const std::string& key, const std::string& field, const int64_t& increment, OUT int64_t& result_valuee) = 0;
    virtual redis_err_code h_incr_by_float(
        const std::string& key, const std::string& field, const double& increment, double& result_value) = 0;
    virtual redis_err_code h_scan(const std::string& key, OUT int64_t& ret_cursor,
        OUT value_vector<std::string>& res_list, const int64_t& cursor, const std::string& pattern,
        const uint32_t& count = 10) = 0;
    virtual redis_err_code exists(const std::string& key, OUT bool& exist) = 0;
    virtual redis_err_code del(const std::string& key) = 0;
    virtual redis_err_code incr_by(const std::string& key, const int64_t& increment, OUT int64_t& result) = 0;
    virtual redis_err_code decr_by(const std::string& key, const int64_t& decrement, OUT int64_t& result) = 0;
    virtual redis_err_code expire(const std::string& key, const int64_t& expire_time, OUT bool& result) = 0;

    template<typename... args_type>
    redis_err_code m_set_args(const args_type... args) {
        std::vector<std::string> params{"MSET", std::forward<std::string>(convert<args_type>(args))...};
        return m_set(params, "MSET", {});
    }

    virtual redis_err_code m_set(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key) = 0;

    template<typename... args_type>
    redis_err_code m_get_args(value_vector<std::string>& str_list, const args_type&... args) {
        std::vector<std::string> params{"MGET", std::forward<std::string>(convert<args_type>(args))...};
        return m_get(params, "MGET", {}, str_list);
    }

    virtual redis_err_code m_get(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, value_vector<std::string>& str_list) = 0;

    template<typename... args_type>
    redis_err_code l_push_args(const std::string& key, int64_t& length, const args_type&... args) {
        std::vector<std::string> params{"LPUSH", key, std::forward<std::string>(convert<args_type>(args))...};
        return l_push(params, "LPUSH", key, length);
    }

    virtual redis_err_code l_push(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    template<typename... args_type>
    redis_err_code r_push_args(const std::string& key, int64_t& length, const args_type&... args) {
        std::vector<std::string> params{"RPUSH", key, std::forward<std::string>(convert<args_type>(args))...};
        return r_push(params, "RPUSH", key, length);
    }

    virtual redis_err_code r_push(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    template<typename... args_type>
    redis_err_code s_rem_args(const std::string& key, int64_t& length, const args_type&... args) {
        std::vector<std::string> params{"SREM", key, std::forward<std::string>(convert<args_type>(args))...};
        return s_rem(params, "SREM", key, length);
    }

    virtual redis_err_code s_rem(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    template<typename... args_type>
    redis_err_code s_add_args(const std::string& key, int64_t& length, const args_type&... args) {
        std::vector<std::string> params{"SADD", key, std::forward<std::string>(convert<args_type>(args))...};
        return s_add(params, "SADD", key, length);
    }

    virtual redis_err_code s_add(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    template<typename... args_type>
    redis_err_code z_add_args(const std::string& key, int64_t& length, const args_type&... args) {
        std::vector<std::string> params{"ZADD", key, std::forward<std::string>(convert<args_type>(args))...};
        return z_add(params, "ZADD", key, length);
    }

    virtual redis_err_code z_add(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    template<typename... args_type>
    redis_err_code z_rem_args(
        const std::string& key, int64_t& length, const args_type&... args) // if Redis < 2.4, args number can only be 1
    {
        std::vector<std::string> params{"ZREM", key, std::forward<std::string>(convert<args_type>(args))...};
        return z_rem(params, "ZREM", key, length);
    }

    virtual redis_err_code z_rem(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    //Asynchronous commands
    virtual void get(const std::string& key, redis_str_func func) = 0;
    virtual void set(
        const std::string& key, const std::string& value, redis_nil_func func, const uint64_t expire_time = 0) = 0;
    virtual void incr(const std::string& key, redis_int_func func) = 0;
    virtual void h_set(
        const std::string& key, const std::string& field, const std::string& value, redis_nil_func func) = 0;
    virtual void h_get(const std::string& key, const std::string& field, redis_str_func func) = 0;
    virtual void set_nx(const std::string& key, const std::string& value, redis_nil_func func) = 0;
    virtual void get_set(const std::string& key, const std::string& value, redis_str_func func) = 0;
    virtual void l_len(const std::string& key, redis_int_func func) = 0;
    virtual void l_range(const std::string& key, const int64_t& begin, const int64_t& end, redis_array_func func) = 0;
    virtual void l_pop(const std::string& key, redis_str_func func) = 0;
    virtual void r_pop(const std::string& key, redis_str_func func) = 0;
    virtual void s_card(const std::string& key, redis_int_func func) = 0;
    virtual void s_members(const std::string& key, redis_array_func func) = 0;
    virtual void s_rand_member(const std::string& key, const uint32_t& count, redis_array_func func) = 0;
    virtual void s_pop(const std::string& key, const uint32_t& count, redis_array_func func) = 0; // For redis 3.2+
    virtual void s_pop(const std::string& key, redis_str_func func) = 0;
    virtual void s_is_member(const std::string& key, const std::string& member, redis_int_func func) = 0;
    virtual void z_incr_by(
        const std::string& key, const double& increment, const std::string& member, redis_str_func func) = 0;
    virtual void z_range(const std::string& key, const int64_t& begin, const int64_t& end, redis_array_func func,
        bool with_score = false) = 0;
    virtual void z_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        redis_array_func func, bool with_score = false) = 0;
    virtual void z_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, redis_array_func func,
        bool with_score = false) = 0; //with limit
    virtual void z_rev_range(const std::string& key, const std::int64_t& begin, const int64_t& end,
        redis_array_func func, bool with_score = false) = 0;
    virtual void z_rev_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        redis_array_func func, bool with_score = false) = 0;
    virtual void z_rev_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, redis_array_func func,
        bool with_score = false) = 0; // with limit
    virtual void z_rank(const std::string& key, const std::string& member, redis_int_func func) = 0;
    virtual void z_rev_rank(const std::string& key, const std::string& member, redis_int_func func) = 0;
    virtual void z_card(const std::string& key, redis_int_func func) = 0;
    virtual void z_count(
        const std::string& key, const std::string& min, const std::string& max, redis_int_func func) = 0;
    virtual void z_score(const std::string& key, const std::string member, redis_str_func func) = 0;
    virtual void z_rem_range_by_rank(
        const std::string& key, const int64_t& begin, const int64_t& end, redis_int_func func) = 0;
    virtual void z_rem_range_by_score(
        const std::string& key, const std::string& min, const std::string& max, redis_int_func func) = 0;
    virtual void h_get_all(const std::string& key, redis_array_func func) = 0;
    virtual void h_len(const std::string& key, redis_int_func func) = 0;
    virtual void h_set_nx(
        const std::string& key, const std::string& field, const std::string& value, redis_nil_func func) = 0;
    virtual void h_exists(const std::string& key, const std::string& field, redis_int_func func) = 0;
    virtual void h_keys(const std::string& key, redis_array_func func) = 0;
    virtual void h_del(const std::string& key, const std::string& field, redis_int_func func) = 0;
    virtual void h_incr_by(
        const std::string& key, const std::string& field, const int64_t& increment, redis_int_func func) = 0;
    virtual void h_incr_by_float(
        const std::string& key, const std::string& field, const double& increment, redis_str_func func) = 0;
    virtual void h_scan(const std::string& key,
        void (*hscan_func)(const redis_err_code&, const int64_t&, const std::vector<std::string>&),
        const int64_t& cursor, const std::string& pattern, const uint32_t& count = 10) = 0;
    virtual void exists(const std::string& key, redis_int_func func) = 0;
    virtual void del(const std::string& key, redis_nil_func func) = 0;
    virtual void incr_by(const std::string& key, const int64_t& increment, redis_int_func func) = 0;
    virtual void decr_by(const std::string& key, const int64_t& decrement, redis_int_func func) = 0;
    virtual void expire(const std::string& key, const int64_t& expire_time, redis_int_func func) = 0;

    template<typename... args_type, typename handler_type>
    void asyn_m_set_args(handler_type func, const args_type&... args) {
        std::vector<std::string> params{"MSET", std::forward<std::string>(convert<args_type>(args))...};
        asyn_m_set(params, "MSET", {}, func);
    }

    virtual void asyn_m_set(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, redis_nil_func func) = 0;

    template<typename... args_type, typename handler_type>
    void asyn_m_get_args(handler_type func, const args_type&... args) {
        std::vector<std::string> params{"MGET", std::forward<std::string>(convert<args_type>(args))...};
        asyn_m_get(params, "MGET", {}, func);
    }

    virtual void asyn_m_get(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, redis_array_func func) = 0;

    template<typename... args_type, typename handler_type>
    void asyn_l_push_args(const std::string& key, handler_type func, const args_type&... args) {
        std::vector<std::string> params{"LPUSH", key, std::forward<std::string>(convert<args_type>(args))...};
        asyn_l_push(params, "LPUSH", key, func);
    }

    virtual void asyn_l_push(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, redis_int_func func) = 0;

    template<typename... args_type, typename handler_type>
    void asyn_r_push_args(const std::string& key, handler_type func, const args_type&... args) {
        std::vector<std::string> params{"RPUSH", key, std::forward<std::string>(convert<args_type>(args))...};
        asyn_r_push(params, "RPUSH", key, func);
    }

    virtual void asyn_r_push(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, redis_int_func func) = 0;

    template<typename... args_type, typename handler_type>
    void asyn_s_rem_args(const std::string& key, handler_type func, const args_type&... args) {
        std::vector<std::string> params{"SREM", key, std::forward<std::string>(convert<args_type>(args))...};
        asyn_s_rem(params, "SREM", key, func);
    }

    virtual void asyn_s_rem(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, redis_int_func func) = 0;

    template<typename... args_type, typename handler_type>
    void asyn_s_add_args(const std::string& key, handler_type func, const args_type&... args) {
        std::vector<std::string> params{"SADD", key, std::forward<std::string>(convert<args_type>(args))...};
        asyn_s_add(params, "SADD", key, func);
    }

    virtual void asyn_s_add(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, redis_int_func func) = 0;

    template<typename... args_type, typename handler_type>
    void asyn_z_add_args(const std::string& key, handler_type func, const args_type&... args) {
        std::vector<std::string> params{"ZADD", key, std::forward<std::string>(convert<args_type>(args))...};
        asyn_z_add(params, "ZADD", key, func);
    }

    virtual void asyn_z_add(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, redis_int_func func) = 0;

    template<typename... args_type, typename handler_type>
    void asyn_z_rem_args(const std::string& key, handler_type func,
        const args_type&... args) // if Redis < 2.4, args number can only be 1
    {
        std::vector<std::string> params{"ZREM", key, std::forward<std::string>(convert<args_type>(args))...};
        asyn_z_rem(params, "ZREM", key, func);
    }

    virtual void asyn_z_rem(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, redis_int_func func) = 0;

    template<typename value_type>
    using true_type = std::enable_if_t<std::is_same_v<std::decay_t<value_type>, std::string>, const value_type&>;

    template<typename value_type>
    using false_type = std::enable_if_t<!std::is_same_v<std::decay_t<value_type>, std::string>, const value_type>;

    template<typename value_type>
    const std::string& convert(true_type<value_type> arg) {
        return arg;
    }

    template<typename value_type>
    std::string convert(
        std::enable_if_t<std::is_same_v<std::decay_t<false_type<value_type>>, const char*>, const char*> arg) {
        return std::string(arg);
    }

    template<typename value_type>
    std::string convert(std::enable_if_t<!std::is_same_v<std::decay_t<false_type<value_type>>, const char*>,
        const std::decay_t<false_type<value_type>>>
            arg) {
        return std::to_string(arg);
    }

    virtual bool connect() = 0;
    virtual bool reconnect() = 0;
    virtual bool disconnect() = 0;
};

} // namespace ark