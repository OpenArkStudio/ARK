#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 || $# -gt 2 || ! -x "$1" ]]; then
    printf 'Usage: bash benchmarks/run_network_buffers.sh EXECUTABLE [ROUNDS]\n' >&2
    exit 2
fi
executable=$1
rounds=${2:-3}
if [[ ! "$rounds" =~ ^[1-9][0-9]*$ ]]; then
    printf 'ROUNDS must be a positive integer.\n' >&2
    exit 2
fi
for ((round = 1; round <= rounds; ++round)); do
    "$executable" --mode sessions --sessions 10000
    "$executable" --mode framing --iterations 20000000 --payload 64 --chunk 65536
    "$executable" --mode framing --iterations 10000000 --payload 1024 --chunk 65536
    "$executable" --mode framing --iterations 5000000 --payload 4096 --chunk 65536
    "$executable" --mode framing --iterations 5000000 --payload 1024 --chunk 64
    "$executable" --mode framing --iterations 50000 --payload 4096 --chunk 1
    "$executable" --mode framing --iterations 312500 --payload 1024 --chunk 64 --batch 16
    "$executable" --mode framing --iterations 250000 --payload 4096 --chunk 65536 --batch 16
done
