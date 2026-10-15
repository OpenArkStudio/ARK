// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.

#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

#include "mysql/src/detail/mysql_service.hpp"

using namespace std::chrono_literals;
using ark::detail::mysql_service;
using ark::detail::mysql_service_reply;
using ark::detail::mysql_service_result;
using namespace arknet::mysql;

namespace {

unsigned checks{};

std::string required_environment(const char* name, bool allow_empty = false) {
    const auto* value = std::getenv(name);
    if (!value || (!allow_empty && !*value))
    {
        throw std::invalid_argument(std::string("Required test environment variable: ") + name);
    }
    return value;
}

std::uint16_t mysql_port() {
    const auto text = required_environment("ARK_DATABASE_MYSQL_PORT");
    unsigned port{};
    const auto parsed = std::from_chars(text.data(), text.data() + text.size(), port);
    if (parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size() || !port || port > 65535)
    {
        throw std::invalid_argument("ARK_DATABASE_MYSQL_PORT must be an integer between 1 and 65535");
    }
    return static_cast<std::uint16_t>(port);
}

void require(bool condition, const char* message) {
    ++checks;
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

mysql_service_reply await(mysql_service& service, std::optional<mysql_service_result>& completion) {
    const auto deadline = std::chrono::steady_clock::now() + 6s;
    while (!completion && std::chrono::steady_clock::now() < deadline)
    {
        service.update();
        std::this_thread::sleep_for(1ms);
    }
    require(bool(completion), "No MySQL module completion before timeout");
    if (auto* failure = std::get_if<std::exception_ptr>(&*completion))
    {
        std::rethrow_exception(*failure);
    }
    auto reply = std::get<mysql_service_reply>(std::move(*completion));
    completion.reset();
    return reply;
}

template<class submitter>
mysql_service_reply run(mysql_service& service, submitter submit) {
    std::optional<mysql_service_result> completion;
    const auto owner = std::this_thread::get_id();
    unsigned calls{};
    require(submit([&](mysql_service_result result) {
        require(std::this_thread::get_id() == owner, "Business callback escaped the module thread");
        require(++calls == 1, "Business callback completed more than once");
        completion = std::move(result);
    }),
        "MySQL operation was not admitted");
    auto reply = await(service, completion);
    require(calls == 1, "Missing business callback");
    return reply;
}

mysql_service_reply query(mysql_service& service, std::string sql, std::chrono::milliseconds timeout = 3s) {
    return run(service, [&](mysql_service::callback handler) {
        return service.submit_query(std::move(sql), std::move(handler), timeout);
    });
}

mysql_service_reply execute(mysql_service& service, std::string sql, std::vector<field_value> parameters,
    std::chrono::milliseconds timeout = 3s) {
    return run(service, [&](mysql_service::callback handler) {
        return service.submit_execute(std::move(sql), std::move(parameters), std::move(handler), timeout);
    });
}

void success(const mysql_service_reply& reply) {
    if (!reply.result)
    {
        throw std::runtime_error("Native MySQL failed: " + std::to_string(static_cast<int>(reply.result.status.code)) +
                                 " " + reply.result.status.message);
    }
    require(!reply.statement_close_exception, "Statement cleanup threw after execution");
    require(!reply.statement_close || bool(*reply.statement_close), "Statement cleanup failed after execution");
}

void check_row(const query_result& result, const std::string& binary) {
    require(result.columns.size() == 7 && result.rows.size() == 1 && result.rows[0].size() == 7,
        "MySQL result dimensions changed");
    require(result.columns[0].name == "repeated" && result.columns[1].name == "repeated",
        "Duplicate column names were lost");
    const auto& row = result.rows[0];
    require(std::get<std::int64_t>(row[0]) == -7, "Signed integer type/value lost");
    require(std::get<std::uint64_t>(row[1]) == std::numeric_limits<std::uint64_t>::max(),
        "Unsigned integer type/value lost");
    require(std::get<double>(row[2]) == 1.25, "Floating-point type/value lost");
    require(std::get<std::string>(row[3]) == binary, "Binary or UTF-8 string ownership lost");
    require(std::holds_alternative<std::monostate>(row[4]), "SQL NULL converted to a non-NULL value");
    require(std::get<datetime_value>(row[5]) == datetime_value{{2026, 10, 15}, 12, 34, 56, 123456},
        "Datetime type/value lost");
    require(std::get<time_value>(row[6]) == time_value{true, 1, 2, 3, 4, 5}, "Time type/value lost");
}

} // namespace

int main() {
    pool_options options;
    std::string table;
    bool table_created{};
    try
    {
        options.connection.host = required_environment("ARK_DATABASE_MYSQL_HOST");
        options.connection.port = mysql_port();
        options.connection.username = required_environment("ARK_DATABASE_MYSQL_USER");
        options.connection.password = required_environment("ARK_DATABASE_MYSQL_PASSWORD", true);
        options.connection.database = required_environment("ARK_DATABASE_MYSQL_DATABASE");
        options.connection.ca_file = required_environment("ARK_DATABASE_MYSQL_CA");
        options.connection.timeout = 3s;
        options.max_connections = 1;
        options.io_threads = 2;
        options.acquire_timeout = 3s;
        options.reset_timeout = 3s;
        table = "ark_private_db_" +
                std::to_string(static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count()));
        mysql_service service(options, 2, 8192);
        // CREATE itself can have an unknown outcome on a transport error.
        // Cleanup must still attempt DROP IF EXISTS for this unique name.
        table_created = true;
        success(
            query(service, "CREATE TABLE " + table +
                               " (id BIGINT NOT NULL PRIMARY KEY, counter BIGINT UNSIGNED, amount DOUBLE,"
                               " payload VARBINARY(256), nullable TEXT NULL, created DATETIME(6), elapsed TIME(6))"));
        const std::string binary = std::string("binary\0", 7) + "中文😀";
        std::string sql = "INSERT INTO " + table + " VALUES(?, ?, ?, ?, ?, ?, ?)";
        std::vector<field_value> parameters{std::int64_t(-7), std::numeric_limits<std::uint64_t>::max(), 1.25, binary,
            std::monostate{}, datetime_value{{2026, 10, 15}, 12, 34, 56, 123456}, time_value{true, 1, 2, 3, 4, 5}};
        auto inserted = run(service, [&](mysql_service::callback handler) {
            const auto accepted = service.submit_execute(sql, parameters, std::move(handler), 3s);
            sql.assign("caller modified its SQL");
            std::get<std::string>(parameters[3]).assign("caller modified its parameter");
            parameters.clear();
            return accepted;
        });
        success(inserted);
        require(inserted.result.affected_rows == 1 && bool(inserted.statement_close),
            "Prepared insert or explicit close was not completed");
        const auto select =
            "SELECT id AS repeated, counter AS repeated, amount, payload, nullable, created, elapsed FROM " + table;
        auto text = query(service, select);
        success(text);
        check_row(text.result, binary);
        require(!text.statement_close, "Text query unexpectedly had statement cleanup");
        auto prepared = execute(service, select + " WHERE id = ?", {std::int64_t(-7)});
        success(prepared);
        check_row(prepared.result, binary);
        require(bool(prepared.statement_close), "Prepared result arrived before statement close");
        success(query(service, "DELETE FROM " + table));
        check_row(text.result, binary);
        check_row(prepared.result, binary);
        auto empty = query(service, select);
        success(empty);
        require(empty.result.rows.empty(), "Zero-row success changed into a row or error");

        auto server_error = query(service, "SELECT * FROM " + table + "_missing");
        require(server_error.result.status.code == connection_errc::server &&
                    server_error.result.status.server_code == 1146 && server_error.result.status.sql_state == "42S02" &&
                    !server_error.result.status.message.empty() && !server_error.result.status.outcome_unknown,
            "Server error code, SQLSTATE or execution certainty was lost");
        auto invalid_prepare = execute(service, "SELECT FROM", {});
        require(invalid_prepare.result.status.code == connection_errc::server &&
                    invalid_prepare.result.status.server_code == 1064 && !invalid_prepare.statement_close,
            "Prepare error did not stop the command chain");
        auto mismatch = execute(service, "SELECT ?", {});
        require(mismatch.result.status.code == connection_errc::invalid_options &&
                    !mismatch.result.status.outcome_unknown && bool(mismatch.statement_close) &&
                    bool(*mismatch.statement_close),
            "Rejected execute lost its error or did not close the prepared statement");

        unsigned rejected_callbacks{};
        require(!service.submit_query(
                    std::string(8193, 'x'), [&](auto) { ++rejected_callbacks; }, 3s),
            "Module retained an oversized waiting SQL input");
        require(!service.submit_execute(
                    "SELECT ?", {std::string(8193, 'x')}, [&](auto) { ++rejected_callbacks; }, 3s),
            "Module retained an oversized waiting parameter input");
        require(service.update() == 0 && rejected_callbacks == 0, "Unaccepted input triggered a callback");
        bool invalid_deadline_rejected{};
        try
        {
            service.submit_query("SELECT 1", [&](auto) { ++rejected_callbacks; }, std::chrono::milliseconds::max());
        }
        catch (const std::invalid_argument&)
        { invalid_deadline_rejected = true; }
        require(invalid_deadline_rejected && rejected_callbacks == 0,
            "Unrepresentable operation deadline was admitted or completed");

        query_options limits;
        limits.max_rows = 1;
        mysql_service limited(options, 1, 8192, limits);
        auto limited_text = query(limited, "SELECT 1 UNION ALL SELECT 2");
        require(limited_text.result.status.code == connection_errc::result_limit,
            "Text query bypassed configured result limits");
        auto limited_prepared = execute(limited, "SELECT ? UNION ALL SELECT ?", {std::int64_t(1), std::int64_t(2)});
        require(limited_prepared.result.status.code == connection_errc::result_limit &&
                    bool(limited_prepared.statement_close),
            "Prepared query bypassed configured result limits or lost cleanup status");
        limited.stop();

        // The second request expires while the sole lease runs the first. It
        // must not receive a new execution budget or insert after acquisition.
        std::optional<mysql_service_result> blocker, waiting;
        const auto owner = std::this_thread::get_id();
        require(service.submit_query(
                    "SELECT SLEEP(0.20)",
                    [&](auto result) {
                        require(std::this_thread::get_id() == owner, "Blocker callback escaped the module thread");
                        blocker = std::move(result);
                    },
                    3s),
            "Blocker was not admitted");
        require(service.submit_execute(
                    "INSERT INTO " + table + " (id) VALUES(?)", {std::int64_t(42)},
                    [&](auto result) {
                        require(std::this_thread::get_id() == owner, "Deadline callback escaped the module thread");
                        waiting = std::move(result);
                    },
                    50ms),
            "Waiting prepared operation was not admitted");
        require(!service.submit_query(
                    "SELECT 1", [&](auto) { ++rejected_callbacks; }, 3s),
            "Module operation-count budget was bypassed");
        auto expired = await(service, waiting);
        require(expired.result.status.code == connection_errc::timeout && !expired.result.status.outcome_unknown &&
                    !expired.statement_close,
            "Acquire expiry was sent or lost its known-not-executed status");
        success(await(service, blocker));
        auto count = query(service, "SELECT COUNT(*) FROM " + table);
        success(count);
        require(std::get<std::int64_t>(count.result.rows[0][0]) == 0, "Expired waiting write was executed");
        auto timed_out = execute(service, "SELECT SLEEP(?)", {0.20}, 50ms);
        require(timed_out.result.status.code == connection_errc::timeout && timed_out.result.status.outcome_unknown &&
                    bool(timed_out.statement_close),
            "Sent timeout lost its uncertainty or prepared cleanup outcome");
        require(rejected_callbacks == 0, "A rejected operation completed later");

        success(query(service, "DROP TABLE " + table));
        table_created = false;
        auto capture = std::make_shared<int>(1);
        std::weak_ptr<int> retained_capture = capture;
        require(service.submit_execute(
                    "SELECT SLEEP(?)", {0.05}, [capture, &rejected_callbacks](auto) { ++rejected_callbacks; }, 3s),
            "Stop test was not admitted");
        capture.reset();
        service.stop();
        require(retained_capture.expired(), "Stop retained an unstarted business callback capture");
        require(service.update() == 0 && rejected_callbacks == 0 &&
                    !service.submit_query(
                        "SELECT 1", [&](auto) { ++rejected_callbacks; }, 3s),
            "Stop dispatched business code or accepted new work");

        auto ephemeral = std::make_unique<mysql_service>(options);
        bool destroyed{};
        require(ephemeral->submit_execute(
                    "SELECT ?", {std::string("owned")},
                    [&](mysql_service_result result) {
                        require(std::this_thread::get_id() == owner, "Destruction callback escaped the module thread");
                        if (auto* failure = std::get_if<std::exception_ptr>(&result))
                        {
                            std::rethrow_exception(*failure);
                        }
                        const auto& reply = std::get<mysql_service_reply>(result);
                        success(reply);
                        require(bool(reply.statement_close), "Destruction callback ran before prepared close");
                        ephemeral.reset();
                        destroyed = true;
                    },
                    3s),
            "Destruction test was not admitted");
        const auto destruction_deadline = std::chrono::steady_clock::now() + 6s;
        while (!destroyed && std::chrono::steady_clock::now() < destruction_deadline)
        {
            ephemeral->update();
            std::this_thread::sleep_for(1ms);
        }
        require(destroyed, "Service destruction from its business callback timed out");

        auto stopped_by_capture = std::make_unique<mysql_service>(options);
        bool capture_destroyed{};
        auto capture_with_owner = std::shared_ptr<int>(new int(1), [&](int* value) {
            delete value;
            capture_destroyed = true;
            stopped_by_capture.reset();
        });
        require(stopped_by_capture->submit_query(
                    "SELECT 1",
                    [capture_with_owner](auto) { throw std::runtime_error("Stopped callback was invoked"); }, 3s),
            "Stop-capture destruction test was not admitted");
        capture_with_owner.reset();
        stopped_by_capture->stop();
        require(capture_destroyed && !stopped_by_capture,
            "Stop did not safely release a capture that destroys the service");
        std::cout << "ark MySQL service tests: " << checks
                  << " checks passed (owning query/prepared results, bounded admission, shared deadline, module-thread "
                     "callbacks, stop and callback destruction)\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        if (table_created)
        {
            try
            {
                mysql_service cleanup(options);
                success(query(cleanup, "DROP TABLE IF EXISTS " + table));
            }
            catch (const std::exception& cleanup_error)
            { std::cerr << "Fixture cleanup failed for " << table << ": " << cleanup_error.what() << '\n'; }
        }
        std::cerr << error.what() << '\n';
        return 1;
    }
}
