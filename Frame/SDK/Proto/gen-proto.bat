echo "generate protobuff files"

set proto_path=proto

for /f "delims=" %%i in ('dir /b/a-d/oN "%proto_path%\*.proto"') do "proto-gen/protoc.exe" --proto_path=%proto_path% --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./cpp/ %proto_path%\%%i
for /f "delims=" %%i in ('dir /b/a-d/oN "%proto_path%\*.proto"') do "proto-gen/protoc.exe" --proto_path=%proto_path% --csharp_out=./cs/ %proto_path%\%%i
for /f "delims=" %%i in ('dir /b/a-d/oN "%proto_path%\*.proto"') do "proto-gen/protoc.exe" --proto_path=%proto_path% --js_out=./js/ %proto_path%\%%i

pause