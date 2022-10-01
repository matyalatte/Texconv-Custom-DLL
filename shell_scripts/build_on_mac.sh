#!/bin/bash
#Builds texconv with cmake.
#libtexconv.so will be generated in Texconv-Custom-DLL/

pushd $(dirname "$0")/../
mkdir build
cd build
cmake \
  -D CMAKE_BUILD_TYPE=Release\
  -D BUILD_DX11=OFF\
  -D BUILD_DX12=OFF\
  -D CMAKE_POSITION_INDEPENDENT_CODE=ON\
  ../
cmake --build .
cp bin/CMake/libtexconv.dylib ../
popd