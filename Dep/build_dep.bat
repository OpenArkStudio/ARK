@echo off

echo Building dependencies...

if exist lib (rd lib /q /s)
md lib
cd lib
md Debug
md Release
cd ../
REM ######################################################################################################
echo Building protobuf...

if exist protobuf (rd protobuf /q /s)
git clone -b 2.7.0 https://github.com/google/protobuf.git

cd protobuf/cmake
md build
cd build
cmake -G "Visual Studio 14 Win64" -Dprotobuf_BUILD_SHARED_LIBS=ON -Dprotobuf_BUILD_TESTS=OFF ..
"%VS140COMNTOOLS%..\IDE\Devenv" protobuf.sln /build "Debug|x64"
"%VS140COMNTOOLS%..\IDE\Devenv" protobuf.sln /build "Release|x64"
copy Debug\*.dll ..\..\..\lib\Debug /Y
copy Debug\*.lib ..\..\..\lib\Debug /Y
copy Release\*.dll ..\..\..\lib\Release /Y
copy Release\*.lib ..\..\..\lib\Release /Y

copy Release\libprotobuf.dll ..\..\..\..\Frame\SDK\Proto\proto-gen /Y
copy Release\libprotoc.dll ..\..\..\..\Frame\SDK\Proto\proto-gen /Y
copy Release\protoc.exe ..\..\..\..\Frame\SDK\Proto\proto-gen /Y

cd ..\..\..\
REM ######################################################################################################
echo Building libevent...

if exist libevent (rd libevent /q /s)
git clone -b master https://github.com/libevent/libevent.git

cd libevent
md build
cd build
cmake -G "Visual Studio 14 Win64" -DEVENT__DISABLE_OPENSSL=ON ..
"%VS140COMNTOOLS%..\IDE\Devenv" libevent.sln /build "Debug|x64" /project event_core_static.vcxproj
"%VS140COMNTOOLS%..\IDE\Devenv" libevent.sln /build "Release|x64" /project event_core_static.vcxproj
"%VS140COMNTOOLS%..\IDE\Devenv" libevent.sln /build "Debug|x64" /project event_static.vcxproj
"%VS140COMNTOOLS%..\IDE\Devenv" libevent.sln /build "Release|x64" /project event_static.vcxproj
"%VS140COMNTOOLS%..\IDE\Devenv" libevent.sln /build "Debug|x64" /project event_extra_static.vcxproj
"%VS140COMNTOOLS%..\IDE\Devenv" libevent.sln /build "Release|x64" /project event_extra_static.vcxproj
copy lib\Debug\*.lib ..\..\lib\Debug /Y
copy lib\Release\*.lib ..\..\lib\Release /Y
cd ..\..\
REM ######################################################################################################
echo Building evpp...
if exist evpp (rd evpp /q /s)
git clone -b master https://github.com/ArkGame/evpp.git

cd evpp
md build
cd build
cmake -G "Visual Studio 14 Win64" -DLIBEVENT_DIR=..\libevent -DLIBEVENT_LIB_DIR=..\lib\Release ..
"%VS140COMNTOOLS%..\IDE\Devenv" evpp.sln /build "Debug|x64" /project evpp/evpp_static.vcxproj
"%VS140COMNTOOLS%..\IDE\Devenv" evpp.sln /build "Release|x64" /project evpp/evpp_static.vcxproj
copy lib\Debug\*.lib ..\..\lib\Debug /Y
copy lib\Release\*.lib ..\..\lib\Release /Y

cd ..\..\
REM ####################################################################################################
REM back to root dir
cd ..\