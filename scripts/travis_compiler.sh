#!/bin/bash

# git submodules
git submodule sync
git submodule update --init

os_name=`uname`

# build ark
if [ -d "build" ]; then rm -rf build; fi
mkdir build && cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DENABLE_COVERAGE=OFF -DBUILD_SAMPLES=ON -DBUILD_TESTS=ON ..

if [ "$USE_CXX" != g++-9 ]; then
    exit 0
fi

# make and sonar scanner
if [ "$os_name" = Linux ]; then
    build-wrapper-linux-x86-64 --out-dir bw-output make -j 4
elif [ "$os_name" = Darwin ]; then
    build-wrapper-macosx-x86 --out-dir bw-output make -j 4
fi

cd ..
