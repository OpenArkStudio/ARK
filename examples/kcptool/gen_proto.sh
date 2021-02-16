#! /bin/bash

function py_protocol()
{
    protoc -I /Users/qinhan/c++/ARK/src/proto/proto \
    --python_out=./ \
    /Users/qinhan/c++/ARK/src/proto/proto/AFPhotonSS.proto
}

py_protocol