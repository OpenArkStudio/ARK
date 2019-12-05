echo "Building dependencies..."

if [ -d "lib" ]; then
    rm -rf lib
fi

mkdir -p lib

#####################################################################
echo "Building protobuf..."

cd protobuf/cmake
if [ -d "build" ]; then
    rm -rf build
fi
mkdir build && cd build
cmake -G "Unix Makefiles" -Dprotobuf_BUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=release -Dprotobuf_BUILD_TESTS=OFF ..
make -j 4
cp -a *.so* ../../../lib

cp -a *.so* ../../../../bin/bin
cp -a *.so* ../../../../src/proto/proto-gen
cp -a protoc* ../../../../src/proto/proto-gen

cd ../../../

##################################################################
#echo "Building brynet..."
#
#cd brynet
#chmod -R 755 *
#if [ -d "build" ]; then
#    rm -rf build
#fi
#mkdir build && cd build
#cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=release ..
#make -j 4
#cp -a lib/*.so ../../lib
#cp -a lib/*.so* ../../../bin/bin
#cd ../../
#
##################################################################
echo "General proto files..."
cd ../src/proto/
bash gen-proto.sh
cd ../../
