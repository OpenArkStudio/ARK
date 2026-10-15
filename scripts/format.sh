#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
source "$script_dir/style_files.sh"

case "${1:-}" in
    '') format_args=(-i) ;;
    --check) format_args=(--dry-run --Werror) ;;
    *) printf 'Usage: %s [--check]\n' "$0" >&2; exit 2 ;;
esac

formatter=${CLANG_FORMAT:-clang-format}
if ! command -v "$formatter" >/dev/null 2>&1; then
    printf 'clang-format 18 or newer is required; set CLANG_FORMAT to its path.\n' >&2
    exit 1
fi

cd -- "$ark_style_root"
ark_style_files | xargs -0 "$formatter" "${format_args[@]}" --style=file
