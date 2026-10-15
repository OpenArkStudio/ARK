#!/usr/bin/env bash
# Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0
set -euo pipefail

if [[ $# -lt 1 || $# -gt 2 ]]; then
    printf 'Usage: %s BUILD_DIR [local|redis|mysql|all]\n' "$0" >&2
    exit 2
fi

database_test_dir=$(cd -- "$1" && pwd)/tests/database
database_test_mode=${2:-local}
case "$database_test_mode" in
    local|redis|mysql|all) ;;
    *) printf 'Unknown database test mode: %s\n' "$database_test_mode" >&2; exit 2 ;;
esac

run_database_test() {
    local database_test_binary="$database_test_dir/$1"
    if [[ ! -x "$database_test_binary" && -x "$database_test_binary.exe" ]]; then
        database_test_binary+='.exe'
    fi
    if [[ ! -x "$database_test_binary" ]]; then
        printf 'Build the requested test target first: %s\n' "$1" >&2
        exit 1
    fi
    "$database_test_binary"
}

if [[ "$database_test_mode" == local || "$database_test_mode" == all ]]; then
    run_database_test ark_database_queue_tests
    run_database_test ark_database_redis_stop_tests
fi
if [[ "$database_test_mode" == redis || "$database_test_mode" == all ]]; then
    run_database_test ark_database_redis_tests
fi
if [[ "$database_test_mode" == mysql || "$database_test_mode" == all ]]; then
    run_database_test ark_database_mysql_tests
fi
