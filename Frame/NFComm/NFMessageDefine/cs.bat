echo on
"proto-gen/protogen" -i:NFDefine.proto -o:NFDefine.cs
"proto-gen/protogen" -i:NFMsgBase.proto -o:NFMsgBase.cs
"proto-gen/protogen" -i:NFMsgPreGame.proto -o:NFMsgPreGame.cs
"proto-gen/protogen" -i:NFMsgShare.proto -o:NFMsgShare.cs
"proto-gen/protogen" -i:NFSLGDefine.proto -o:NFSLGDefine.cs
"proto-gen/protogen" -i:NFFleetingDefine.proto -o:NFFleetingDefine.cs
"proto-gen/protogen" -i:NFMsgURl.proto -o:NFMsgURl.cs

pause