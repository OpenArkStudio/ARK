// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <memory>

#include "ark/mysql/mysql_module.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

class mysql_module_impl final : public mysql_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    mysql_module_impl();
    ~mysql_module_impl() override;
    bool init() override;
    bool update() override;
    bool ready_to_shut() const override;
    bool pre_shut() override;
    bool shut() override;
    bool configure(std::string name, mysql_options options) override;
    task<mysql_reply> query(
        std::string name, std::string sql, std::chrono::milliseconds timeout = std::chrono::seconds(5)) override;
    task<mysql_reply> execute(std::string name, std::string sql, std::vector<arknet::mysql::field_value> parameters,
        std::chrono::milliseconds timeout = std::chrono::seconds(5)) override;
    task<mysql_transaction_reply> begin(std::string name, mysql_transaction_options options = {}) override;
    task<mysql_transaction_reply> query(mysql_transaction_id transaction, std::string sql) override;
    task<mysql_transaction_reply> execute(
        mysql_transaction_id transaction, std::string sql, std::vector<arknet::mysql::field_value> parameters) override;
    task<mysql_transaction_reply> commit(mysql_transaction_id transaction) override;
    task<mysql_transaction_reply> rollback(mysql_transaction_id transaction) override;

private:
    struct state;
    std::shared_ptr<state> state_;
};

} // namespace ark
