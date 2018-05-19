#!/bin/bash

echo "generate protobuff files"

for file in ./*.proto
do
	proto-gen/protoc -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./cpp/ ./$file
done

