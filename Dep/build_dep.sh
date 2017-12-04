echo "Building dependencies..."

if [ -d "lib" ]; then
    rm -rf lib
fi

mkdir -p lib/Debug/
mkdir -p lib/Release/

#####################################################################
echo "Building protobuf..."

if [ -d "protobuf" ]; then
    rm -rf protobuf
fi

git clone -b 2.7.0 https://github.com/google/protobuf.git
cd protobuf/cmake
mkdir build && cd build
cmake -G "Unix Makefiles" -Dprotobuf_BUILD_SHARED_LIBS=ON -Dprotobuf_BUILD_TESTS=OFF ..
make -j 8
cp -R -f *.so* ../../../lib/Debug
cp -R -f *.so* ../../../lib/Release
cp -R -f protoc ../../../lib/Debug
cp -R -f protoc ../../../lib/Release
cd ../../../
####################################################################
echo "Building libevent..."

if [ -d "libevent" ]; then
    rm -rf libevent
fi

git clone -b master https://github.com/libevent/libevent.git

cd libevent
chmod -R 755 *
./autogen.sh #depend automake & libtool
./configure --disable-shared --disable-openssl
make -j 8
cp -R -f .libs/*.a ../lib/Debug
cp -R -f .libs/*.a ../lib/Release
cd ../

##################################################################
echo Building evpp...

if [ -d "evpp" ]; then
    rm -rf evpp
fi

git clone -b master https://github.com/ArkGame/evpp.git
cd evpp
chmod -R 755 *
mkdir build
cd build

CURDIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )
echo $CURDIR
cmake -G "Unix Makefiles"  -DLIBEVENT_DIR=$CURDIR/../../libevent -DLIBEVENT_LIB_DIR=$CURDIR/../../lib/Release ..
make -j 8
cp -R -f lib/*.a ../../lib/Debug
cp -R -f lib/*.a ../../lib/Release
cd ../../
