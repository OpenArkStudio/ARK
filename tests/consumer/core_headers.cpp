// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <type_traits>

#include <ark/app.hpp>
#include <ark/application_manifest.hpp>
#include <ark/container/vector.hpp>
#include <ark/module_base.hpp>
#include <ark/plugin_base.hpp>
#include <ark/time/date_time.hpp>

static_assert(std::is_enum_v<ark::app_type>);
static_assert(std::is_class_v<ark::application_manifest>);
static_assert(std::is_base_of_v<ark::module, ark::module_base<ark::module>>);
