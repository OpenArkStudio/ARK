// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/lock/rw_lock.hpp>

void compile_rw_lock_header() {
    ark::reader_writer_lock lock;
    { ark::scope_r_lock guard(lock); }
    { ark::scope_w_lock guard(lock); }
}
