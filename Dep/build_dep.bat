@echo off

echo Building Dep...

echo Compiling libevent
"..\\Tool\\7z.exe" x libevent-release-2.1.8-stable.zip -Y 

rd libevent  /q /s
rename libevent-release-2.1.8-stable libevent

echo Compiling evpp
"..\\Tool\\7z.exe" x evpp.zip -Y -o./evpp

echo Compiling glog
"..\\Tool\\7z.exe" x glog-0.3.5.zip -Y 

rd glog  /q /s
rename glog-0.3.5 glog



