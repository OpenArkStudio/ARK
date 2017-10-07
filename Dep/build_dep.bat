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
echo Building glog...
if exist glog (rd glog /q /s)
git clone -b v035 https://github.com/google/glog.git

cd glog
md build
cd build
cmake -G "Visual Studio 14 Win64" ..
"%VS140COMNTOOLS%..\IDE\Devenv" google-glog.sln /build "Debug|x64"
"%VS140COMNTOOLS%..\IDE\Devenv" google-glog.sln /build "Release|x64"
copy Debug\*.lib ..\..\lib\Debug /Y
copy Release\*.lib ..\..\lib\Release /Y
cd ..\..\
REM ######################################################################################################
echo Building evpp...

if exist evpp (rd evpp /q /s)
git clone -b master https://github.com/ArkGame/evpp.git

cd evpp
cd vsprojects
"%VS140COMNTOOLS%..\IDE\Devenv" libevpp.sln /build "Debug|x64" /project evpp_static.vcxproj
"%VS140COMNTOOLS%..\IDE\Devenv" libevpp.sln /build "Release|x64" /project evpp_static.vcxproj
cd ..\..\
REM ####################################################################################################
REM back to root dir
cd ../