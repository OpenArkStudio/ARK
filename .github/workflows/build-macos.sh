#!/usr/bin/env sh

#zstd
brew install zstd

#zlib
brew install zlib

#Protobuf
brew install protobuf

#boost
brew install boost

# git submodules
git submodule sync
git submodule update --init --recursive

# build ark
if [ -d "cmake-build" ]; then rm -rf cmake-build; fi
# shellcheck disable=SC2164
mkdir cmake-build && cd cmake-build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DARK_BUILD_EXAMPLES=OFF -DARK_BUILD_TESTS=OFF ..
make -j 4
cd ..