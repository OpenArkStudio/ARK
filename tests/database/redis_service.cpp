// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>

#include "redis/src/detail/redis_service.hpp"

using namespace std::chrono_literals;

int main() {
    try
    {
        const auto* host = std::getenv("ARK_DATABASE_REDIS_HOST");
        const auto* port = std::getenv("ARK_DATABASE_REDIS_PORT");
        if (!host || !*host || !port || !*port)
        {
            throw std::invalid_argument(
                "Set ARK_DATABASE_REDIS_HOST and ARK_DATABASE_REDIS_PORT for an isolated test server");
        }
        arknet::redis::pool_options options;
        options.connection.host = host;
        options.connection.port = port;
        if (const auto* username = std::getenv("ARK_DATABASE_REDIS_USER"))
        {
            options.connection.username = username;
        }
        if (const auto* password = std::getenv("ARK_DATABASE_REDIS_PASSWORD"))
        {
            options.connection.password = password;
        }
        options.connection.client_name = "ark-database-service-test";
        options.max_connections = 1;
        options.acquire_timeout = 2s;
        options.client_configuration.command_timeout = 2s;
        options.client_configuration.response_limits.max_bulk_bytes = 1024;
        ark::detail::redis_service service(options, 1, 8192);
        std::optional<ark::detail::redis_service_result> completed;
        unsigned callbacks{};
        const auto owner = std::this_thread::get_id();
        auto handler = [&](ark::detail::redis_service_result result) {
            if (std::this_thread::get_id() != owner)
            {
                throw std::runtime_error("Business callback escaped the module thread");
            }
            ++callbacks;
            completed = std::move(result);
        };
        const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
        const auto key = "ark-private-service-" + std::to_string(unique);
        const std::string expected("binary\0value", 12);
        std::string input = expected;
        if (!service.submit_pipeline({{"SET", key, input}, {"GET", key}}, handler, 2s))
        {
            throw std::runtime_error("First operation was not admitted");
        }
        input.assign("modified caller storage");
        if (service.submit({"PING"}, handler, 2s))
        {
            throw std::runtime_error("Completed/active operations bypassed module admission limit");
        }
        auto await = [&] {
            const auto expires = std::chrono::steady_clock::now() + 4s;
            while (!completed && std::chrono::steady_clock::now() < expires)
            {
                service.update();
                std::this_thread::sleep_for(1ms);
            }
            if (!completed)
            {
                throw std::runtime_error("No module completion before timeout");
            }
            if (auto failure = std::get_if<std::exception_ptr>(&*completed))
            {
                std::rethrow_exception(*failure);
            }
            auto result = std::get<arknet::redis::result>(std::move(*completed));
            completed.reset();
            if (!result)
            {
                throw std::runtime_error("Native Redis failed: " + std::to_string(static_cast<int>(result.error)));
            }
            return result;
        };
        auto owned = await();
        if (owned.replies.size() != 2 || owned.replies[1].type != arknet::redis::value_type::bulk_string ||
            owned.replies[1].text != expected)
        {
            throw std::runtime_error("Pipeline/binary result did not preserve ownership");
        }
        if (!service.submit({"DEL", key}, handler, 2s))
        {
            throw std::runtime_error("Module admission did not recover after dispatch");
        }
        auto removed = await();
        if (removed.replies.size() != 1 || removed.replies[0].integer != 1 || owned.replies[1].text != expected)
        {
            throw std::runtime_error("A later command invalidated an owning result");
        }
        if (!service.submit({"PING"}, handler, 2s))
        {
            throw std::runtime_error("Stop test was not admitted");
        }
        service.stop();
        if (service.update() != 0 || callbacks != 2 || service.submit({"PING"}, handler, 2s))
        {
            throw std::runtime_error("Stop delivered business code or accepted new work");
        }
        auto ephemeral = std::make_unique<ark::detail::redis_service>(options);
        bool destroyed_from_callback{};
        if (!ephemeral->submit(
                {"PING"},
                [&](ark::detail::redis_service_result result) {
                    if (auto failure = std::get_if<std::exception_ptr>(&result))
                    {
                        std::rethrow_exception(*failure);
                    }
                    if (!std::get<arknet::redis::result>(result))
                    {
                        throw std::runtime_error("Callback-destruction PING failed");
                    }
                    ephemeral.reset();
                    destroyed_from_callback = true;
                },
                2s))
        {
            throw std::runtime_error("Callback-destruction operation was not admitted");
        }
        const auto destruction_deadline = std::chrono::steady_clock::now() + 4s;
        while (!destroyed_from_callback && std::chrono::steady_clock::now() < destruction_deadline)
        {
            ephemeral->update();
            std::this_thread::sleep_for(1ms);
        }
        if (!destroyed_from_callback)
        {
            throw std::runtime_error("Callback-destruction completion timed out");
        }
        std::cout
            << "private Redis service: owning binary pipeline, admission, module-thread dispatch and stop passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
