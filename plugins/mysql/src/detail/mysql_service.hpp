// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <chrono>
#include <exception>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <arknet/base/detail/deadline.hpp>
#include <arknet/mysql/pool.hpp>

#include "detail/database_completion_queue.hpp"

namespace ark::detail {

// Internal bridge; the published plugin API remains unchanged.
// Keep execution and statement cleanup outcomes separate:
// a failed close must not turn a successful write into a failed/replayed write.
struct mysql_service_reply
{
    arknet::mysql::query_result result;
    std::optional<arknet::mysql::connection_result> statement_close;
    std::exception_ptr statement_close_exception;
};

using mysql_service_result = std::variant<mysql_service_reply, std::exception_ptr>;

class mysql_service
{
    using queue_type = completion_queue<mysql_service_result>;

    struct operation : std::enable_shared_from_this<operation>
    {
        std::shared_ptr<queue_type> queue;
        queue_type::ticket ticket;
        std::chrono::steady_clock::time_point deadline;
        arknet::mysql::query_options limits;
        std::string sql;
        std::vector<arknet::mysql::field_value> parameters;
        arknet::mysql::pool::lease lease;
        arknet::mysql::prepared_statement statement;
        mysql_service_reply reply;
        bool prepared{};

        std::optional<arknet::mysql::query_options> remaining_options() const {
            const auto remaining =
                std::chrono::duration_cast<std::chrono::milliseconds>(deadline - std::chrono::steady_clock::now());
            if (remaining <= std::chrono::milliseconds::zero())
            {
                return std::nullopt;
            }
            auto options = limits;
            options.timeout = remaining;
            return options;
        }

        void finish() {
            lease.release();
            queue->complete(ticket, std::move(reply));
        }

        void fail(std::exception_ptr failure) {
            lease.discard();
            queue->complete(ticket, std::move(failure));
        }

        void close_statement() {
            const auto options = remaining_options();
            if (!options || !lease)
            {
                arknet::mysql::connection_result cleanup;
                cleanup.code =
                    options ? arknet::mysql::connection_errc::not_connected : arknet::mysql::connection_errc::timeout;
                reply.statement_close.emplace(std::move(cleanup));
                lease.discard();
                finish();
                return;
            }
            try
            {
                lease.async_close(
                    statement,
                    [self = shared_from_this()](arknet::mysql::connection_result result) mutable {
                        self->reply.statement_close.emplace(std::move(result));
                        if (!bool(*self->reply.statement_close))
                        {
                            self->lease.discard();
                        }
                        self->finish();
                    },
                    *options);
            }
            catch (...)
            {
                reply.statement_close_exception = std::current_exception();
                lease.discard();
                finish();
            }
        }

        void execute() {
            const auto options = remaining_options();
            if (!options)
            {
                reply.result.status.code = arknet::mysql::connection_errc::timeout;
                close_statement();
                return;
            }
            lease.async_execute(
                statement, std::move(parameters),
                [self = shared_from_this()](arknet::mysql::query_result result) mutable {
                    self->reply.result = std::move(result);
                    self->close_statement();
                },
                *options);
        }

        void acquired(arknet::mysql::pool::acquire_result result) {
            if (!result)
            {
                reply.result.status = std::move(result.status);
                // A successful acquisition status is insufficient if the
                // lease became unavailable before this callback used it.
                if (reply.result.status.code == arknet::mysql::connection_errc::none)
                {
                    reply.result.status.code = arknet::mysql::connection_errc::not_connected;
                }
                finish();
                return;
            }
            lease = std::move(result.connection);
            const auto options = remaining_options();
            if (!options)
            {
                reply.result.status.code = arknet::mysql::connection_errc::timeout;
                finish();
                return;
            }
            if (prepared)
            {
                lease.async_prepare(
                    std::move(sql),
                    [self = shared_from_this()](arknet::mysql::prepare_result result) mutable {
                        if (!result)
                        {
                            self->reply.result.status = std::move(result.status);
                            self->finish();
                            return;
                        }
                        self->statement = std::move(result.statement);
                        try
                        { self->execute(); }
                        catch (...)
                        { self->fail(std::current_exception()); }
                    },
                    *options);
            }
            else
            {
                lease.async_query(
                    std::move(sql),
                    [self = shared_from_this()](arknet::mysql::query_result result) mutable {
                        self->reply.result = std::move(result);
                        self->finish();
                    },
                    *options);
            }
        }
    };

public:
    using callback = std::function<void(mysql_service_result)>;

