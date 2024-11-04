#! /bin/bash

mkdir build
cd build
cmake .. --fresh
make
opt -load-pass-plugin PassesDir/libFnNamePrintPass.so -passes="funcnameprint" ../test.ll

opt -O2 -S -debug-pass-manager -S ../test.ll
opt -O2 -S -debug-pass-manager -stats -time-passes -S ../test.ll

opt -load-pass-plugin PassesDir/libFnNamePrintPass.so -passes="funcnameprint" -debug-pass-manager -S ../test.ll
opt -load-pass-plugin PassesDir/libFnNamePrintPass.so -passes="funcnameprint" -debug-pass-manager -stats -time-passes -S ../test.ll

opt -passes="instcombine" -S ../test.ll
