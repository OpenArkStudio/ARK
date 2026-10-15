#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
source "$script_dir/style_files.sh"

if [[ $# -lt 1 ]]; then
    printf 'Usage: %s BUILD_DIR [source.cpp ...]\n' "$0" >&2
    exit 2
fi

build_dir=$(cd -- "$1" && pwd -P)
shift
database="$build_dir/compile_commands.json"
if [[ ! -f "$database" ]]; then
    printf 'Missing %s; configure CMake with CMAKE_EXPORT_COMPILE_COMMANDS=ON.\n' "$database" >&2
    exit 1
fi

tidy=${CLANG_TIDY:-clang-tidy}
if ! command -v "$tidy" >/dev/null 2>&1; then
    printf 'clang-tidy is required; set CLANG_TIDY to its path.\n' >&2
    exit 1
fi

cd -- "$ark_style_root"
source_files=("$@")
if [[ ${#source_files[@]} -eq 0 ]]; then
    while IFS= read -r -d '' style_file; do
        case "$style_file" in
            *.cpp|*.cc|*.c)
                if grep -Fq "\"file\": \"$ark_style_root/$style_file\"" "$database"; then
                    source_files+=("$style_file")
                fi
                ;;
        esac
    done < <(ark_style_files)
fi

if [[ ${#source_files[@]} -eq 0 ]]; then
    printf 'No ark source files found in the compilation database.\n' >&2
    exit 1
fi

"$tidy" -p "$build_dir" --config-file="$ark_style_root/.clang-tidy" \
    --warnings-as-errors=readability-identifier-naming "${source_files[@]}"
