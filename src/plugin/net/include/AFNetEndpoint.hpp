// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.

#pragma once

#include <memory>
#include <utility>

#include <arknet/external/asio.hpp>

namespace ark::detail {

struct NetEndpointCleanup
{
    asio::thread_pool executor{1};

    ~NetEndpointCleanup()
    {
        // Complete every queued deletion before the executor's destructor stops it.
        executor.join();
    }
};

inline asio::thread_pool& NetEndpointCleanupExecutor()
{
    static NetEndpointCleanup cleanup;
    return cleanup.executor;
}

template<class Endpoint, class... Args>
std::shared_ptr<Endpoint> MakeNetEndpoint(Args&&... args)
{
    // Initialize the process cleanup executor before an IO callback needs it.
    auto& cleanup = NetEndpointCleanupExecutor();
    return std::shared_ptr<Endpoint>(new Endpoint(std::forward<Args>(args)...), [&cleanup](Endpoint* endpoint) {
        if (endpoint->iopool().running_in_threads())
        {
            // An owned pool must join its workers from an external thread.
            asio::post(cleanup, [endpoint] { delete endpoint; });
        }
        else
        {
            delete endpoint;
        }
    });
}

} // namespace ark::detail
