#!/bin/bash

echo "generate protobuff files"

proto_path=./proto

for file in ${proto_path}/*.proto
do
	proto-gen/protoc -I=${proto_path} --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./cpp/ $file
done