    mysql_service(arknet::mysql::pool_options options, std::size_t max_operations = 64,
        std::size_t max_input_bytes = 1024 * 1024, arknet::mysql::query_options limits = {})
        : completions_(std::make_shared<queue_type>(max_operations, max_input_bytes))
        , pool_(std::move(options))
        , limits_(limits) {
    }

    ~mysql_service() {
        stop();
    }

    mysql_service(const mysql_service&) = delete;
    mysql_service& operator=(const mysql_service&) = delete;

    // false means not admitted and no callback. One deadline includes acquiring
    // the lease and every command; no stage gets a fresh timeout or write retry.
    bool submit_query(std::string sql, callback completion, std::chrono::milliseconds timeout) {
        return submit(std::move(sql), {}, std::move(completion), timeout, false);
    }

    bool submit_execute(std::string sql, std::vector<arknet::mysql::field_value> parameters, callback completion,
        std::chrono::milliseconds timeout) {
        return submit(std::move(sql), std::move(parameters), std::move(completion), timeout, true);
    }

    std::size_t update(std::size_t maximum = 64) {
        // The business completion may destroy the service itself.
        auto queue = completions_;
        return queue->dispatch(maximum);
    }

    // Only suppresses business callbacks and requests native close. NOT drain,
    // a join barrier, or permission to unload a dynamic library.
    void stop() {
        // Destroying a business callback capture can destroy this service.
        // Pin the queue and access no members after dropping those captures.
        auto queue = completions_;
        pool_.close();
        queue->stop();
    }

private:
    bool submit(std::string sql, std::vector<arknet::mysql::field_value> parameters, callback completion,
        std::chrono::milliseconds timeout, bool prepared) {
        const auto deadline = arknet::detail::checked_deadline(std::chrono::steady_clock::now(), timeout);
        if (!deadline)
        {
            throw std::invalid_argument("Database operation timeout is not representable");
        }
        std::size_t bytes = sql.capacity();
        if (parameters.capacity() >
            (std::numeric_limits<std::size_t>::max() - bytes) / sizeof(arknet::mysql::field_value))
        {
            return false;
        }
        bytes += parameters.capacity() * sizeof(arknet::mysql::field_value);
        for (const auto& value : parameters)
        {
            if (const auto* text = std::get_if<std::string>(&value))
            {
                if (text->capacity() > std::numeric_limits<std::size_t>::max() - bytes)
                {
                    return false;
                }
                bytes += text->capacity();
            }
        }
        auto ticket = completions_->admit(bytes, std::move(completion));
        if (!ticket)
        {
            return false;
        }
        try
        {
            auto pending = std::make_shared<operation>();
            pending->queue = completions_;
            pending->ticket = ticket;
            pending->deadline = *deadline;
            pending->limits = limits_;
            pending->sql = std::move(sql);
            pending->parameters = std::move(parameters);
            pending->prepared = prepared;
            const auto options = pending->remaining_options();
            if (!options)
            {
                pending->reply.result.status.code = arknet::mysql::connection_errc::timeout;
                pending->finish();
                return true;
            }
            pool_.async_acquire(
                [pending](arknet::mysql::pool::acquire_result acquired) mutable {
                    try
                    { pending->acquired(std::move(acquired)); }
                    catch (...)
                    { pending->fail(std::current_exception()); }
                },
                options->timeout);
        }
        catch (...)
        {
            completions_->abandon(ticket);
            throw;
        }
        return true;
    }

    std::shared_ptr<queue_type> completions_;
    arknet::mysql::pool pool_;
    const arknet::mysql::query_options limits_;
};

} // namespace ark::detail
