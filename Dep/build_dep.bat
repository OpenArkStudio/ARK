@echo off

echo Building dependencies...

if exist lib (rd lib /f /q /s)
md lib
REM ######################################################################################################
echo Building protobuf...

if exist protobuf (rd protobuf /q /s)
git clone -b 2.7.0 https://github.com/google/protobuf.git

cd protobuf/cmake
md build
cd build
cmake -G "Visual Studio 14 Win64" -Dprotobuf_BUILD_SHARED_LIBS=ON -Dprotobuf_BUILD_TESTS=OFF ..
"%VS140COMNTOOLS%..\IDE\Devenv" protobuf.sln /build "Release|x64"
copy Release\*.dll ..\..\..\lib /Y
copy Release\*.lib ..\..\..\lib /Y
cd ..\..\..\
REM ######################################################################################################
echo Building libevent...

if exist libevent (rd libevent /q /s)
git clone -b master https://github.com/libevent/libevent.git

cd libevent
md build
cd build
cmake -G "Visual Studio 14 Win64" -DEVENT__DISABLE_OPENSSL=ON ..
"%VS140COMNTOOLS%..\IDE\Devenv" libevent.sln /build "Release|x64"
copy lib\Release\*.lib ..\..\lib /Y
cd ..\..\
REM ######################################################################################################
echo Building glog...
if exist glog (rd glog /q /s)
git clone -b v035 https://github.com/google/glog.git

cd glog
md build
cd build
cmake -G "Visual Studio 14 Win64" ..
"%VS140COMNTOOLS%..\IDE\Devenv" google-glog.sln /build "Release|x64"
copy Release\*.lib ..\..\lib /Y
cd ..\..\
REM ######################################################################################################
echo Building evpp...

if exist evpp (rd evpp /q /s)
git clone -b master https://github.com/Qihoo360/evpp.git

cd evpp
md build
cd build
git clone https://github.com/Microsoft/vcpkg.git 
cd vcpkg
call bootstrap-vcpkg.bat
vcpkg install evpp:x64-windows

copy packages\evpp_x64-windows\lib\evpp*.lib ..\..\..\lib /Y
cd ..\..\..\
REM ####################################################################################################
REM back to root dir
cd ../