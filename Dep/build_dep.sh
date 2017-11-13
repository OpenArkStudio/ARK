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
cmake -G "Unix Makefiles" -Dprotobuf_BUILD_SHARED_LIBS=ON -Dprotobuf_BUILD_TESTS=OFF ..
make -j
cp -R -f *.so ../../../lib/Debug
cp -R -f *.so ../../../lib/Release
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

###################################################################
echo "Building glog..."

if [ -d "glog" ]; then
    rm -rf glog
fi

git clone https://github.com/google/glog.git
cd glog
chmod -R 755 *
./autogen.sh
./configure
make -j
cp -R -f .libs/*.so* ../lib/Debug/
cp -R -f .libs/*.so* ../lib/Release/
cd ../../
##################################################################
echo Building evpp...

if [ -d "evpp" ]; then
    rm -rf evpp
fi

git clone -b master https://github.com/ArkGame/evpp.git
cd evpp
git submodule update --init --recursive
chmod -R 755 *
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make -j
#copy lib\Debug\*.lib ..\..\lib\Debug /Y
#copy lib\Release\*.lib ..\..\lib\Release /Y

cd ../../


# extract libevent libs
#echo Extract libevent libs
#tar -xzvf libevent-2.0.22-stable.tar.gz
#if [ -d ./libevent ]; then
# rm -rf ./libevent
#fi

#mv libevent-2.0.22-stable libevent

# compiling libevent
#cd libevent
#chmod -R 755 *
#./configure CPPFLAGS=-fPIC --disable-shared --disable-openssl
#make

#cp -R -f .libs/*.a ../lib/Debug/
#cp -R -f .libs/*.a ../lib/
#cp -R -f .libs/*.a ../lib/Release/
#cd ../

# compiling protobuf
#cd protobuf
#chmod -R 755 *
#./configure CXXFLAGS=-fPIC
#make
#make check

#if [ ! -d ../../Bin/Comm/ ]; then
# mkdir ../../Bin/Comm/
#fi
#if [ ! -d ../../Bin/Comm/Debug/ ]; then
# mkdir ../../Bin/Comm/Debug/
#fi
#if [ ! -d ../../Bin/Comm/Release/ ]; then
# mkdir ../../Bin/Comm/Release/
#fi

#cp -r -f ./src/.libs/*.so ../lib/Debug/
#cp -r -f ./src/.libs/*.so.* ../lib/Debug/
#cp -r -f ./src/.libs/*.so ../lib/Release/
#cp -r -f ./src/.libs/*.so.* ../lib/Release/
#cp -r -f ./src/.libs/*.so ../../Bin/Comm/Debug/
#cp -r -f ./src/.libs/*.so.* ../../Bin/Comm/Debug/
#cp -r -f ./src/.libs/*.so ../../Bin/Comm/Release/
#cp -r -f ./src/.libs/*.so.* ../../Bin/Comm/Release/

#cp -r -f ./src/.libs/*.so ../lib/
#cp -r -f ./src/.libs/*.so.* ../lib/
#cd ../


# TODO: other libs

# back to main dir
#pwd
#'
