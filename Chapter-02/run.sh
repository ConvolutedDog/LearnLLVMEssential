#! /bin/bash

clang++ -O3 toy.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o toy
./toy
