#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 || $# -gt 3 || ! -x "$1" ]]; then
    printf 'Usage: bash benchmarks/run_network_buffers.sh EXECUTABLE [ROUNDS] [INITIAL_BUFFER_BYTES]\n' >&2
    exit 2
fi
executable=$1
rounds=${2:-3}
if [[ ! "$rounds" =~ ^[1-9][0-9]*$ ]]; then
    printf 'ROUNDS must be a positive integer.\n' >&2
    exit 2
fi
initial_buffer=${3:-}
if [[ $# -eq 3 ]]; then
    if [[ ! "$3" =~ ^[1-9][0-9]*$ ]]; then
        printf 'INITIAL_BUFFER_BYTES must be a positive integer.\n' >&2
        exit 2
    fi

fi
run() {
    if [[ -n "$initial_buffer" ]]; then
        "$executable" --initial-buffer "$initial_buffer" "$@"
    else
        "$executable" "$@"
    fi
}
for ((round = 1; round <= rounds; ++round)); do
    run --mode sessions --sessions 10000
    run --mode framing --iterations 20000000 --payload 64 --chunk 65536
    run --mode framing --iterations 10000000 --payload 1024 --chunk 65536
    run --mode framing --iterations 5000000 --payload 4096 --chunk 65536
    run --mode framing --iterations 5000000 --payload 1024 --chunk 64
    run --mode framing --iterations 50000 --payload 4096 --chunk 1
    run --mode framing --iterations 312500 --payload 1024 --chunk 64 --batch 16
    run --mode framing --iterations 250000 --payload 4096 --chunk 65536 --batch 16
done
