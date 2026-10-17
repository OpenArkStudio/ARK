// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "detail/database_awaitable.hpp"
#include "mysql/detail/mysql_transaction.hpp"

namespace ark::detail {

// Private factories own inputs immediately; the returned Asio tasks are lazy.
// Keep service/transaction and bridge alive and unmoved until task completion
// or destruction of an unstarted task. A step retains the transaction deadline.
// Preserve the complete reply, including initiation and cleanup diagnostics.
template<class service_type>
module_task<mysql_transaction_reply> mysql_transaction_begin(
    service_type& service, database_awaitable<mysql_transaction_reply>& bridge, mysql_transaction_options options) {
    return bridge.wait(
        [&service, options](auto completion) {
            auto callback = [completion = std::move(completion)](
                                mysql_transaction_reply reply) mutable { completion({}, std::move(reply)); };
            return service.start(options, std::move(callback));
        },
        [&service](std::uint64_t id) { service.cancel(id); });
}

template<class transaction_type>
module_task<mysql_transaction_reply> mysql_transaction_query(
    transaction_type& transaction, database_awaitable<mysql_transaction_reply>& bridge, std::string sql) {
    return bridge.wait(
        [&transaction, sql = std::move(sql)](auto completion) mutable {
            auto callback = [completion = std::move(completion)](
                                mysql_transaction_reply reply) mutable { completion({}, std::move(reply)); };
            return transaction.start_query(std::move(sql), std::move(callback));
        },
        [&transaction](std::uint64_t id) { transaction.cancel(id); });
}

template<class transaction_type>
module_task<mysql_transaction_reply> mysql_transaction_commit(
    transaction_type& transaction, database_awaitable<mysql_transaction_reply>& bridge) {
    return bridge.wait(
        [&transaction](auto completion) {
            auto callback = [completion = std::move(completion)](
                                mysql_transaction_reply reply) mutable { completion({}, std::move(reply)); };
            return transaction.start_commit(std::move(callback));
        },
        [&transaction](std::uint64_t id) { transaction.cancel(id); });
}

template<class transaction_type>
module_task<mysql_transaction_reply> mysql_transaction_rollback(
    transaction_type& transaction, database_awaitable<mysql_transaction_reply>& bridge) {
    return bridge.wait(
        [&transaction](auto completion) {
            auto callback = [completion = std::move(completion)](
                                mysql_transaction_reply reply) mutable { completion({}, std::move(reply)); };
            return transaction.start_rollback(std::move(callback));
        },
        [&transaction](std::uint64_t id) { transaction.cancel(id); });
}

} // namespace ark::detail
