echo "generate protobuff files"

for /f "delims=" %%i in ('dir /b/a-d/oN *.proto') do "proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./cpp/ ./%%~ni.proto
for /f "delims=" %%i in ('dir /b/a-d/oN *.proto') do "proto-gen/protoc.exe" -I=./ --csharp_out=./cs/ ./%%~ni.proto
for /f "delims=" %%i in ('dir /b/a-d/oN *.proto') do "proto-gen/protoc.exe" -I=./ --js_out=./js/ ./%%~ni.proto

pause