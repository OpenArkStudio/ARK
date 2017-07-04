
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFDefine.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFMsgBase.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFMsgPreGame.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFMsgShare.proto  
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFSLGDefine.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFFleetingDefine.proto  
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFMsgURl.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFMsgMysql.proto

"proto-gen/protogen.exe" -i:NFDefine.proto -o:NFDefine.cs
"proto-gen/protogen.exe" -i:NFMsgBase.proto -o:NFMsgBase.cs
"proto-gen/protogen.exe" -i:NFMsgPreGame.proto -o:NFMsgPreGame.cs
"proto-gen/protogen.exe" -i:NFMsgShare.proto -o:NFMsgShare.cs
"proto-gen/protogen.exe" -i:NFSLGDefine.proto -o:NFSLGDefine.cs
"proto-gen/protogen.exe" -i:NFFleetingDefine.proto -o:NFFleetingDefine.cs
"proto-gen/protogen.exe" -i:NFMsgURl.proto -o:NFMsgURl.cs

pause