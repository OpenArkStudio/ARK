@echo off

echo Building dependencies...

echo Building evpp...


cd evpp
cd vsprojects
"%VS140COMNTOOLS%..\IDE\Devenv" libevpp.sln /build "Debug|x64" /project libevpp.vcxproj
"%VS140COMNTOOLS%..\IDE\Devenv" libevpp.sln /build "Release|x64" /project libevpp.vcxproj
cd ..\..\
REM ####################################################################################################
REM back to root dir
cd ../