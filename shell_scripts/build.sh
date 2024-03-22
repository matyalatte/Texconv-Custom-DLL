#!/bin/bash
# Builds texconv with cmake.
# libtexconv.so or libtexconv.dylib will be generated in ./Texconv-Custom-DLL/

pushd $(dirname "$0")/../
mkdir build
cd build
cmake \
  -D CMAKE_BUILD_TYPE=Release\
  -D CMAKE_POSITION_INDEPENDENT_CODE=ON\
  -D TEXCONV_USE_ALL=ON\
  ../
cmake --build .
cp lib/libtexconv.* ../
popd
