#!/bin/bash
# Make a minimal build.
# libtexconv.so or libtexconv.dylib will be generated in ./Texconv-Custom-DLL/

pushd $(dirname "$0")/../
mkdir build
cd build
cmake \
  -D CMAKE_BUILD_TYPE=Release\
  -D BUILD_DX11=OFF\
  -D BUILD_DX12=OFF\
  -D CMAKE_POSITION_INDEPENDENT_CODE=ON\
  -D BC_USE_OPENMP=OFF\
  ../
cmake --build .
cp bin/CMake/libtexconv.* ../
popd