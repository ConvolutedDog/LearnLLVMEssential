#! /bin/bash

mkdir build
cd build
cmake .. --fresh
make
opt -load-pass-plugin PassesDir/libFnNamePrintPass.so -passes="funcnameprint" ../test.ll
