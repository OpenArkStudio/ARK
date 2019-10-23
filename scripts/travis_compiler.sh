#!/bin/bash

# git submodules
git submodule sync
git submodule update --init

# build deps
cd dep
chmod -R 755 build_dep.sh
bash build_dep.sh
cd ..

# build ark
if [ -d "build" ]; then rm -rf build; fi
mkdir build && cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DENABLE_COVERAGE=ON -DBUILD_SAMPLES=ON -DBUILD_TESTS=ON ..

# make and sonar scanner
os_name=`uname -o`
if [ "$os_name" = GNU/Linux ]
then
    build-wrapper-linux-x86-64 --out-dir bw-output make -j 4
else
    #build-wrapper-macosx-x86 --out-dir bw-output make
fi

cd ../src
