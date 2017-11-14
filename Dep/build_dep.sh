echo "Building dependencies..."

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
cmake -G "Unix Makefiles" -Dprotobuf_BUILD_SHARED_LIBS=OFF -Dprotobuf_BUILD_TESTS=OFF ..
make -j
cp -R -f *.a ../../../lib/Debug
cp -R -f *.a ../../../lib/Release
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
cmake -G "Unix Makefiles"  -DLIBEVENT_DIR=../../libevent -DLIBEVENT_LIB_DIR=../../lib/Release ..
make -j
copy lib\*.a ..\..\lib\Debug /Y
copy lib\*.a ..\..\lib\Release /Y
cd ../../
