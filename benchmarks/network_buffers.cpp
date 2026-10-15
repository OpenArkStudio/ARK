// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "net/include/AFNetSession.hpp"

namespace {

bool count_allocations{};
std::size_t array_allocations{};
std::size_t array_bytes{};
std::size_t object_allocations{};
std::size_t object_bytes{};

struct Options
{
    std::string_view mode{"framing"};
    std::size_t sessions{10000};
    std::size_t iterations{200000};
    std::size_t payload{1024};
    std::size_t chunk{64};
    std::size_t batch{1};
    std::size_t warmup{10000};
};

std::size_t Number(std::string_view text, bool allow_zero = false)
{
    std::size_t value{};
    const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
    if (result.ec != std::errc{} || result.ptr != text.data() + text.size() || (!allow_zero && value == 0))
    {
        throw std::invalid_argument(allow_zero ? "Expected a nonnegative integer" : "Expected a positive integer");
    }
    return value;
}

Options ParseOptions(int argc, char** argv)
{
    Options options;
    for (int i = 1; i < argc; i += 2)
    {
        const std::string_view key(argv[i]);
        if (i + 1 >= argc)
        {
            throw std::invalid_argument("Each option requires a value");
        }
        const std::string_view value(argv[i + 1]);
        if (key == "--mode")
        {
            options.mode = value;
        }
        else if (key == "--sessions")
        {
            options.sessions = Number(value);
        }
        else if (key == "--iterations")
        {
            options.iterations = Number(value);
        }
        else if (key == "--warmup-iterations")
        {
            options.warmup = Number(value, true);
        }
        else if (key == "--payload")
        {
            options.payload = Number(value);
        }
        else if (key == "--batch")
        {
            options.batch = Number(value);
        }
        else if (key == "--chunk")
        {
            options.chunk = Number(value);
        }
        else
        {
            throw std::invalid_argument("Unknown option: " + std::string(key));
        }
    }
    if ((options.mode != "sessions" && options.mode != "framing") || options.payload > ark::ARK_MSG_MAX_LENGTH)
    {
        throw std::invalid_argument("Use --mode sessions|framing and payloads within ARK_MSG_MAX_LENGTH");
    }
    if (options.iterations > std::numeric_limits<std::size_t>::max() / options.batch)
    {
        throw std::invalid_argument("The total message count exceeds the addressable range");
    }
    return options;
}

void Check(bool value)
{
    if (!value)
    {
        throw std::runtime_error("Frame content or message count mismatch");
    }
}

void ParseFrames(ark::AFTCPClientSession& session, std::string_view frame, std::string_view body,
    const Options& options, std::size_t iterations)
{
    for (std::size_t i = 0; i < iterations; ++i)
    {
        for (std::size_t pos = 0; pos < frame.size();)
        {
            const auto count = std::min(options.chunk, frame.size() - pos);
            session.AddBuffer(frame.data() + pos, count);
            Check(session.ParseBufferToMsg());
            pos += count;
        }
        for (std::size_t j = 0; j < options.batch; ++j)
        {
            ark::AFNetMsg* message{};
            Check(session.PopNetMsg(message));
            const bool valid = message->GetMsgId() == 7 && message->GetMsgLength() == body.size() &&
                               std::string_view(message->GetMsgData(), message->GetMsgLength()) == body;
            ark::AFNetMsg::Release(message);
            Check(valid);
        }
        ark::AFNetMsg* extra{};
        Check(!session.PopNetMsg(extra) && session.GetBufferLen() == 0);
    }
}

double ProcessCpuSeconds()
{
#if defined(_WIN32)
    FILETIME created{}, exited{}, kernel{}, user{};
    if (!GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user))
    {
        throw std::runtime_error("Cannot read process CPU usage");
    }
    const auto ticks = [](const FILETIME& value) {
        return (static_cast<std::uint64_t>(value.dwHighDateTime) << 32) | value.dwLowDateTime;
    };
    return static_cast<double>(ticks(kernel) + ticks(user)) / 10000000.0;
#else
    return static_cast<double>(std::clock()) / CLOCKS_PER_SEC;
#endif
}

