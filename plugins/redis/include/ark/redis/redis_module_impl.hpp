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

#include "ark/plugin_manager.hpp"
#include "ark/redis/redis_manager.hpp"
#include "ark/redis/redis_module.hpp"

namespace ark {

class is_corr : public boost::static_visitor<redis_err_code>
{
public:
    redis_err_code operator()(const bredis::markers::int_t<iterator>& i) const {
        return redis_err_code::ok;
    }

    redis_err_code operator()(const bredis::markers::string_t<iterator>& i) const {
        return redis_err_code::ok;
    }

    redis_err_code operator()(const bredis::markers::array_wrapper_t<iterator>& i) const {
        return redis_err_code::ok;
    }

    redis_err_code operator()(const bredis::markers::nil_t<iterator>& i) const {
        return redis_err_code::miss;
    }

    redis_err_code operator()(const bredis::markers::error_t<iterator>& i) const {
        return redis_err_code::err;
    }
};

class redis_module_impl final : public redis_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init();

    // Synchronous commands
    redis_err_code get(const std::string& key, std::string& value);
    redis_err_code set(const std::string& key, const std::string& value, const uint64_t expire_time = 0);
    redis_err_code incr(std::string const& key, int64_t& value);
    redis_err_code h_get(const std::string& key, const std::string& field, std::string& value);
    redis_err_code h_set(const std::string& key, const std::string& field, const std::string& value);
    redis_err_code set_nx(const std::string& key, const std::string& value);
    redis_err_code get_set(const std::string& key, const std::string& value, std::string& str);
    redis_err_code l_len(const std::string& key, OUT int64_t& length);
    redis_err_code l_range(
        const std::string& key, const int64_t& begin, const int64_t& end, OUT value_vector<std::string>& res_list);
    redis_err_code l_pop(const std::string& key, OUT std::string& value);
    redis_err_code r_pop(const std::string& key, OUT std::string& value);
    redis_err_code s_card(const std::string& key, OUT int64_t& value);
    redis_err_code s_members(const std::string& key, OUT value_vector<std::string>& res_list);
    redis_err_code s_rand_member(
        const std::string& key, const uint32_t& count, OUT value_vector<std::string>& res_list);
    redis_err_code s_pop(
        const std::string& key, const uint32_t& count, OUT value_vector<std::string>& res_list); // For redis 3.2+
    redis_err_code s_pop(const std::string& key, OUT std::string& value);
    redis_err_code s_is_member(const std::string& key, const std::string& member, bool& res);
    redis_err_code z_incr_by(
        const std::string& key, const double& increment, const std::string& member, OUT double& result_value);
    redis_err_code z_range(const std::string& key, const int64_t& begin, const int64_t& end,
        OUT value_vector<std::string>& res_list, bool with_score = false);
    redis_err_code z_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        OUT value_vector<std::string>& res_list, bool with_score = false);
    redis_err_code z_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, OUT value_vector<std::string>& res_list,
        bool with_score = false); //with limit
    redis_err_code z_rev_range(const std::string& key, const std::int64_t& begin, const int64_t& end,
        OUT value_vector<std::string>& res_list, bool with_score = false);
    redis_err_code z_rev_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        OUT value_vector<std::string>& res_list, bool with_score = false);
    redis_err_code z_rev_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, OUT value_vector<std::string>& res_list,
        bool with_score = false); // with limit
    redis_err_code z_rank(const std::string& key, const std::string& member, OUT int64_t& rank_res);
    redis_err_code z_rev_rank(const std::string& key, const std::string& member, OUT int64_t& rank_res);
    redis_err_code z_card(const std::string& key, OUT int64_t& count);
    redis_err_code z_count(const std::string& key, const std::string& min, const std::string& max, OUT int64_t& count);
    redis_err_code z_score(const std::string& key, const std::string member, OUT double& score);
    redis_err_code z_rem_range_by_rank(
        const std::string& key, const int64_t& begin, const int64_t& end, OUT int64_t& count);
    redis_err_code z_rem_range_by_score(
        const std::string& key, const std::string& min, const std::string& max, OUT int64_t& count);
    redis_err_code h_get_all(const std::string& key, OUT value_vector<std::string>& res_list);
    redis_err_code h_len(const std::string& key, OUT int64_t& length);
    redis_err_code h_set_nx(const std::string& key, const std::string& field, const std::string& value);
    redis_err_code h_exists(const std::string& key, const std::string& field, OUT bool& exist);
    redis_err_code h_keys(const std::string& key, OUT value_vector<std::string>& fields);
    redis_err_code h_del(const std::string& key, const std::string& field, OUT bool& is_deleted);
    redis_err_code h_incr_by(
        const std::string& key, const std::string& field, const int64_t& increment, OUT int64_t& result_value);
    redis_err_code h_incr_by_float(
        const std::string& key, const std::string& field, const double& increment, OUT double& result_value);
    redis_err_code h_scan(const std::string& key, OUT int64_t& ret_cursor, OUT value_vector<std::string>& res_list,
        const int64_t& cursor, const std::string& pattern, const uint32_t& count = 10);
    redis_err_code exists(const std::string& key, OUT bool& exist);
    redis_err_code del(const std::string& key);
    redis_err_code incr_by(const std::string& key, const int64_t& increment, OUT int64_t& result);
    redis_err_code decr_by(const std::string& key, const int64_t& decrement, OUT int64_t& result);
    redis_err_code expire(const std::string& key, const int64_t& expire_time, OUT bool& result);

    redis_err_code m_set(const std::vector<std::string>& commands, const std::string& command, const std::string& key);
    redis_err_code m_get(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        value_vector<std::string>& str_list);
    redis_err_code l_push(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    redis_err_code r_push(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    redis_err_code s_rem(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    redis_err_code s_add(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    redis_err_code z_add(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    redis_err_code z_rem(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);

    //Asynchronous commands
    void get(const std::string& key, redis_str_func func);
    void set(const std::string& key, const std::string& value, redis_nil_func func, const uint64_t expire_time = 0);
    void incr(const std::string& key, redis_int_func func);
    void h_set(const std::string& key, const std::string& field, const std::string& value, redis_nil_func func);
    void h_get(const std::string& key, const std::string& field, redis_str_func func);
    void set_nx(const std::string& key, const std::string& value, redis_nil_func func);
    void get_set(const std::string& key, const std::string& value, redis_str_func func);
    void l_len(const std::string& key, redis_int_func func);
    void l_range(const std::string& key, const int64_t& begin, const int64_t& end, redis_array_func func);
    void l_pop(const std::string& key, redis_str_func func);
    void r_pop(const std::string& key, redis_str_func func);
    void s_card(const std::string& key, redis_int_func func);
    void s_members(const std::string& key, redis_array_func func);
    void s_rand_member(const std::string& key, const uint32_t& count, redis_array_func func);
    void s_pop(const std::string& key, const uint32_t& count, redis_array_func func); // For redis 3.2+
    void s_pop(const std::string& key, redis_str_func func);
    void s_is_member(const std::string& key, const std::string& member, redis_int_func func);
    void z_incr_by(const std::string& key, const double& increment, const std::string& member, redis_str_func func);
    void z_range(const std::string& key, const int64_t& begin, const int64_t& end, redis_array_func func,
        bool with_score = false);
    void z_range_by_score(const std::string& key, const std::string& min, const std::string& max, redis_array_func func,
        bool with_score = false);
    void z_range_by_score(const std::string& key, const std::string& min, const std::string& max, const int64_t& offset,
        const uint32_t& count, redis_array_func func,
        bool with_score = false); //with limit
    void z_rev_range(const std::string& key, const std::int64_t& begin, const int64_t& end, redis_array_func func,
        bool with_score = false);
    void z_rev_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        redis_array_func func, bool with_score = false);
    void z_rev_range_by_score(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, redis_array_func func,
        bool with_score = false); // with limit
    void z_rank(const std::string& key, const std::string& member, redis_int_func func);
    void z_rev_rank(const std::string& key, const std::string& member, redis_int_func func);
    void z_card(const std::string& key, redis_int_func func);
    void z_count(const std::string& key, const std::string& min, const std::string& max, redis_int_func func);
    void z_score(const std::string& key, const std::string member, redis_str_func func);
    void z_rem_range_by_rank(const std::string& key, const int64_t& begin, const int64_t& end, redis_int_func func);
    void z_rem_range_by_score(
        const std::string& key, const std::string& min, const std::string& max, redis_int_func func);
    void h_get_all(const std::string& key, redis_array_func func);
    void h_len(const std::string& key, redis_int_func func);
    void h_set_nx(const std::string& key, const std::string& field, const std::string& value, redis_nil_func func);
    void h_exists(const std::string& key, const std::string& field, redis_int_func func);
    void h_keys(const std::string& key, redis_array_func func);
    void h_del(const std::string& key, const std::string& field, redis_int_func func);
    void h_incr_by(const std::string& key, const std::string& field, const int64_t& increment, redis_int_func func);
    void h_incr_by_float(
        const std::string& key, const std::string& field, const double& increment, redis_str_func func);
    void h_scan(const std::string& key,
        void (*hscan_func)(const redis_err_code&, const int64_t&, const std::vector<std::string>&),
        const int64_t& cursor, const std::string& pattern, const uint32_t& count = 10);
    void exists(const std::string& key, redis_int_func func);
    void del(const std::string& key, redis_nil_func func);
    void incr_by(const std::string& key, const int64_t& increment, redis_int_func func);
    void decr_by(const std::string& key, const int64_t& decrement, redis_int_func func);
    void expire(const std::string& key, const int64_t& expire_time, redis_int_func func);

    void asyn_m_set(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        redis_nil_func func);
    void asyn_m_get(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        redis_array_func func);
    void asyn_l_push(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        redis_int_func func);
    void asyn_r_push(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        redis_int_func func);
    void asyn_s_rem(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        redis_int_func func);
    void asyn_s_add(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        redis_int_func func);
    void asyn_z_add(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        redis_int_func func);
    void asyn_z_rem(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        redis_int_func func);

    bool auth_pwd(const std::string& password, redis_conn* conn);
    bool connect();
    bool reconnect();
    bool disconnect();

private:
    redis_err_code is_ok(const bredis::markers::redis_result_t<iterator>& res) {
        return boost::apply_visitor(is_corr(), res);
    }

    template<typename... args_type>
    redis_err_code on_ret_nil_command(const std::string& command, const std::string& key, const args_type&... args) {
        auto ret = std::make_tuple(convert<args_type>(args)...);
        redis_err_code err_code;
        std::apply([this, &key, &command, &err_code](
                       auto&&... params) { err_code = ret_nil_command(command, key, params...); },
            ret);
        return err_code;
    }

    template<typename... args_type>
    redis_err_code ret_nil_command(const std::string& command, const std::string& key, const args_type&... args) {
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
            if (!key.empty())
            {
                conn->write(bredis::single_command_t(command, key, args...));
            }
            else
            {
                conn->write(bredis::single_command_t(command, args...));
            }

            auto result_markers = conn->read(rx_buff);
            redis_manager::instance()->release_conn(c);
            auto err_code = is_ok(result_markers.result);
            rx_buff.consume(result_markers.consumed);
            return err_code;
        }
        catch ([[maybe_unused]] boost::system::system_error& ec)
        {
            redis_manager::instance()->release_conn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return redis_err_code::disconnect;
        }
    }

    template<typename... args_type>
    redis_err_code on_ret_int_command(
        const std::string& command, const std::string& key, int64_t& value, const args_type&... args) {
        auto ret = std::make_tuple(convert<args_type>(args)...);
        redis_err_code err_code;
        std::apply([this, &key, &value, &command, &err_code](
                       auto&&... params) { err_code = ret_int_command(command, key, value, params...); },
            ret);
        return err_code;
    }

    template<typename... args_type>
    redis_err_code ret_int_command(
        const std::string& command, const std::string& key, int64_t& value, const args_type&... args) {
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
            if (!key.empty())
            {
                conn->write(bredis::single_command_t(command, key, args...));
            }
            else
            {
                conn->write(bredis::single_command_t(command, args...));
            }
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
            redis_manager::instance()->release_conn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return redis_err_code::disconnect;
        }
    }

    template<typename... args_type>
    redis_err_code on_ret_str_command(
        const std::string& command, const std::string& key, std::string& value, const args_type&... args) {
        auto ret = std::make_tuple(convert<args_type>(args)...);
        redis_err_code err_code;
        std::apply([this, &key, &value, &command, &err_code](
                       auto&&... params) { err_code = ret_str_command(command, key, value, params...); },
            ret);
        return err_code;
    }

    template<typename... args_type>
    redis_err_code ret_str_command(
        const std::string& command, const std::string& key, std::string& value, const args_type&... args) {
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
            if (!key.empty())
            {
                conn->write(bredis::single_command_t(command, key, args...));
            }
            else
            {
                conn->write(bredis::single_command_t(command, args...));
            }
            auto result_markers = conn->read(rx_buff);
            redis_manager::instance()->release_conn(c);
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
            redis_manager::instance()->release_conn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return redis_err_code::disconnect;
        }
    }

    template<typename... args_type>
    redis_err_code on_ret_array_command(
        const std::string& command, const std::string& key, std::vector<std::string>& value, const args_type&... args) {
        auto ret = std::make_tuple(convert<args_type>(args)...);
        redis_err_code err_code;
        std::apply([this, &key, &value, &command, &err_code](
                       auto&&... params) { err_code = ret_array_command(command, key, value, params...); },
            ret);
        return err_code;
    }

    template<typename... args_type>
    redis_err_code ret_array_command(
        const std::string& command, const std::string& key, std::vector<std::string>& value, const args_type&... args) {
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
            if (!key.empty())
            {
                conn->write(bredis::single_command_t(command, key, args...));
            }
            else
            {
                conn->write(bredis::single_command_t(command, args...));
            }
            auto result_markers = conn->read(rx_buff);
            redis_manager::instance()->release_conn(c);
            rx_buff.consume(result_markers.consumed);
            auto err_code = is_ok(result_markers.result);
            if (err_code != redis_err_code::ok)
            {
                return err_code;
            }
            auto extract = boost::apply_visitor(bredis::extractor<iterator>(), result_markers.result);

            auto& tmp_v = boost::get<bredis::extracts::array_holder_t>(extract).elements;
            value.clear();
            for (auto iter : tmp_v)
            {
                value.emplace_back(boost::get<bredis::extracts::string_t>(iter).str);
            }
            return err_code;
        }
        catch ([[maybe_unused]] boost::system::system_error& ec)
        {
            redis_manager::instance()->release_conn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return redis_err_code::disconnect;
        }
    }

    // For Asynchronous Command

    template<typename... args_type>
    void async_on_ret_nil_command(
        const std::string& command, const std::string& key, redis_nil_func func, const args_type&... args) {
        auto ret = std::make_tuple(convert<args_type>(args)...);
        std::apply(
            [this, &key, &func, &command](auto&&... params) { async_ret_nil_command(command, key, func, params...); },
            ret);
        return;
    }

    template<typename... args_type>
    void async_ret_nil_command(
        const std::string& command, const std::string& key, redis_nil_func func, const args_type&... args) {
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
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

        conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->release_conn(c);
                func(redis_err_code::disconnect);
                ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->release_conn(c);
                    func(redis_err_code::disconnect);
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    return;
                }
                redis_manager::instance()->release_conn(c);
                std::vector<std::string> result_vector;
                rx_buff.consume(r.consumed);
                auto err_code = is_ok(r.result);
                func(err_code);
            });
        });
        io.get().poll();
    }

    template<typename... args_type>
    void async_on_ret_int_command(
        const std::string& command, const std::string& key, redis_int_func func, const args_type&... args) {
        auto ret = std::make_tuple(convert<args_type>(args)...);
        std::apply(
            [this, &key, &func, &command](auto&&... params) { async_ret_int_command(command, key, func, params...); },
            ret);
        return;
    }

    template<typename... args_type>
    void async_ret_int_command(
        const std::string& command, const std::string& key, redis_int_func func, const args_type&... args) {
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
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

        conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->release_conn(c);
                func(redis_err_code::disconnect, -1);
                ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->release_conn(c);
                    func(redis_err_code::disconnect, -1);
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
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
            });
        });
        io.get().poll();
    }

    template<typename... args_type>
    void async_on_ret_str_command(
        const std::string& command, const std::string& key, redis_str_func func, const args_type&... args) {
        auto ret = std::make_tuple(convert<args_type>(args)...);
        std::apply(
            [this, &key, &func, &command](auto&&... params) { async_ret_str_command(command, key, func, params...); },
            ret);
        return;
    }

    template<typename... args_type>
    void async_ret_str_command(
        const std::string& command, const std::string& key, redis_str_func func, const args_type&... args) {
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
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

        conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->release_conn(c);
                func(redis_err_code::disconnect, std::string());
                ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->release_conn(c);
                    func(redis_err_code::disconnect, std::string());
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
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
            });
        });
        io.get().poll();
    }

    template<typename... args_type>
    void async_on_ret_array_command(
        const std::string& command, const std::string& key, redis_array_func func, const args_type&... args) {
        auto ret = std::make_tuple(convert<args_type>(args)...);
        std::apply(
            [this, &key, &func, &command](auto&&... params) { async_ret_array_command(command, key, func, params...); },
            ret);
        return;
    }

    template<typename... args_type>
    void async_ret_array_command(
        const std::string& command, const std::string& key, redis_array_func func, const args_type&... args) {
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
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

        conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->release_conn(c);
                func(redis_err_code::disconnect, result_vector);
                ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->release_conn(c);
                    func(redis_err_code::disconnect, result_vector);
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
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
            });
        });
        io.get().poll();
    }

    // Use vector instead of variadic template

    // Synchronous
    redis_err_code ret_nil_vec_command(
        const std::string& command, const std::string& key, const std::vector<std::string>& args);
    redis_err_code ret_int_vec_command(
        const std::string& command, const std::string& key, int64_t& value, const std::vector<std::string>& args);
    redis_err_code ret_str_vec_command(
        const std::string& command, const std::string& key, std::string& value, const std::vector<std::string>& args);
    redis_err_code ret_array_vec_command(const std::string& command, const std::string& key,
        std::vector<std::string>& value, const std::vector<std::string>& args);

    // Asynchronous
    void asyn_ret_nil_vec_command(
        const std::string& command, const std::string& key, redis_nil_func func, const std::vector<std::string>& args);
    void asyn_ret_int_vec_command(
        const std::string& command, const std::string& key, redis_int_func func, const std::vector<std::string>& args);
    void asyn_ret_str_vec_command(
        const std::string& command, const std::string& key, redis_str_func func, const std::vector<std::string>& args);
    void asyn_ret_array_vec_command(const std::string& command, const std::string& key, redis_array_func func,
        const std::vector<std::string>& args);

    bool load_config();
    std::string ip_;
    uint16_t port_;
    uint32_t pool_size_;
    std::string auth_keys_;
};
} // namespace ark