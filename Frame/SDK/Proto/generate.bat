
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFDefine.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFMsgBase.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFMsgPreGame.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFMsgShare.proto  
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFSLGDefine.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFFleetingDefine.proto  
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFMsgURl.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./AFMsgMysql.proto

"proto-gen/protogen.exe" -i:AFDefine.proto -o:AFDefine.cs
"proto-gen/protogen.exe" -i:AFMsgBase.proto -o:AFMsgBase.cs
"proto-gen/protogen.exe" -i:AFMsgPreGame.proto -o:AFMsgPreGame.cs
"proto-gen/protogen.exe" -i:AFMsgShare.proto -o:AFMsgShare.cs
"proto-gen/protogen.exe" -i:AFSLGDefine.proto -o:AFSLGDefine.cs
"proto-gen/protogen.exe" -i:AFFleetingDefine.proto -o:AFFleetingDefine.cs
"proto-gen/protogen.exe" -i:AFMsgURl.proto -o:AFMsgURl.cs

pause