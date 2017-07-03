
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFDefine.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFMsgBase.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFMsgPreGame.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFMsgShare.proto  
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFSLGDefine.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFFleetingDefine.proto  
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFMsgURl.proto 
"proto-gen/protoc.exe" -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ ./NFMsgMysql.proto 

pause