echo "Building dependencies..."

if [ -d "lib" ]; then
    rm -rf lib
fi

mkdir -p lib

#####################################################################
echo "Building protobuf..."

if [ -d "protobuf" ]; then
    rm -rf protobuf
fi

git clone https://github.com/protocolbuffers/protobuf.git --depth 1
cd protobuf/cmake
mkdir build && cd build
cmake -G "Unix Makefiles" -Dprotobuf_BUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=release -Dprotobuf_BUILD_TESTS=OFF ..
make -j 4
cp -a *.so* ../../../lib

cp -a *.so ../../../../Bin/lib
cp -a *.so ../../../../Frame/SDK/ProtoPlugin/proto-gen
cp -a protoc* ../../../../Frame/SDK/ProtoPlugin/proto-gen

cd ../../../

##################################################################
echo "Building brynet..."

if [ -d "brynet" ]; then
    rm -rf brynet
fi

git clone -b master https://github.com/ArkGame/brynet.git --depth 1
cd brynet
chmod -R 755 *
mkdir build && cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=release ..
make -j 4
cp -R -f lib/*.a ../../lib
cd ../../

##################################################################
echo "General proto files..."
cd ../Frame/SDK/ProtoPlugin/
bash gen-proto.sh
cd ../../../
