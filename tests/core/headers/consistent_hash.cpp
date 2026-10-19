// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/container/consistent_hash.hpp>

// Keep the second inclusion separate to retain the include-guard check.
#include <ark/container/consistent_hash.hpp>

template class ark::consistent_hashmap<ark::v_node, ark::crc_hasher>;
template class ark::consistent_hashmap<ark::v_node, ark::murmur_hasher>;
