echo "generate protobuff files"

for /f "delims=" %%i in ('dir /b/a-d/oN *.proto') do "proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:../ %%~ni.proto
for /f "delims=" %%i in ('dir /b/a-d/oN *.proto') do "proto-gen/protogen.exe" -i:%%i -o:cs/%%~ni.cs

pause