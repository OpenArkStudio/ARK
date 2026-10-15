// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/guid128.hpp>

void compile_guid128_header() {
    ark::guid_t128 value(1, 2);
    static_cast<void>(value.to_string());
    static_cast<void>(value.from_string("3-4"));
}
