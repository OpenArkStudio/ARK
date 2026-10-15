#!/usr/bin/env sh
set -eu

cd "$(dirname "$0")"
"${PYTHON:-python}" config_tool/config_tool.py -p ../resource
"${CLANG_FORMAT:-clang-format}" -i --style=file:../../.clang-format ../resource/code/meta_define.hpp
cp ../resource/code/meta_define.hpp ../../include/ark/meta_define.hpp
