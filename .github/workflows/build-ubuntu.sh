#!/usr/bin/env sh

#zstd
sudo apt install libzstd-dev -y

#zlib
sudo apt-get install zlib1g-dev -y

#Protobuf
sudo apt install protobuf-compiler -y

#boost
sudo apt install libboost-dev

# build ark
if [ -d "cmake-build" ]; then rm -rf cmake-build; fi
# shellcheck disable=SC2164
mkdir cmake-build && cd cmake-build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DARK_BUILD_EXAMPLES=OFF -DARK_BUILD_TESTS=OFF ..
make -j 4
cd ..