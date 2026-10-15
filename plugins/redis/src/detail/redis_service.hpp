// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <chrono>
#include <exception>
#include <functional>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <arknet/base/detail/deadline.hpp>
#include <arknet/redis/pool.hpp>

#include "detail/database_completion_queue.hpp"

namespace ark::detail {

// Private integration envelope; the public module API is not exposed here.
using redis_service_result = std::variant<arknet::redis::result, std::exception_ptr>;

inline arknet::redis::result redis_acquire_failure(arknet::redis::pool::acquire_result acquired) {
    const auto error =
        acquired.error == arknet::redis::client_errc::none ? arknet::redis::client_errc::not_connected : acquired.error;
    return {error, acquired.transport_error, std::move(acquired.replies), false};
}

class redis_service
{
public:
    using callback = std::function<void(redis_service_result)>;

    redis_service(
        arknet::redis::pool_options options, std::size_t max_operations = 64, std::size_t max_input_bytes = 1024 * 1024)
        : completions_(std::make_shared<completion_queue<redis_service_result>>(max_operations, max_input_bytes))
        , pool_(std::move(options)) {
    }

    ~redis_service() {
        // The service is created, updated and destroyed by its module thread.
        stop();
    }

    redis_service(const redis_service&) = delete;
    redis_service& operator=(const redis_service&) = delete;

    // false means not accepted: no callback. Accepted callbacks run in update()
    // unless stop() suppresses them. timeout covers both acquire and execution.
    bool submit(arknet::redis::command command, callback completion, std::chrono::milliseconds timeout) {
        std::vector<arknet::redis::command> commands;
        commands.push_back(std::move(command));
        return submit_pipeline(std::move(commands), std::move(completion), timeout);
    }

    bool submit_pipeline(
        std::vector<arknet::redis::command> commands, callback completion, std::chrono::milliseconds timeout) {
        const auto deadline = arknet::detail::checked_deadline(std::chrono::steady_clock::now(), timeout);
        if (!deadline)
        {
            throw std::invalid_argument("Database operation timeout is not representable");
        }
        std::size_t bytes{};
        for (const auto& command : commands)
        {
            if (command.capacity() > (std::numeric_limits<std::size_t>::max() - bytes) / sizeof(std::string))
            {
                return false;
            }
            bytes += command.capacity() * sizeof(std::string);
            for (const auto& argument : command)
            {
                if (argument.capacity() > std::numeric_limits<std::size_t>::max() - bytes)
                {
                    return false;
                }
                bytes += argument.capacity();
            }
        }
        if (commands.capacity() > (std::numeric_limits<std::size_t>::max() - bytes) / sizeof(arknet::redis::command))
        {
            return false;
        }
        bytes += commands.capacity() * sizeof(arknet::redis::command);
        auto ticket = completions_->admit(bytes, std::move(completion));
        if (!ticket)
        {
            return false;
        }
        try
        {
            const auto acquire_remaining =
                std::chrono::duration_cast<std::chrono::milliseconds>(*deadline - std::chrono::steady_clock::now());
            if (acquire_remaining <= std::chrono::milliseconds::zero())
            {
                completions_->complete(
                    ticket, arknet::redis::result{arknet::redis::client_errc::timeout, {}, {}, false});
                return true;
            }
            pool_.async_acquire(
                [queue = completions_, ticket, commands = std::move(commands), deadline = *deadline](
                    arknet::redis::pool::acquire_result acquired) mutable {
                    if (!acquired)
                    {
                        queue->complete(ticket, redis_acquire_failure(std::move(acquired)));
                        return;
                    }
                    auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
                        deadline - std::chrono::steady_clock::now());
                    if (remaining <= std::chrono::milliseconds::zero())
                    {
                        acquired.connection.release();
                        queue->complete(
                            ticket, arknet::redis::result{arknet::redis::client_errc::timeout, {}, {}, false});
                        return;
                    }
                    try
                    {
                        // Returning the façade after submit is safe: the lease
                        // defers return/reset until its submitted callback exits.
                        acquired.connection.async_pipeline(
                            std::move(commands),
                            [queue, ticket](
                                arknet::redis::result result) mutable { queue->complete(ticket, std::move(result)); },
                            remaining);
                    }
                    catch (...)
                    { queue->complete(ticket, std::current_exception()); }
                },
                acquire_remaining);
        }
        catch (...)
        {
            completions_->abandon(ticket);
            throw;
        }
        return true;
    }

    std::size_t update(std::size_t maximum = 64) {
        // A business callback may destroy its service while dispatch is active.
        auto completions = completions_;
        return completions->dispatch(maximum);
    }

    void stop() {
        auto completions = completions_;
        pool_.close();
        // Releasing business captures may destroy this service. Keep the queue
        // alive and do not access service members after callbacks are removed.
        completions->stop();
    }

private:
    std::shared_ptr<completion_queue<redis_service_result>> completions_;
    arknet::redis::pool pool_;
};

} // namespace ark::detail
