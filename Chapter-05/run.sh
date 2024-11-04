#! /bin/bash

opt -passes="licm" -S ./licm.ll

opt -passes="print<scalar-evolution>" -S ./scalevl.ll

clang -emit-llvm -S intrinsic.cpp
opt -O1 -S intrinsic.ll > O1-Optimized-intrinsic.ll

opt -S -passes=slp-vectorizer -mtriple=x86_64-unknown-linuxgnu -mcpu=generic intrinsic.ll -debug
opt -S -passes='default<O3>,slp-vectorizer' -mtriple=x86_64-unknown-linuxgnu -mcpu=generic intrinsic.ll
