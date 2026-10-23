// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/random/random_generator.hpp>

// Keep the first inclusion separate to check standalone inclusion and the include guard.
#include <array>
#include <cstdint>
#include <limits>

#include "../network/doctest.h"

#include <ark/random/random_generator.hpp>

TEST_CASE("Random generator retains its seed sequence and resets its count") {
    ark::random_generator generator;
    CHECK(generator.get_seed() == 0);
    CHECK(generator.get_random_count() == 0);
    generator.set_seed(1);

    const std::array<std::uint32_t, 6> expected{476707713, 1186278907, 505671508, 2137716191, 936145377, 1215825599};
    for (const auto value : expected)
    {
        CHECK(generator.random() == value);
    }
    CHECK(generator.get_seed() == 3535774167u);
    CHECK(generator.get_random_count() == 6);
    generator.set_seed(1);
    CHECK(generator.get_seed() == 1);
    CHECK(generator.get_random_count() == 0);
    CHECK(generator.random() == expected.front());
}

TEST_CASE("Signed random ranges retain legacy in-range values and unsigned overloads") {
    ark::random_generator signed_values;
    ark::random_generator unsigned_values;
    signed_values.set_seed(1);
    unsigned_values.set_seed(1);

    const std::array<int, 6> expected_signed{13, 7, 8, 91, 77, 99};
    const std::array<std::uint32_t, 6> expected_unsigned{23, 17, 18, 101, 87, 109};
    for (std::size_t index = 0; index < expected_signed.size(); ++index)
    {
        CHECK(signed_values.random(-100, 100) == expected_signed[index]);
        CHECK(unsigned_values.random(10u, 110u) == expected_unsigned[index]);
    }
    CHECK(signed_values.get_seed() == unsigned_values.get_seed());
    CHECK(signed_values.get_random_count() == unsigned_values.get_random_count());
    CHECK(unsigned_values.random(0u) == 0);
    CHECK(unsigned_values.random(10u, 10u) == 10);
    CHECK(unsigned_values.get_seed() == 3535774167u);
    CHECK(unsigned_values.get_random_count() == 6);
}

TEST_CASE("Signed random ranges decode full-width spans without overflow") {
    ark::random_generator generator;
    generator.set_seed(1);
    const std::array<int, 6> expected{-1670775935, -961204741, -1641812140, -9767457, -1211338271, -931658049};
    for (const auto value : expected)
    {
        CHECK(generator.random(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()) == value);
    }
    CHECK(generator.get_seed() == 3535774167u);
    CHECK(generator.get_random_count() == 6);
}

TEST_CASE("Signed random ranges preserve half-open reversed and equal bounds") {
    constexpr int lowest = std::numeric_limits<int>::min();
    constexpr int highest = std::numeric_limits<int>::max();
    const std::array<std::array<int, 2>, 12> ranges{
        {{lowest, highest}, {lowest, 0}, {-1, highest}, {lowest, 1}, {-100, 100}, {-100, -1}, {0, highest},
            {lowest, lowest + 1}, {highest - 1, highest}, {lowest, lowest}, {highest, highest}, {0, 0}}};
    for (const auto& bounds : ranges)
    {
        CAPTURE(bounds[0]);
        CAPTURE(bounds[1]);
        for (const std::uint32_t seed : {0u, 1u, 123456789u, std::numeric_limits<std::uint32_t>::max()})
        {
            ark::random_generator forward;
            ark::random_generator reversed;
            forward.set_seed(seed);
            reversed.set_seed(seed);
            for (int index = 0; index < 64; ++index)
            {
                const int value = forward.random(bounds[0], bounds[1]);
                CHECK(value == reversed.random(bounds[1], bounds[0]));
                if (bounds[0] == bounds[1])
                {
                    CHECK(value == bounds[0]);
                    CHECK(forward.get_seed() == seed);
                    CHECK(forward.get_random_count() == 0);
                }
                else
                {
                    CHECK(value >= bounds[0]);
                    CHECK(value < bounds[1]);
                    CHECK(forward.get_random_count() == index + 1);
                }
                CHECK(forward.get_seed() == reversed.get_seed());
                CHECK(forward.get_random_count() == reversed.get_random_count());
            }
        }
    }
}
