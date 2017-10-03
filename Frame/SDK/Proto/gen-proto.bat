
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFDefine.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFMsgBase.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFMsgPreGame.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFMsgShare.proto  
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFSLGDefine.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFFleetingDefine.proto  
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFMsgURl.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFMsgMysql.proto

"proto-gen/protogen.exe" -i:AFDefine.proto -o:cs/AFDefine.cs
"proto-gen/protogen.exe" -i:AFMsgBase.proto -o:cs/AFMsgBase.cs
"proto-gen/protogen.exe" -i:AFMsgPreGame.proto -o:cs/AFMsgPreGame.cs
"proto-gen/protogen.exe" -i:AFMsgShare.proto -o:cs/AFMsgShare.cs
"proto-gen/protogen.exe" -i:AFSLGDefine.proto -o:cs/AFSLGDefine.cs
"proto-gen/protogen.exe" -i:AFFleetingDefine.proto -o:cs/AFFleetingDefine.cs
"proto-gen/protogen.exe" -i:AFMsgURl.proto -o:cs/AFMsgURl.cs

pause