void Run(const Options& options)
{
    auto header = std::make_shared<ark::AFCSMsgHeader>();
    header->SetMessageId(7);
    header->SetMessageLength(static_cast<uint32_t>(options.payload));
    const std::string body(options.payload, 'x');
    std::string frame;
    header->SerializeToString(frame);
    frame += body;
    const auto single = frame;
    for (std::size_t i = 1; i < options.batch; ++i)
    {
        frame += single;
    }

    std::vector<std::unique_ptr<ark::AFTCPClientSession>> sessions;
    sessions.reserve(options.mode == "sessions" ? options.sessions : 1);
    if (options.mode == "framing")
    {
        sessions.push_back(std::make_unique<ark::AFTCPClientSession>(header, 1));
        ParseFrames(*sessions.front(), frame, body, options, options.warmup);
    }
    const auto cpu_start = ProcessCpuSeconds();
    const auto start = std::chrono::steady_clock::now();
    count_allocations = true;
    if (options.mode == "sessions")
    {
        for (std::size_t i = 0; i < options.sessions; ++i)
        {
            sessions.push_back(std::make_unique<ark::AFTCPClientSession>(header, i + 1));
        }
    }
    else
    {
        ParseFrames(*sessions.front(), frame, body, options, options.iterations);
    }
    count_allocations = false;
    const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    const auto cpu = ProcessCpuSeconds() - cpu_start;
    const auto count = options.mode == "sessions" ? options.sessions : options.iterations * options.batch;
    std::cout << "{\"mode\":\"" << options.mode << "\",\"operations\":" << count
              << ",\"session_size_bytes\":" << sizeof(ark::AFTCPClientSession)
              << ",\"session_alignment_bytes\":" << alignof(ark::AFTCPClientSession)
              << ",\"payload_bytes\":" << options.payload << ",\"chunk_bytes\":" << options.chunk
              << ",\"batch_messages\":" << options.batch
              << ",\"warmup_iterations\":" << (options.mode == "framing" ? options.warmup : 0)
              << ",\"elapsed_seconds\":" << elapsed << ",\"operations_per_second\":" << count / elapsed
              << ",\"cpu_percent\":" << cpu * 100.0 / elapsed << ",\"cpp_array_allocations\":" << array_allocations
              << ",\"cpp_array_requested_bytes\":" << array_bytes
              << ",\"cpp_object_allocations\":" << object_allocations
              << ",\"cpp_object_requested_bytes\":" << object_bytes << ",\"ok\":true}\n";
}

} // namespace

// Count ordinary C++ requests; aligned allocations, queue-internal malloc calls
// and socket buffers are outside these metrics. This program runs on one thread.
void* operator new(std::size_t size)
{
    if (void* value = std::malloc(size == 0 ? 1 : size))
    {
        if (count_allocations)
        {
            ++object_allocations;
            object_bytes += size;
        }
        return value;
    }
    throw std::bad_alloc();
}

void operator delete(void* value) noexcept
{
    std::free(value);
}

void operator delete(void* value, std::size_t) noexcept
{
    std::free(value);
}

void* operator new[](std::size_t size)
{
    if (void* value = std::malloc(size == 0 ? 1 : size))
    {
        if (count_allocations)
        {
            ++array_allocations;
            array_bytes += size;
        }
        return value;
    }
    throw std::bad_alloc();
}

void operator delete[](void* value) noexcept
{
    std::free(value);
}

void operator delete[](void* value, std::size_t) noexcept
{
    std::free(value);
}

int main(int argc, char** argv)
{
    try
    {
        if (argc == 2 && std::string_view(argv[1]) == "--help")
        {
            std::cout << "Usage: ark_network_buffer_benchmark --mode sessions|framing\n"
                         "  [--sessions N] [--iterations N] [--payload BYTES] [--chunk BYTES] [--batch N]\n"
                         "  [--warmup-iterations N]\n";
            return 0;
        }
        Run(ParseOptions(argc, argv));
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
