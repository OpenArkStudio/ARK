// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <algorithm>
#include <array>
#include <cstdlib>
#include <memory>
#include <string>

#include "../network/doctest.h"

#include <ark/crypto/xxtea.hpp>

namespace {

std::string encrypt(const std::string& message, const void* key) {
    size_t length = 0;
    std::unique_ptr<void, decltype(&std::free)> value(
        ark::xxtea::xxtea_encrypt(message.data(), message.size(), key, &length), &std::free);
    if (!value)
    {
        return {};
    }
    return {static_cast<const char*>(value.get()), length};
}

} // namespace

DOCTEST_TEST_CASE("XXTEA reads short keys only through their terminating zero") {
    const std::string message("binary\0message", 14);
    for (size_t length = 0; length < 16; ++length)
    {
        DOCTEST_CAPTURE(length);
        // Exact allocation makes reading beyond the terminator observable.
        auto short_key = std::make_unique<char[]>(length + 1);
        std::fill_n(short_key.get(), length, 'k');
        short_key[length] = '\0';
        std::array<char, 16> padded{};
        std::fill_n(padded.begin(), length, 'k');
        const auto ciphertext = encrypt(message, short_key.get());
        DOCTEST_REQUIRE_FALSE(ciphertext.empty());
        DOCTEST_CHECK(ciphertext == encrypt(message, padded.data()));
        size_t decoded_size = 0;
        std::unique_ptr<void, decltype(&std::free)> decoded(
            ark::xxtea::xxtea_decrypt(ciphertext.data(), ciphertext.size(), short_key.get(), &decoded_size),
            &std::free);
        DOCTEST_REQUIRE(decoded != nullptr);
        DOCTEST_CHECK(std::string(static_cast<const char*>(decoded.get()), decoded_size) == message);
    }
}

DOCTEST_TEST_CASE("XXTEA preserves full-width and zero-terminated key normalization") {
    const std::string message = "message";
    const std::array<char, 16> full{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    DOCTEST_CHECK(encrypt(message, full.data()) == encrypt(message, "0123456789abcdefignored"));
    const std::array<char, 16> embedded{'k', '\0', 'x', 'y'};
    DOCTEST_CHECK(encrypt(message, embedded.data()) == encrypt(message, "k"));
    std::string ciphertext = "prefix:";
    DOCTEST_REQUIRE(ark::xxtea::xxtea_encrypt(message, "k", ciphertext));
    DOCTEST_CHECK(ciphertext == "prefix:" + encrypt(message, "k"));
    std::string plaintext = "prefix:";
    DOCTEST_REQUIRE(ark::xxtea::xxtea_decrypt(ciphertext.substr(7), "k", plaintext));
    DOCTEST_CHECK(plaintext == "prefix:" + message);
    DOCTEST_CHECK_FALSE(ark::xxtea::xxtea_encrypt(std::string{}, "k", ciphertext));
    DOCTEST_CHECK_FALSE(ark::xxtea::xxtea_decrypt(std::string{}, "k", plaintext));
}
