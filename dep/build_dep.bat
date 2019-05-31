@echo off

REM If your path is different with below, please change to your install path
REM ========================================================================
REM Visual Sutido 2019 professional
set VS_DEVENV="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\Tools\..\IDE\Devenv"
set CMAKE_CMD=cmake -G "Visual Studio 16" -A x64
REM ========================================================================
echo %VS_DEVENV%
echo %CMAKE_CMD%

REM ######################################################################################################

echo "Building dependencies..."

if exist lib (rd lib /q /s)
md lib
cd lib
cd ../

echo "Building protobuf..."

if exist protobuf (rd protobuf /q /s)
git clone https://github.com/protocolbuffers/protobuf.git --depth 1

cd protobuf/cmake
md build
cd build
%CMAKE_CMD% -Dprotobuf_BUILD_SHARED_LIBS=OFF -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_MSVC_STATIC_RUNTIME=OFF ..
%VS_DEVENV% protobuf.sln /build "Debug|x64"
%VS_DEVENV% protobuf.sln /build "Release|x64"
REM copy Debug\*.dll ..\..\..\lib /Y
REM copy Release\*.dll ..\..\..\lib /Y
copy Debug\*.lib ..\..\..\lib /Y
copy Release\*.lib ..\..\..\lib /Y

copy Debug\libprotobufd.dll ..\..\..\..\bin\lib /Y
copy Release\libprotobuf.dll ..\..\..\..\bin\lib /Y
copy Release\libprotobuf.dll ..\..\..\..\frame\proto\proto-gen /Y
copy Release\libprotoc.dll ..\..\..\..\frame\proto\proto-gen /Y
copy Release\protoc.exe ..\..\..\..\frame\proto\proto-gen /Y

cd ..\..\..\

REM ####################################################################################################
echo "Building brynet..."
if exist brynet (rd brynet /q /s)
git clone -b master https://github.com/ArkNX/brynet.git --depth 1

cd brynet
md build
cd build
%CMAKE_CMD% ..
%VS_DEVENV% brynet.sln /build "Debug|x64" /project brynet.vcxproj
%VS_DEVENV% brynet.sln /build "Release|x64" /project brynet.vcxproj
copy lib\Debug\*.lib ..\..\lib\ /Y
copy lib\Release\*.lib ..\..\lib /Y

cd ..\..\
REM ####################################################################################################
REM generate pb files
echo "generate proto files..."
cd ..\frame\proto\
call gen-proto.bat
cd ..\..\

pause