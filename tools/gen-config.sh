#!/usr/bin/env sh
set -eu

cd "$(dirname "$0")"
"${PYTHON:-python}" config_tool/config_tool.py -p ../resources
"${CLANG_FORMAT:-clang-format}" -i --style=file:../.clang-format ../resources/code/meta_define.hpp
cp ../resources/code/meta_define.hpp ../include/ark/meta_define.hpp
