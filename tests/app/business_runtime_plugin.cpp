// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <csignal>
#include <cstdint>
#include <exception>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>

#include <ark/plugin_manager.hpp>
#include "detail/database_awaitable.hpp"

namespace {

void trace(const std::string& event) {
    std::ofstream output("business-runtime.trace", std::ios::app);
    output << event << '\n';
}

// A controlled protocol completion, not a real database client or public plugin.
// No result is released until the real launcher enters managed-task drain.
class probe_module final : public ark::module
{
public:
    probe_module(ark::plugin_manager& manager, std::string mode)
        : manager_(manager)
        , mode_(std::move(mode))
        , bridge_(manager.get_task_runtime(), 1) {
    }

    ~probe_module() override {
        trace("destroy");
    }

    const std::string& get_name() const override {
        return name_;
    }

    bool init() override {
        auto& tasks = manager_.create_task_group(*this, 1);
        return tasks.start(sequence(this), [this](std::exception_ptr error) {
            if (error)
            {
                trace("task-error");
                return;
            }
            trace("complete-begin");
            if (manager_.stop(manager_.get_now_time()))
            {
                throw std::runtime_error("Teardown entered the final completion stack");
            }
            trace("callback-stop-pending");
            completion_returned_ = true;
            trace("complete-end");
        }) != 0;
    }

    bool update() override {
        if (pending_)
        {
            const auto state = manager_.get_shutdown_state();
            if (state == ark::plugin_manager::shutdown_state::running && !stop_requested_)
            {
                stop_requested_ = true;
                trace("request-stop");
#if !defined(_WIN32)
                std::raise(SIGTERM);
#endif
            }
            else if (state == ark::plugin_manager::shutdown_state::draining)
            {
                trace("reply-" + std::to_string(started_));
                auto complete = std::move(pending_);
                complete({}, started_ == 1 ? "first" : "second");
            }
        }
        bridge_.dispatch();
        return true;
    }

    bool pre_shut() override {
        trace("pre-shut");
        if (!sequence_finished_ || !completion_returned_ || pending_ || bridge_.pending())
        {
            throw std::runtime_error("Teardown preceded accepted business completion");
        }
        return mode_ != "pre_shut_false";
    }

    bool shut() override {
        trace("shut");
        if (mode_ == "shut_throw")
        {
            throw std::runtime_error("test shutdown exception");
        }
        return mode_ != "shut_false";
    }

private:
    using callback = std::function<void(std::exception_ptr, std::string)>;

    std::uint64_t start(callback complete) {
        if (pending_)
        {
            throw std::logic_error("Overlapping controlled operations");
        }
        pending_ = std::move(complete);
        trace("start-" + std::to_string(++started_));
        return started_;
    }

    static ark::task<> sequence(probe_module* self) {
        for (unsigned step = 1; step <= 2; ++step)
        {
            const auto response =
                co_await self->bridge_.wait([self](auto complete) { return self->start(std::move(complete)); },
                    [](std::uint64_t) { trace("cancel"); });
            if (response != (step == 1 ? "first" : "second"))
            {
                throw std::runtime_error("Controlled operation lost its owning response");
            }
            trace("received-" + std::to_string(step));
        }
        self->sequence_finished_ = true;
    }

    const std::string name_{"business-runtime-probe"};
    ark::plugin_manager& manager_;
    const std::string mode_;
    ark::detail::database_awaitable<std::string> bridge_;
    callback pending_;
    unsigned started_{};
    bool stop_requested_{};
    bool sequence_finished_{};
    bool completion_returned_{};
};

probe_module* active_module{};

} // namespace

ARK_EXPORT_FUNC void DllEntryPlugin(ark::plugin_manager* manager, const std::string& name, ark::logger* logger) {
    ark::logger::init(logger);
    active_module = new probe_module(*manager, manager->get_plugin_conf(name));
    if (!manager->add_module(active_module->get_name(), active_module) || !manager->add_update_module(active_module))
    {
        throw std::runtime_error("Cannot register the business-runtime test module");
    }
}

ARK_EXPORT_FUNC void DllExitPlugin(ark::plugin_manager* manager) {
    trace("uninstall");
    manager->remove_module(active_module->get_name());
    manager->remove_update_module(active_module->get_name());
    delete std::exchange(active_module, nullptr);
}
