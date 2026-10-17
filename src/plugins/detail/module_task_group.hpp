// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <ark/task.hpp>

namespace ark::detail {

template<class result_type = void>
using module_task = ark::task<result_type>;

using module_task_group = ark::task_group;

} // namespace ark::detail
